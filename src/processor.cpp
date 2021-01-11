#include <QStringList>
#include <QDebug>
#include <QFile>

#include <stdexcept>

#include "processor.h"
#include "motif.h"
#include "global.h"

TabliatoProcessor::TabliatoProcessor(Tabulature &tabulature)
{
    tab = &tabulature;

    keyboard.buildKeyboard(tab->get("accordion"));

    parseMusic();
    parseLyric();
    generateLilypondCode();
}

void TabliatoProcessor::parseMusic()
{
    // A button holds the logic of parsing the notes and stores some context
    Button::direction = "p";
    Button::duration = "4";
    ButtonParser button(keyboard);
    MultiButtonParser multiButton(keyboard);

    QString tabulature = tab->tabulature;
    tabulature.replace("\t", " ");            // Remplace les tabulations par des espaces
    tabulature.replace("~", " ~");            // Ajoute un espace avant les ~ des liaisons des notes
    tabulature.replace("  ~", " ~");          // Supprime les espaces ajoutés en trop
    tabulature.replace("\n", " newline ");    // Protection des retours chariots pour linéariser le contenu
    tabulature.replace("volta 2", "volta:2"); // Protection d'un cas particulier
    tabulature.replace("<", " < ");
    tabulature.replace(">", " >");
    tabulature.replace("[", " [ ");
    tabulature.replace("]", " ] ");
    tabulature.replace("}", " } ");
    tabulature.replace("{", " { ");

    Motif pattern(tab->get("time"), tab->get("motif").toInt());

    QStringList symbols = tabulature.split(" ");         // Séparation du contenu à chaque espace pour analyser les éléments un par un
    QStringList parsedSymbolsMelody;
    QStringList parsedSymbolsBass;

    int line = 1;

    try
    {
        // Initialisation des états. Le parseur s'occupe des spanners
        // de tenu des notes
        bool spannerIsOpen = false;
        bool spannerMustBeClosed = false;

        // Loop throught all symbols
        for (int i = 0 ; i < symbols.size() ; i++)
        {
            QString symbol = symbols[i];
            QString parsed = symbol;

            int type = getType(symbol);
            bool closeSpanner = false;
            bool openSpanner = false;
            bool markup = true;

            // Séparation la séparation main D/G.
            bool currentSymbolIsMelody = true;
            bool currentSymbolIsBass = true;

            switch(type)
            {
            case NEWLINE:
                line++;
                break;

            case BUTTON:
                currentSymbolIsBass = false;

                button.setButton(symbol);

                if (spannerIsOpen && spannerMustBeClosed)
                {
                    spannerIsOpen = false;
                    spannerMustBeClosed = false;
                    closeSpanner = true;
                }
                else if (spannerIsOpen && !spannerMustBeClosed)
                {
                    spannerMustBeClosed = true;
                    markup = false;
                    log("Suppression des markups: liaison de notes");
                }
                else
                {
                    spannerMustBeClosed = true;
                }

                if (button.getDurationAsNumber() > 1 && !spannerIsOpen)
                {
                    spannerIsOpen = true;
                    spannerMustBeClosed = true;
                    openSpanner = true;
                }

                parsed = button.print(markup);
                break;

            case DIRECTION:
                Button::direction = getDirection(symbol);
                parsed = "";
                break;

            case MELODYCHORD:
                currentSymbolIsBass = false;

                multiButton.setMultiButton(symbol);

                if (spannerIsOpen && spannerMustBeClosed)
                {
                    spannerIsOpen = false;
                    spannerMustBeClosed = false;
                    closeSpanner = true;
                }
                else if (spannerIsOpen && !spannerMustBeClosed)
                {
                    spannerMustBeClosed = true;
                    markup = false;
                    log("Suppression des markups: liaison d'accord");
                }
                else
                {
                    spannerMustBeClosed = true;
                }

                if (multiButton.getDurationAsNumber() > 1 && !spannerIsOpen)
                {
                    spannerIsOpen = true;
                    spannerMustBeClosed = true;
                    openSpanner = true;
                }

                parsed = multiButton.print(markup);
                break;

            case COMMENT:
                while(symbols[i] != "newline" && i < symbols.size()) i++;
                parsed = "newline";
                break;

            case TIE:
                currentSymbolIsBass = false;

                if (spannerIsOpen)
                {
                    spannerMustBeClosed = false;
                }
                else
                {
                    spannerIsOpen = true;
                    spannerMustBeClosed = false;
                    parsed = "\\startTextSpan ~";
                }
                break;

            case REST:
                currentSymbolIsBass = false;

                if (spannerIsOpen)
                {
                    spannerMustBeClosed = false;
                    spannerIsOpen = false;
                    closeSpanner = true;
                }
                break;

            case OPENCHORD:
            {
                currentSymbolIsBass = false;

                QStringList tmp;
                while (!isCloseChord(symbols[i]) && i < symbols.size())
                {
                    if (isButton(symbols[i]))
                    {
                        tmp.append(symbols[i]);
                    }
                    else if (isNote(symbols[i]))
                    {
                        button.setNote(symbols[i]);
                        tmp.append(button.button);
                    }
                    i++;
                }

                symbol = tmp.join(",");

                if (extractDuration.indexIn(symbols[i]) >= 0)
                    symbol += ":" + extractDuration.cap(1);

                multiButton.setMultiButton(symbol);

                if (spannerIsOpen && spannerMustBeClosed)
                {
                    spannerIsOpen = false;
                    spannerMustBeClosed = false;
                    closeSpanner = true;
                }
                else if (spannerIsOpen && !spannerMustBeClosed)
                {
                    spannerMustBeClosed = true;
                    markup = false;
                }
                else
                {
                    spannerMustBeClosed = true;
                }

                if (multiButton.getDurationAsNumber() > 4 && !spannerIsOpen)
                {
                    spannerIsOpen = true;
                    spannerMustBeClosed = true;
                    openSpanner = true;
                }

                parsed = multiButton.print();

                break;
            }
            case OPENMANUALBASS:
            {
                bool interpolate = false;
                currentSymbolIsMelody = false;

                QStringList tmp;
                while (!isCloseManualBass(symbols[i]) && i < symbols.size())
                {
                    if (isExplicitBass(symbols[i]))
                    {
                       interpolate = false;
                       tmp.append(pattern.parseExplicitBass(symbols[i]));
                    }

                    if (isImplicitBass(symbols[i]))
                    {
                        interpolate = true;
                        tmp.append(symbols[i]);
                    }

                    if (isRest(symbols[i]))
                    {
                        tmp.append(symbols[i]);
                    }

                    i++;
                };


                if (interpolate)
                {
                    parsed = tmp.join(":");
                    parsed = pattern.parseBassSet(parsed);
                }
                else
                {
                    parsed = tmp.join(" ");
                }
                break;
            }
            case NOTE:
            {
                currentSymbolIsBass = false;

                button.setNote(symbol);

                if (spannerIsOpen && spannerMustBeClosed)
                {
                    spannerIsOpen = false;
                    spannerMustBeClosed = false;
                    closeSpanner = true;
                }
                else if (spannerIsOpen && !spannerMustBeClosed)
                {
                    spannerMustBeClosed = true;
                    markup = false;
                }
                else
                {
                    spannerMustBeClosed = true;
                }

                if (button.getDurationAsNumber() > 4 && !spannerIsOpen)
                {
                    spannerIsOpen = true;
                    spannerMustBeClosed = true;
                    openSpanner = true;
                }

                parsed = button.print(markup);
                break;
            }
            case BASSCHORD:
                currentSymbolIsMelody = false;
                parsed = pattern.parseBassSet(symbol); // Transforms G:g:g in G g g and check compliance with motif B a a
                break;

            case IMPLICITBASS:
                currentSymbolIsMelody = false;
                parsed = pattern.parseBass(symbol); // Transforms G in G g g if motif is B a a
                break;

            case EXPLICITBASS:
                currentSymbolIsMelody = false;
                parsed = pattern.parseExplicitBass(symbol); // Detect manual G:4 g:4
                break;
            case COMMAND:
                 if (symbol == "\\break") currentSymbolIsBass = false;
                 if (symbol == "\\motif")
                 {
                     currentSymbolIsBass = false;
                     currentSymbolIsMelody = false;

                     while (symbols[i] != "[" && i < symbols.size()) i++;
                     i++;

                     QStringList tmp;
                     while (!isCloseManualBass(symbols[i]) && i < symbols.size())
                     {
                       tmp.append(symbols[i]);
                       i++;
                     }
                     pattern = Motif(tmp.join(" "));
                 }
                break;
            default:
                break;
            }

            if (currentSymbolIsMelody)
            {
                parsedSymbolsMelody.append(parsed);
                if (closeSpanner) parsedSymbolsMelody.append("\\stopTextSpan");
                if (openSpanner) parsedSymbolsMelody.append("\\startTextSpan");
            }

            if (currentSymbolIsBass)
            {
                parsedSymbolsBass.append(parsed);
            }


            // logs
            if (symbol != "") log("Symbol: " + symbol + " | type: " + QString::number(type) + " | parsed: " + parsed);
        }

        if (spannerMustBeClosed) parsedSymbolsMelody.append(QString("\\endSpanners \\stopTextSpan"));
    }
    catch(std::exception &e)
    {
        QString err = "ligne " + QString::number(line) + " - " + e.what();
        throw std::logic_error(err.toStdString());
    }

    tab->melody = parsedSymbolsMelody.join(" ");
    tab->melody.replace(QRegExp("newline"), "\n");
    tab->melody.replace(QRegExp(":"), "");

    tab->bass = parsedSymbolsBass.join(" ");
    tab->bass.replace(QRegExp("newline"), "\n");
    tab->bass.replace(QRegExp(":"), "");
}

void TabliatoProcessor::parseLyric()
{
    QStringList list;
    QStringList list2;
    list = tab->lyrics.split("\n\n");
    int n = list.size();
    int hn = n/2;

    tab->firstVerse = list[0];

    if (n > 1)
    {
        for(int i = 1 ; i <= hn ; i++)
        {
            list2 = list[i].split("\n");

            for(int j = 0 ; j < list2.size() ; j++)
                tab->leftVerses = tab->leftVerses + "\\line{ " + list2[j] + "} \n";

            tab->leftVerses = tab->leftVerses +  "\\vspace #1 \n";
        }

        for(int i = hn+1 ; i < n ; i++)
        {
            list2 = list[i].split("\n");

            for(int j = 0 ; j < list2.size() ; j++)
                tab->rightVerses = tab->rightVerses + "\\line{ " + list2[j] + "} \n";

            tab->rightVerses = tab->rightVerses +  "\\vspace #1 \n";
        }
    }
}

void TabliatoProcessor::generateLilypondCode()
{
    QString cadb;
    QString fingerDisplay;
    QString bassDisplay;
    QString lilypondCode;

    cadb = (tab->get("system") != "cadb") ? "f" : "t";
    fingerDisplay = (tab->get("fingerDisplay") != "true") ? "f" : "t";
    bassDisplay = (tab->bass != "") ? "t" : "f";

    QFile file(TEMPLATE + "/tabliato.ly");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::logic_error(QString("Impossible d'ouvrir le fichier : " + TEMPLATE + "/tabliato.ly").toStdString() );
    lilypondCode = file.readAll();
    file.close();

    lilypondCode.replace(QRegExp("<basses>"), tab->bass);
    lilypondCode.replace(QRegExp("<tablature>"), tab->melody);
    lilypondCode.replace(QRegExp("<title>"), tab->get("title"));
    lilypondCode.replace(QRegExp("<subtitle>"), tab->get("subtitle"));
    lilypondCode.replace(QRegExp("<composer>"), tab->get("composer"));
    lilypondCode.replace(QRegExp("<poet>"), tab->get("poet"));
    lilypondCode.replace(QRegExp("<tagline>"), tab->get("tagline"));
    lilypondCode.replace(QRegExp("<instrument>"), tab->get("instrument"));
    lilypondCode.replace(QRegExp("<tempo>"), tab->get("tempo"));
    lilypondCode.replace(QRegExp("<time>"), tab->get("time"));
    lilypondCode.replace(QRegExp("<key>"), tab->get("key"));
    lilypondCode.replace(QRegExp("<system>"), cadb);
    lilypondCode.replace(QRegExp("<fingerDisplay>"), fingerDisplay);
    lilypondCode.replace(QRegExp("<bassDisplay>"), bassDisplay);
    lilypondCode.replace(QRegExp("<firstVerse>"), tab->firstVerse);
    lilypondCode.replace(QRegExp("<leftLyrics>"), tab->leftVerses);
    lilypondCode.replace(QRegExp("<rightLyrics>"), tab->rightVerses);

    tab->lilypond = lilypondCode;
}

QString TabliatoProcessor::getLogs()
{
    return logs.join(" \n");
}

void TabliatoProcessor::log(QString str)
{
    logs.append(str);
}
