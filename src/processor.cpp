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

    // A motif holds the logic of parsing the left hand button into something that corresponds to the rythm
    Motif pattern(tab->get("time"), tab->get("motif").toInt());

    // Protections préliminaires
    QString tabulature = tab->tabulature;
    tabulature.replace("\t", " ");            // Remplace les tabulations par des espaces
    tabulature.replace("~", " ~");            // Ajoute un espace avant les ~ des liaisons des notes
    tabulature.replace("  ~", " ~");          // Supprime les espaces ajoutés en trop
    tabulature.replace("\n", " newline ");    // Protection des retours chariots pour linéariser le contenu
    tabulature.replace("volta 2", "volta:2"); // Protection d'un cas particulier
    tabulature.replace("<<", " \\doubelt ");   // Protection de << pour les contre chants
    tabulature.replace(">>", " \\doubegt ");   // Protection de >> pour les contre chants
    tabulature.replace("<", " < ");
    tabulature.replace(">", " >");
    tabulature.replace("[", " [ ");
    tabulature.replace("]", " ] ");
    tabulature.replace("}", " } ");
    tabulature.replace("{", " { ");

    QStringList symbols = tabulature.split(" "); // Séparation du contenu à chaque espace pour analyser les éléments un par un
    QStringList parsedSymbolsMelody;             // Certain symboles vont dans la mélodie
    QStringList parsedSymbolsBass;               // Certain symboles vont dans l'accompagnement

    int line = 1;
    int nnote = 0;
    int nbass = 0;

    try
    {
        // Initialisation des états. Le parseur s'occupe des spanners de tenu des notes
        // Si on trouve un note plus longue que blanche alors on inserer ouvrir un testSpanner
        // avant la note et le fermer plus tard. Une liaison à le même effet. On peut aussi
        // avoir des spanner à l'accompagnement dans de rares cas. Exemple G:2.
        bool spannerIsOpen = false;
        bool spannerMustBeClosed = false;

        // Loop throught all symbols of the code
        for (int i = 0 ; i < symbols.size() ; i++)
        {
            QString symbol = symbols[i];
            QString parsed = symbol;

            int type = getType(symbol);

            bool closeSpanner = false;
            bool openSpanner = false;
            bool markup = true;

            // Séparation de main D/G
            bool currentSymbolIsMelody = true;
            bool currentSymbolIsBass = true;

            switch(type)
            {
            // Une nouvelle ligne. On compte pour savoir potentiellement
            // où le parseur a trouvé une erreur
            case NEWLINE:
                line++;
                break;

            // Un commentaire % on parse et on skip jusqu'à la prochaine ligne
            case COMMENT:
                while(symbols[i] != "newline" && i < symbols.size()) i++;
                parsed = "newline";
                break;

            // Lettre p t P T. On update l'état global courant
            case DIRECTION:
            {
                Button::direction = getDirection(symbol);
                parsed = "";
                break;
            }

            // Bouton main droite. On parse, on trouve la note, on génè le code lilypond
            // et en fonction de la durée on regarde si on doit ouvrir ou fermer des spanners
            case BUTTON:
            case NOTE:
            {
                nnote++;
                currentSymbolIsBass = false;

                if (type == BUTTON)
                    button.setButton(symbol);
                else
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
            }

            // Un liaison ~, on doit ouvrir un spanner. Sauf si il est déjà ouvert
            // auquel cas il ne faut pas le fermer
            case TIE:
            {
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
            }

            // Un silence, il faut fermer le spanner
            case REST:
            {
                currentSymbolIsBass = false;

                if (spannerIsOpen)
                {
                    spannerMustBeClosed = false;
                    spannerIsOpen = false;
                    closeSpanner = true;
                }
                break;
            }

            // symbole < d'ouverture d'accord dans la mélodie
            // On parse jusqu'au prochain > chaque symbole est une note ou un bouton
            case OPENCHORD:
            {
                nnote++;
                currentSymbolIsBass = false;

                // On lit jusqu'a la fermeture >
                QStringList tmp;
                while (!isCloseChord(symbols[i]) && i < symbols.size())
                {
                    // Si le symbole est un bouton on l'ajoute à la liste
                    if (isButton(symbols[i]))
                    {
                        tmp.append(symbols[i]);
                    }
                    // Si c'est une note on la transforme en bouton
                    else if (isNote(symbols[i]))
                    {
                        button.setNote(symbols[i]);
                        tmp.append(button.button);
                    }
                    i++;
                }

                // On joint les boutons en une seule stringavec des virgules car c'est comme
                // ca que le multibutton parse
                symbol = tmp.join(",");

                // on trouve une durée dans ce bazarre
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

                if (multiButton.getDurationAsNumber() > 1 && !spannerIsOpen)
                {
                    spannerIsOpen = true;
                    spannerMustBeClosed = true;
                    openSpanner = true;
                }

                parsed = multiButton.print(markup);

                break;
            }

            // Symbole [ de syntaxe explicite ou semi-explicite de l'accompagnement
            // On parse jusqu'au prochain ]
            case OPENMANUALBASS:
            {
                nbass++;
                bool interpolate = false;
                currentSymbolIsMelody = false;

               // On lit jusqu'a la fermeture ]
                QStringList tmp;
                while (!isCloseManualBass(symbols[i]) && i < symbols.size())
                {
                    // Basse/accord explicite e.g B:4 ou a:4
                    if (isExplicitBass(symbols[i]))
                    {
                       interpolate = false;
                       tmp.append(pattern.parseExplicitBass(symbols[i]));
                    }

                    // Basse/accord implicite e.g B ou a
                    if (isImplicitBass(symbols[i]))
                    {
                        interpolate = true;
                        tmp.append(symbols[i]);
                    }

                    // Un silence
                    if (isRest(symbols[i]))
                    {
                        tmp.append(symbols[i]);
                    }

                    i++;
                };


                if (interpolate)
                {
                    parsed = tmp.join(":");
                    parsed = pattern.parseBassSet(parsed); // Transforms G:g:g in G g g and check compliance with motif B a a
                }
                else
                {
                    parsed = tmp.join(" ");
                }

                break;
            }

             // Une lettre e.g. A ou g toute seule en dehors de []
            case IMPLICITBASS:
            {
                nbass++;
                currentSymbolIsMelody = false;
                parsed = pattern.parseBass(symbol); // Transforms G in G g g if motif is B a a
                break;
            }

            // Une lettre associée à un chiffre e.g. A:4 ou g:2 toute seule en dehors de []
            case EXPLICITBASS:
            {
                nbass++;
                currentSymbolIsMelody = false;
                parsed = pattern.parseExplicitBass(symbol); // Detect manual G:4 g:4
                break;
             }

            // Un commande latex e.g. \repeat
            // Le cas spécial \motif permer de changer l'état du parseur de motif.
            case COMMAND:
            {
                if (symbol == "\\break") currentSymbolIsBass = false;
                if (symbol == "\\motif")
                {
                    // \motif dans être suivit de e.g. [A:2 a:4 a:4] on parse cette sequence
                    currentSymbolIsBass = false;
                    currentSymbolIsMelody = false;

                    while (!isOpenManualBass(symbols[i]) && i < symbols.size()) i++;
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
            }

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
    tab->melody.replace(QRegExp("\\doublegt"), ">>");
    tab->melody.replace(QRegExp("\\doubleglt"), "<<");

    if (nbass == 0) return;

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

void TabliatoProcessor::log(QString str)
{
    logs.append(str);
}
