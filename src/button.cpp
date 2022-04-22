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
    m_rank = "";
    m_side = RHS;
    m_duration = CURRENTDURATION;
    m_direction = CURRENTDIRECTION;

    // extraction de la note a b c d e f g avec ses ' e.g a''
    extractNote.indexIn(str);
    m_note = extractNote.cap(1);

    // extraction du rang sur le diato /1 e.g. a''/2 pour le rang 2
    if (extractRankNote.indexIn(str) >= 0)
        m_rank = extractRankNote.cap(1);

    // extraction de la durée de la note e.g. a'':4/2
    if (extractDuration.indexIn(str) >= 0)
        m_duration = extractDuration.cap(1);

    // extraction de la direction ta'':4/2 (ne fonctionne pas)
    if (extractDirection.indexIn(str) >= 0)
        m_direction = extractDirection.cap(1).toLower();

    // Trouve tous les boutons qui font cette note
    avaibleButton = kbd->getButtons(m_note);

    // Si il n'y a qu'une option c'est facile
    if (avaibleButton.length() == 1)
    {
        m_button = avaibleButton[0];
    }
    // Si il y a plus d'une options il faut déterminier quel bouton choisir.
    // Il ne peut pas y avoir 3 fois la même note donc la taille est 2 (enfin je crois)
    else if (avaibleButton.length() > 1)
    {
        // Si aucune possibilités n'est dans la direction actuelle on change de direction automatiquement
        if (QString(avaibleButton[1][0]) != m_direction && QString(avaibleButton[0][0]) != m_direction)
            m_direction = (m_direction == "p") ? "t" : "p";
            //throw std::logic_error(QString("Vous ne pouvez pas jouer la note " + m_note + " dans le sens " + ((m_direction == "t") ? "tiré" : "poussé") + " avec cet accordéon.").toStdString());

        // Si on a pas précisé le rang
        if (m_rank.isEmpty())
        {
            // Si la direction de la première option correspond à la direction actuelle mais pas la deuxième alors on prend la première option
            if (QString(avaibleButton[0][0]) == m_direction && QString(avaibleButton[1][0]) != m_direction)
            {
               m_button = avaibleButton[0];
            }
            // Si la direction de la deuxième option correspond à la direction actuelle mais pas la première alors on prend la deuxième option
            else if (QString(avaibleButton[1][0]) == m_direction && QString(avaibleButton[0][0]) != m_direction)
            {
               m_button = avaibleButton[1];
            }
            // Sinon on ne sait pas quoi faire il y a deux options. On va afficher les deux options en rouge
            // en trichant sur le contenu de m_bouton pour que la fonction print affiche de quoi
            else
            {
               m_button = avaibleButton[0].remove(0,1) + "\" \\once \\override TextScript.color = #red \\" + m_direction + " \""  + avaibleButton[1].remove(0,1);
               //throw std::logic_error(QString("Il y a deux possibilités pour la note " + m_note + "\n  1. " + avaibleButton[0] + "\n  2. " + avaibleButton[1] + "\nAjoutez une indication de rang. Par exemple : " + m_note + "/1").toStdString());
                return;
            }
        }
        // Si on a précisé le rang
        else
        {
            int rank = m_rank.toInt();
            int pos = -1;

            for (int i = 0 ; i < avaibleButton.size() ; i++)
            {
                int srank = 1;
                if (extractRankButton.indexIn(avaibleButton[i]) >= 0)
                    srank = extractRankButton.cap(1).length()+1;

                if (rank == srank)
                    pos = i;
            }


            if (pos >= 0)
                m_button = avaibleButton[pos];
            else
                throw std::logic_error(QString("Le rang " + m_rank + " n'est pas un rang valide pour la note " + m_note + " sur cet accordéon").toStdString());
         }
     }

    if (extractNumButton.indexIn(m_button) >= 0){}
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
