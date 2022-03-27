#include <QStringList>
#include <QDebug>

#include <stdexcept>

#include "button.h"

QString Button::direction = "p";
QString Button::duration = "4";

ButtonParser::ButtonParser(Keyboard &keyboard)
{
    kbd = &keyboard;
}

void ButtonParser::setNote(QString str)
{
    extractNote.indexIn(str);
    note = extractNote.cap(1);

    if (extractRankNote.indexIn(str) >= 0)
        rank = extractRankNote.cap(1);

    if (extractDuration.indexIn(str) >= 0)
        duration = extractDuration.cap(1);

    if (extractDirection.indexIn(str) >= 0)
    {
        direction = extractDirection.cap(1);
        direction = direction.toLower();
    }

    avaibleButton = kbd->getButtons(note);

    if(avaibleButton.length() == 1)
        button = avaibleButton[0];

    else if(avaibleButton.length() > 1)
    {
        if(QString(avaibleButton[1][0]) != direction && QString(avaibleButton[0][0]) != direction)
            throw std::logic_error(QString("Vous ne pouvez pas jouer la note " + note + " dans le sens " + direction + " avec cet accordéon.").toStdString());

        if(rank.isEmpty())
        {
            if(QString(avaibleButton[0][0]) == direction && QString(avaibleButton[1][0]) != direction)
               button = avaibleButton[0];
            else if(QString(avaibleButton[1][0]) == direction && QString(avaibleButton[0][0]) != direction)
               button = avaibleButton[1];
            else
            {
                throw std::logic_error(QString("Il y a deux possibilités pour la note " + note
                                           + "\n  1. " + avaibleButton[0]
                                           + "\n  2. " + avaibleButton[1]
                                           + "\nAjoutez une indication de rang. Par exemple : "
                                           + note + "/1").toStdString());
            }
        }
        else
        {
            int i = rank.toInt();

            if(i <= avaibleButton.length())
                button = avaibleButton[i-1];
            else
                throw std::logic_error(QString("Le rang " + rank + " n'est pas un rang valide pour la note " + note + " sur cet accordéon").toStdString());
         }
     }

    if(extractNumButton.indexIn(button) >= 0)
        button = extractNumButton.cap(1);
}

void ButtonParser::setButton(QString str)
{    
    if (extractDuration.indexIn(str) >= 0)
        duration = extractDuration.cap(1);

    if (duration != "1"  && duration != "1."  &&
        duration != "2"  && duration != "2."  &&
        duration != "4"  && duration != "4."  &&
        duration != "8"  && duration != "8."  &&
        duration != "16" && duration != "16." &&
        duration != "32" && duration != "32." )
        throw std::logic_error(QString("La durée " + duration + " n'est pas une durée valide").toStdString());

    if (extractNumButton.indexIn(str) >= 0)
        button = extractNumButton.cap(1);

    if (extractDirection.indexIn(str) >= 0)
        direction = extractDirection.cap(1);

    extractRankButton.indexIn(str);

    if (extractRankButton.cap(0) == "")
        rank = "1";
    else if(extractRankButton.cap(0) == "'")
        rank = "2";
    else
        rank = "3";

    note = kbd->getNote(direction + button);
}

void ButtonParser::setBass(QString str)
{
    if(extractDuration.indexIn(str) >= 0)
        duration = extractDuration.cap(1);

    button = str.split(":")[0];

    direction = "up";

    note = kbd->getNote(button);

    button.replace(QRegExp("#"), "♯");
    button.replace(QRegExp("([a-gA-G])b"), "\\1♭");
}

QString ButtonParser::print(bool markup)
{
    QString dir;
    QString btn;
    QString nte;

    if(!direction.isEmpty() && markup)
        dir = "\\" + direction + " ";

    if(!button.isEmpty() && markup)
        btn = " \"" + button + "\" ";

    nte = note;
    if (note.split(" ").length() > 1)
        nte = "<" + note + ">";

    return dir + btn + nte + duration;
}

void ButtonParser::clear()
{
    button.clear();
    note.clear();
    finger.clear();
    rank.clear();
}

//===============================

MultiButtonParser::MultiButtonParser(Keyboard &keyboard)
{
    kbd = &keyboard;
}

void MultiButtonParser::setMultiButton(QString str)
{
    chord.clear();

    QStringList symbols = str.split(",");
    for (QString symbol : symbols)
    {
        ButtonParser button(*kbd);
        button.setButton(symbol);
        chord.append(button);
    }
}

QString MultiButtonParser::print(bool markup)
{
    QString out;

    for(int i = 0 ; i < chord.length() ; ++i)
    {
        if (markup)
            out += "\\" + chord[i].direction + " \"" + chord[i].button + "\" ";
        else
            out += " ";
    }

    out += "<";

    for(int i = 0 ; i < chord.length() ; ++i)
    {
        out += chord[i].note + " ";
    }

    out += "> ";
    out += chord[0].duration;

    return out;
}
