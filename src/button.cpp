#include <QStringList>
#include <QDebug>

#include <stdexcept>

#include "button.h"

QString CURRENTDIRECTION = "p";
QString CURRENTDURATION = "4";

ButtonParser::ButtonParser(Keyboard &keyboard)
{
    kbd = &keyboard;
}

void ButtonParser::set_rhs_note(QString str)
{
    m_side = RHS;
    m_duration = CURRENTDURATION;
    m_direction = CURRENTDIRECTION;

    extractNote.indexIn(str);
    m_note = extractNote.cap(1);

    if (extractRankNote.indexIn(str) >= 0)
        m_rank = extractRankNote.cap(1);

    if (extractDuration.indexIn(str) >= 0)
    {
        m_duration = extractDuration.cap(1);
    }

    if (extractDirection.indexIn(str) >= 0)
    {
        m_direction = extractDirection.cap(1);
        m_direction = m_direction.toLower();
    }

    avaibleButton = kbd->getButtons(m_note);

    if (avaibleButton.length() == 1)
    {
        m_button = avaibleButton[0];
    }
    else if (avaibleButton.length() > 1)
    {
        if (QString(avaibleButton[1][0]) != m_direction && QString(avaibleButton[0][0]) != m_direction)
            throw std::logic_error(QString("Vous ne pouvez pas jouer la note " + m_note + " dans le sens " + ((m_direction == "t") ? "tiré" : "poussé") + " avec cet accordéon.").toStdString());

        if (m_rank.isEmpty())
        {
            if(QString(avaibleButton[0][0]) == m_direction && QString(avaibleButton[1][0]) != m_direction)
               m_button = avaibleButton[0];
            else if(QString(avaibleButton[1][0]) == m_direction && QString(avaibleButton[0][0]) != m_direction)
               m_button = avaibleButton[1];
            else
               throw std::logic_error(QString("Il y a deux possibilités pour la note " + m_note + "\n  1. " + avaibleButton[0] + "\n  2. " + avaibleButton[1] + "\nAjoutez une indication de rang. Par exemple : " + m_note + "/1").toStdString());
        }
        else
        {
            int i = m_rank.toInt();

            if (i <= avaibleButton.length())
                m_button = avaibleButton[i-1];
            else
                throw std::logic_error(QString("Le rang " + m_rank + " n'est pas un rang valide pour la note " + m_note + " sur cet accordéon").toStdString());
         }
     }

    if (extractNumButton.indexIn(m_button) >= 0)
        m_button = extractNumButton.cap(1);
}

void ButtonParser::set_rhs_button(QString str)
{    
    m_side = RHS;
    m_duration = CURRENTDURATION;
    m_direction = CURRENTDIRECTION;

    if (extractDuration.indexIn(str) >= 0)
    {
        m_duration = extractDuration.cap(1);

        if (m_duration != "1"  && m_duration != "1."  &&
            m_duration != "2"  && m_duration != "2."  &&
            m_duration != "4"  && m_duration != "4."  &&
            m_duration != "8"  && m_duration != "8."  &&
            m_duration != "16" && m_duration != "16." &&
            m_duration != "32" && m_duration != "32." )
            throw std::logic_error(QString("La durée " + m_duration + " n'est pas une durée valide").toStdString());
    }

    if (extractNumButton.indexIn(str) >= 0)
        m_button = extractNumButton.cap(1);

    if (extractDirection.indexIn(str) >= 0)
        m_direction = extractDirection.cap(1);

    extractRankButton.indexIn(str);

    if (extractRankButton.cap(0) == "")
        m_rank = "1";
    else if(extractRankButton.cap(0) == "'")
        m_rank = "2";
    else
        m_rank = "3";

    m_note = kbd->getNote(m_direction + m_button);
}

void ButtonParser::set_lhs_button(QString str)
{
    m_side = LHS;

    if (extractDuration.indexIn(str) >= 0)
    {
        m_duration = extractDuration.cap(1);
        m_button = str.split(":")[0];
        m_note = kbd->getNote(m_button);

        if (m_duration != "1"  && m_duration != "1."  &&
            m_duration != "2"  && m_duration != "2."  &&
            m_duration != "4"  && m_duration != "4."  &&
            m_duration != "8"  && m_duration != "8."  &&
            m_duration != "16" && m_duration != "16." &&
            m_duration != "32" && m_duration != "32." )
            throw std::logic_error(QString("La durée " + m_duration + " n'est pas une durée valide").toStdString());

        return;
    }

    throw std::logic_error("Erreur interne, un bouton main gauche devrait toujours être associée à une durée");
}

QString ButtonParser::print(bool markup)
{
    if (m_side == RHS)
    {
        QString dir;
        QString btn;
        QString nte;

        if (!m_direction.isEmpty() && markup)
            dir = "\\" + m_direction + " ";

        if (!m_button.isEmpty() && markup)
            btn = " \"" + m_button + "\" ";

        return dir + btn + m_note + m_duration;
    }
    else
    {
        QString openSpanner;
        QString closeSpanner;

        return m_note + m_duration + "^\"" + m_button + "\"";
    }
}

void ButtonParser::clear()
{
    m_button.clear();
    m_note.clear();
    m_finger.clear();
    m_rank.clear();
    m_direction.clear();
    m_duration.clear();
}

//===============================

MultiButtonParser::MultiButtonParser(Keyboard &keyboard)
{
    m_kbd = &keyboard;
}

void MultiButtonParser::set_rhs_multibutton(QString str)
{
    m_chord.clear();

    // on trouve une durée dans ce bazarre après la fermeture
    QString duration = CURRENTDURATION;
    QString direction = CURRENTDIRECTION;

    if (extractDuration.indexIn(str) >= 0)
        duration = extractDuration.cap(1);

    if (extractDirection.indexIn(str) >= 0)
        direction = extractDirection.cap(1);

    str = str.split("<")[1]; // remove first character which is <
    str = str.split(">")[0]; // remove last character which is >

    QStringList symbols = str.split(" ");
    for (QString symbol : symbols)
    {
        ButtonParser button(*m_kbd);
        button.set_rhs_button(direction + symbol + ":" + duration);
        m_chord.append(button);
    }
}

void MultiButtonParser::set_lhs_multibutton(QString str)
{
    // TODO
}

QString MultiButtonParser::print(bool markup)
{
    QString out;
    if (m_chord[0].side() == Button::RHS)
    {

        for(int i = 0 ; i < m_chord.length() ; ++i)
        {
            if (markup)
                out += "\\" + m_chord[i].direction() + " \"" + m_chord[i].button() + "\" ";
            else
                out += " ";
        }

        out += "<";

        for(int i = 0 ; i < m_chord.length() ; ++i)
        {
            out += m_chord[i].note() + " ";
        }

        out += "> ";
        out += m_chord[0].duration();
    }
    else
    {
        // TODO
    }

    return out;
}
