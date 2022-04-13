#include <QStringList>
#include <QDebug>
#include <QFile>

#include <stdexcept>

#include "processor.h"
#include "motif.h"
#include "global.h"

TabliatoProcessor::TabliatoProcessor(Tabulature &tabulature)
{
    m_tab = &tabulature;
    m_rhs_spanner_is_open = false;
    m_lhs_spanner_is_open = false;
    m_rhs_spanner_must_be_closed = false;
    m_lhs_spanner_must_be_closed = false;
    m_rhs_close_spanner = false;
    m_lhs_close_spanner = false;
    m_rhs_open_spanner = false;
    m_lhs_open_spanner = false;
    m_rhs_markup = true;

    m_keyboard.buildKeyboard(m_tab->get("accordion"));

    parseMusic();
    parseLyric();
    generateLilypondCode();
}

void TabliatoProcessor::parseMusic()
{
    // A button holds the logic of parsing the notes and stores some context
    CURRENTDIRECTION = "p";
    CURRENTDURATION = "4";

    ButtonParser button(m_keyboard);
    MultiButtonParser multiButton(m_keyboard);

    // A motif holds the logic of parsing the left hand button into something that corresponds to the rythm
    Motif pattern(m_tab->get("time"), m_tab->get("motif").toInt());

    // Timeline
    QString tempo = m_tab->get("tempo");
    Timelines timeline(tempo);

    // Protections préliminaires
    QString tabulature = m_tab->tabulature;
    tabulature.replace("\t", " ");             // Remplace les m_tabulations par des espaces
    tabulature.replace("~", " ~");             // Ajoute un espace avant les ~ des liaisons des notes
    tabulature.replace("\n", " \n ");          // Protection des retours chariots pour linéariser le contenu
    tabulature.replace("volta 2", "volta:2");  // Protection d'un cas particulier
    tabulature.replace("<<", " \\doubelt ");   // Protection de << pour les contre chants
    tabulature.replace(">>", " \\doubegt ");   // Protection de >> pour les contre chants
    tabulature.replace("<", "< ");
    tabulature.replace(">", " >");
    tabulature.replace("[", " [ ");
    tabulature.replace("]", " ] ");
    tabulature.replace("}", " } ");
    tabulature.replace("{", " { ");
    tabulature.replace(QRegExp(" +"), " ");    // Replace multiple spaces by a single space

    QStringList symbols = tabulature.split(" "); // Séparation du contenu à chaque espace pour analyser les éléments un par un
    QStringList parsedSymbolsMelody;             // Certain symboles vont dans la mélodie
    QStringList parsedSymbolsBass;               // Certain symboles vont dans l'accompagnement

    int line = 1;
    int nnote = 0;
    int nbass = 0;

    try
    {
        // Loop throught all symbols of the code
        for (int i = 0 ; i < symbols.size() ; i++)
        {
            QString symbol = symbols[i];
            QString parsed = symbol;

            int type = getType(symbol);

            m_rhs_close_spanner = false;
            m_rhs_open_spanner = false;
            m_rhs_markup = true;
            m_lhs_close_spanner = false;
            m_lhs_open_spanner = false;

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
                while(i < symbols.size() && !isNewLine(symbols[i]))
                {
                    i++;
                    parsed += " " + symbols[i];
                }

                line++;
                break;

            // Lettre p t P T. On update l'état global courant
            case DIRECTION:
            {
                CURRENTDIRECTION = getDirection(symbol);
                parsed = "";
                break;
            }

            // Bouton main droite. On parse, on trouve la note, on génè le code lilypond
            // et en fonction de la durée on regarde si on doit ouvrir ou fermer des spanners
            case BUTTON:
            case NOTE:
            {
                currentSymbolIsBass = false;

                if (type == BUTTON)
                    button.set_rhs_button(symbol);
                else
                    button.set_rhs_note(symbol);

                CURRENTDURATION = button.duration();
                CURRENTDIRECTION =  button.direction();

                timeline.append(CURRENTDURATION, nnote);

                update_rhs_spanner_state(button.get_duration_as_whole_note());

                parsed = button.print(m_rhs_markup);
                parsed = insert_rhs_spanners(parsed);

                nnote++;
                break;
            }

            // Un liaison ~, on doit ouvrir un spanner. Sauf si il est déjà ouvert
            // auquel cas il ne faut pas le fermer
            case TIE:
            {
                currentSymbolIsBass = false;
                if (open_rhs_spanner())
                    parsed = "\\startTextSpan ~";

                break;
            }

            // Un silence, il faut fermer le spanner
            case REST:
            {
                currentSymbolIsBass = false;
                close_rhs_spanner();
                parsed = insert_rhs_spanners(symbols[i]);
                break;
            }

            // symbole < d'ouverture d'accord dans la mélodie
            // On parse jusqu'au prochain > chaque symbole est une note ou un bouton
            case OPENCHORD:
            {

                currentSymbolIsBass = false;
                QString open = symbols[i];

                // On lit jusqu'a la fermeture >
                QStringList tmp;
                while (i < symbols.size() && !isCloseChord(symbols[i]))
                {
                    // Si le symbole est un bouton on l'ajoute à la liste
                    if (isButton(symbols[i]))
                    {
                        tmp.append(symbols[i]);
                    }
                    // Si c'est une note on la transforme en bouton
                    else if (isNote(symbols[i]))
                    {
                        button.set_rhs_note(symbols[i]);
                        tmp.append(button.button());
                    }
                    else if (!isOpenChord(symbols[i]) && !isCloseChord(symbols[i]))
                    {
                        throw std::logic_error((QString("Symbole innatentu détecté à l'intérieur d'un accord: ") + symbols[i]).toStdString());
                    }
                    i++;
                }

                if (i == symbols.size())
                    throw std::logic_error("Chevron < ouvert mais jamais fermé.");

                symbol = open + tmp.join(" ") + symbols[i];

                multiButton.set_rhs_multibutton(symbol);

                CURRENTDURATION = multiButton.duration();
                CURRENTDIRECTION = multiButton.direction();

                timeline.append(CURRENTDURATION, nnote);

                update_rhs_spanner_state(multiButton.get_duration_as_whole_note());

                parsed = multiButton.print(m_rhs_markup);
                parsed = insert_rhs_spanners(parsed);

                nnote++;
                break;
            }

            // Symbole [ de syntaxe explicite ou semi-explicite de l'accompagnement
            // On parse jusqu'au prochain ]
            case OPENMANUALBASS:
            {
                nbass++;
                currentSymbolIsMelody = false;
                i++;

               // On lit jusqu'a la fermeture ]
                QStringList tmp;
                while (i < symbols.size() && !isCloseManualBass(symbols[i]))
                {
                    if (!isCloseManualBass(symbols[i]))
                        tmp.append(symbols[i]);

                    if (!isExplicitBass(symbols[i]) &&  // Basse/accord explicite e.g B:4 ou a:4
                        !isImplicitBass(symbols[i]) &&  // Basse/accord implicite e.g B ou a
                        !isRest(symbols[i]) &&
                        !isCloseManualBass(symbols[i]))
                    {
                        throw std::logic_error((QString("Symbole inattentu détecté à l'intérieur d'un accompagnement: ") + symbols[i]).toStdString());
                    }

                    i++;
                };

                symbol = tmp.join(" ");

                // Liste de choses possibles dans tmp et de leur decompaction explicite possible
                // A            -> A:8 r:8 a:8 r:8 a:8 r:8
                // A a          -> A:8 r:8 a:8 r:8
                // A a a        -> A:8 r:8 a:8 r:8 a:8 r:8
                // A:2.         -> A:2.
                // A:2 a:4      -> A:2 a:4
                // A:4 r:4 a:4  -> A:4 r:4 a:4
                tmp = pattern.decompact_motif(symbol);

                if (i == symbols.size())
                    throw std::logic_error("Crochet [ ouvert mais jamais fermé.");

                for (QString &sym : tmp)
                {
                    if (isExplicitBass(sym))
                    {
                        button.set_lhs_button(sym);
                        sym = button.print();
                        update_lhs_spanner_state(button.get_duration_as_whole_note());
                        sym = insert_lhs_spanners(sym);
                    }
                }

                parsed = tmp.join(" ");
                break;
            }

             // Une lettre e.g. A ou g toute seule en dehors de []
            case IMPLICITBASS:
            {
                nbass++;
                currentSymbolIsMelody = false;
                QStringList syms = pattern.decompact_motif(symbol);
                for (int i = 0 ; i < syms.size() ; i++)
                {
                    if (isExplicitBass(syms[i]))
                    {
                        button.set_lhs_button(syms[i]);
                        update_lhs_spanner_state(button.get_duration_as_whole_note());
                        syms[i] = insert_lhs_spanners(button.print());
                    }
                }

                parsed = syms.join(" ");
                break;
            }

            // Une lettre associée à un chiffre e.g. A:4 ou g:2 toute seule en dehors de []
            case EXPLICITBASS:
            {
                nbass++;
                currentSymbolIsMelody = false;
                button.set_lhs_button(symbol);
                update_lhs_spanner_state(button.get_duration_as_whole_note());
                parsed = button.print();
                parsed = insert_lhs_spanners(parsed);
                break;
             }

            // Un commande latex e.g. \repeat
            // Le cas spécial \motif permer de changer l'état du parseur de motif.
            case COMMAND:
            {
                if (symbol == "\\break")
                {
                    currentSymbolIsBass = false;
                }
                else if (symbol == "\\partial")
                {
                    extractRankButton.indexIn(symbols[i+1]);
                    if (isButton(symbols[i+1]) && extractRankButton.cap(0) != "")
                        throw std::logic_error("Symbol incorrect détecté après \\partial. Une durée est attendue.");

                    parsed = symbols[i] + " " + symbols[i+1];
                    i++;
                }
                else if (symbol == "\\motif")
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
                else if (symbol == "\\repeat")
                {
                    m_scope.append(REPEAT);
                    int repeat = symbols[i+1].split(":")[1].toInt();
                    timeline.add_timeline();
                    timeline.set_repetition(repeat);
                    while(i < symbols.size()-1 && !isOpenBracket(symbols[i])) {
                        i++;
                        parsed += " " + symbols[i];
                    }
                }
                else if (symbol == "\\alternative")
                {
                    m_scope.append(ALTERNATIVE);
                    while(i < symbols.size()-1 && !isOpenBracket(symbols[i])) {
                        i++;
                        parsed += " " + symbols[i];
                    }
                }
                else if (symbol == "\\tuplet")
                {
                    m_scope.append(TUPLET);
                    timeline.set_speed_factor(symbols[i+1]);
                    while(i < symbols.size()-1 && !isOpenBracket(symbols[i])) {
                        i++;
                        parsed += " " + symbols[i];
                    }
                }


                //qDebug() << "command: scope =" << m_scope;
                break;
            }

            case OPENBRACKET:
            {
                if (m_scope.last() == ALTERNATIVE)
                {
                    //qDebug() << "add alternaive";
                    timeline.not_end_timeline();
                    timeline.add_alternative();
                    m_scope.append(ALTERNATIVESEQUENCE);
                }
                else
                {
                    m_scope.append(NONAME);
                }

                //qDebug() << "open: scope =" << m_scope;
                break;
            }

            case CLOSEBRACKET:
            {
                if (m_scope.size() == 0)
                    throw std::logic_error("Crochet fermant surnuméraire détecté");

                Scope scope = m_scope.last();

                if (scope == REPEAT)
                {
                    timeline.end_timeline();
                }
                else if (scope == ALTERNATIVE)
                {
                    timeline.end_timeline();
                }
                else if (scope == TUPLET)
                {
                    timeline.set_speed_factor("1/1");
                }

                m_scope.pop_back();
                //qDebug() << "close: scope =" << m_scope;
                break;
            }

            default:
                break;
            }

            if (currentSymbolIsMelody)
                parsedSymbolsMelody.append(parsed);

            if (currentSymbolIsBass)
                parsedSymbolsBass.append(parsed);

            // logs
            if (symbol != "" && symbol != "\n") log("Symbol: " + symbol + " >> " + parsed);
        }

        if (m_scope.size() > 0)
            throw std::logic_error("Crochet ouvrant non fermé détecté");

        if (m_rhs_spanner_must_be_closed) parsedSymbolsMelody.append(QString("\\endSpanners \\stopTextSpan"));
        if (m_lhs_spanner_must_be_closed) parsedSymbolsBass.append(QString("\\endSpanners \\stopTextSpan"));
    }
    catch(std::exception &e)
    {
        QString err = "ligne " + QString::number(line) + " - " + e.what();
        throw std::logic_error(err.toStdString());
    }

    m_tab->melody = parsedSymbolsMelody.join(" ");
    m_tab->melody.replace(QRegExp(":"), "");
    m_tab->melody.replace(QRegExp("\\doublegt"), ">>");
    m_tab->melody.replace(QRegExp("\\doubleglt"), "<<");

    if (nbass == 0) return;

    m_tab->bass = parsedSymbolsBass.join(" ");
    m_tab->bass.replace(QRegExp(":"), "");

    m_timeline = timeline.timeline();
}

void TabliatoProcessor::parseLyric()
{
    QStringList list;
    QStringList list2;
    list = m_tab->lyrics.split("\n\n");
    int n = list.size();
    int hn = n/2;

    m_tab->firstVerse = list[0];

    if (n > 1)
    {
        for(int i = 1 ; i <= hn ; i++)
        {
            list2 = list[i].split("\n");

            for(int j = 0 ; j < list2.size() ; j++)
                m_tab->leftVerses = m_tab->leftVerses + "\\line{ " + list2[j] + "} \n";

            m_tab->leftVerses = m_tab->leftVerses +  "\\vspace #1 \n";
        }

        for(int i = hn+1 ; i < n ; i++)
        {
            list2 = list[i].split("\n");

            for(int j = 0 ; j < list2.size() ; j++)
                m_tab->rightVerses = m_tab->rightVerses + "\\line{ " + list2[j] + "} \n";

            m_tab->rightVerses = m_tab->rightVerses +  "\\vspace #1 \n";
        }
    }
}

void TabliatoProcessor::generateLilypondCode()
{
    QString cadb;
    QString fingerDisplay;
    QString bassDisplay;
    QString lilypondCode;

    cadb = (m_tab->get("system") != "cadb") ? "f" : "t";
    fingerDisplay = (m_tab->get("fingerDisplay") != "true") ? "f" : "t";
    bassDisplay = (m_tab->bass != "") ? "t" : "f";

    QFile file(TEMPLATE + "/tabliato.ly");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::logic_error(QString("Impossible d'ouvrir le fichier : " + TEMPLATE + "/m_tabliato.ly").toStdString() );
    lilypondCode = file.readAll();
    file.close();

    lilypondCode.replace(QRegExp("<ranks>"), QString::number(m_keyboard.ranks()));
    lilypondCode.replace(QRegExp("<basses>"), m_tab->bass);
    lilypondCode.replace(QRegExp("<tablature>"), m_tab->melody);
    lilypondCode.replace(QRegExp("<title>"), m_tab->get("title"));
    lilypondCode.replace(QRegExp("<subtitle>"), m_tab->get("subtitle"));
    lilypondCode.replace(QRegExp("<composer>"), m_tab->get("composer"));
    lilypondCode.replace(QRegExp("<poet>"), m_tab->get("poet"));
    lilypondCode.replace(QRegExp("<tagline>"), m_tab->get("tagline"));
    lilypondCode.replace(QRegExp("<instrument>"), m_tab->get("instrument"));
    lilypondCode.replace(QRegExp("<tempo>"), m_tab->get("tempo"));
    lilypondCode.replace(QRegExp("<time>"), m_tab->get("time"));
    lilypondCode.replace(QRegExp("<key>"), m_tab->get("key"));
    lilypondCode.replace(QRegExp("<system>"), cadb);
    lilypondCode.replace(QRegExp("<fingerDisplay>"), fingerDisplay);
    lilypondCode.replace(QRegExp("<bassDisplay>"), bassDisplay);
    lilypondCode.replace(QRegExp("<firstVerse>"), m_tab->firstVerse);
    lilypondCode.replace(QRegExp("<leftLyrics>"), m_tab->leftVerses);
    lilypondCode.replace(QRegExp("<rightLyrics>"), m_tab->rightVerses);

    m_tab->lilypond = lilypondCode;
}



void TabliatoProcessor::update_rhs_spanner_state(float duration)
{
    if (m_rhs_spanner_is_open && m_rhs_spanner_must_be_closed)
    {
        m_rhs_spanner_is_open = false;
        m_rhs_spanner_must_be_closed = false;
        m_rhs_close_spanner = true;
    }
    else if (m_rhs_spanner_is_open && !m_rhs_spanner_must_be_closed)
    {
        m_rhs_spanner_must_be_closed = true;
        m_rhs_markup = false;
    }
    else
    {
        m_rhs_spanner_must_be_closed = true;
    }

    if (duration > 1 && !m_rhs_spanner_is_open)
    {
        m_rhs_spanner_is_open = true;
        m_rhs_spanner_must_be_closed = true;
        m_rhs_open_spanner = true;
    }
}


bool TabliatoProcessor::open_rhs_spanner()
{
    if (m_rhs_spanner_is_open)
    {
        m_rhs_spanner_must_be_closed = false;
        return false;
    }
    else
    {
        m_rhs_spanner_is_open = true;
        m_rhs_spanner_must_be_closed = false;
        return true;
    }
}

bool TabliatoProcessor::close_rhs_spanner()
{
    if (m_rhs_spanner_is_open)
    {
        m_rhs_spanner_must_be_closed = false;
        m_rhs_spanner_is_open = false;
        m_rhs_close_spanner = true;
        return true;
    }

    return false;
}


void TabliatoProcessor::update_lhs_spanner_state(float duration)
{
    if (m_lhs_spanner_is_open && m_lhs_spanner_must_be_closed)
    {
        m_lhs_spanner_is_open = false;
        m_lhs_spanner_must_be_closed = false;
        m_lhs_close_spanner = true;
        m_lhs_open_spanner = false;
    }
    else if (m_lhs_spanner_is_open && !m_lhs_spanner_must_be_closed)
    {
        m_lhs_spanner_must_be_closed = true;
        m_lhs_open_spanner = false;
    }
    else
    {
        m_lhs_spanner_must_be_closed = true;
        m_lhs_open_spanner = false;
    }

    if (duration > 1 && !m_lhs_spanner_is_open)
    {
        m_lhs_spanner_is_open = true;
        m_lhs_spanner_must_be_closed = true;
        m_lhs_open_spanner = true;
    }
}


bool TabliatoProcessor::open_lhs_spanner()
{
    if (m_lhs_spanner_is_open)
    {
        m_lhs_spanner_must_be_closed = false;
        return false;
    }
    else
    {
        m_lhs_spanner_is_open = true;
        m_lhs_spanner_must_be_closed = false;
        return true;
    }
}

bool TabliatoProcessor::close_lhs_spanner()
{
    if (m_lhs_spanner_is_open)
    {
        m_lhs_spanner_must_be_closed = false;
        m_lhs_spanner_is_open = false;
        m_lhs_close_spanner = true;
        return true;
    }

    return false;
}

QString TabliatoProcessor::insert_rhs_spanners(QString str)
{
    if (m_rhs_close_spanner)
        str += " \\stopTextSpan";

    if (m_rhs_open_spanner)
        str += " \\startTextSpan";

    return str;
}

QString TabliatoProcessor::insert_lhs_spanners(QString str)
{
    if (m_lhs_close_spanner)
        str += " \\stopTextSpan";

    if (m_lhs_open_spanner)
        str += " \\startTextSpan";

    return str;
}

void TabliatoProcessor::log(QString str)
{
    m_logs.append(str);
}
