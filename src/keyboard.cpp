#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <QDebug>
#include <stdexcept>

#include "global.h"
#include "keyboard.h"
#include "syntaxanalyser.h"

Keyboard::Keyboard()
{
    m_number_of_ranks = 0;
}

Keyboard::Keyboard(QString name)
{
    m_number_of_ranks = 0;
    read_keyboard_from_assemblage(name);
}

void Keyboard::read_keyboard_from_assemblage(QString name)
{
    if (name.isEmpty())
    {
        QString e("Aucun accordéon n'a été choisi.");
        throw std::logic_error(e.toStdString());
    }

    // List of possible config files to search
    QStringList possibleFiles = {
        KEYBOARDS + "/assemblages.csv",
        KEYBOARDS + "/assemblages_utilisateur.csv"
    };

    QStringList list;
    bool found = false;

    // Try each file until we find the target assemblage
    for (const QString& filename : possibleFiles)
    {
        QFile file(filename);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            // Only warn about missing user file; fail only if both fail later
            continue;
        }

        QTextStream stream(&file);

        while (!stream.atEnd())
        {
            QString line = stream.readLine().trimmed();
            if (line.isEmpty()) continue;

            list = line.split(";");

            if (!list.isEmpty() && list[0] == name)
            {
                found = true;
                break;
            }
        }

        file.close();

        if (found)
            break;
    }

    if (!found)
    {
        QString e("Impossible de trouver le clavier " + name + " dans les fichiers de configuration des claviers");
        throw std::logic_error(e.toStdString());
    }

    m_name = list[1];

    for (int i = 2; i < list.length(); ++i)
        m_map_button_to_note += read_keyboard_from_name(list[i]);

    build_note_to_button_map();
    set_number_of_ranks();
}

void Keyboard::build_note_to_button_map()
{
    QMapIterator<QString, QString> it(m_map_button_to_note);
    while (it.hasNext())
    {
        it.next();
        m_map_note_to_buttons.insertMulti(it.value(), it.key());
    }
}

void Keyboard::set_number_of_ranks()
{
    // Search the button with the most numerous ' to know
    // how many ranks this accordion has. For exemple if
    // p3'' exists this is a 3 ranks accordion

    int rank;
    SyntaxAnalyser sa;
    QMapIterator<QString, QString> it(m_map_button_to_note);

    while (it.hasNext())
    {
        it.next();
        rank = sa.getRank(it.key());
        if (rank > m_number_of_ranks)
            m_number_of_ranks = rank;
    }
}

QMultiMap<QString, QString> Keyboard::read_keyboard_from_name(QString name)
{
    QMultiMap<QString, QString> map;

    if (name.isEmpty())
        return map;

    QFile file(KEYBOARDS + "/" + name + ".csv");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QString e("Impossible de lire le fichier de configuration des claviers : " + KEYBOARDS + "/" + name + ".csv");
        throw std::logic_error(e.toStdString());
    }

    QTextStream stream(&file);

    while (!stream.atEnd())
    {
        QString line = stream.readLine();

        if (!line.isEmpty())
            throw std::runtime_error("Fichier de clavier invalide qui contient une ligne vide (" + name.toStdString() + ".csv)");

        QStringList list = line.split(";");

        if (list.size() != 2)
            throw std::runtime_error("Fichier de clavier invalide à la ligne: " + line.toStdString() +  " (" + name.toStdString() + ".csv)");

        QString button = list[0];
        QString note = list[1];

        map.insert(button, note);
    }

    file.close();


    return map;
}

QString Keyboard::get_note_from_button(QString button)
{
    QString ret = m_map_button_to_note.value(button, "");

    if (ret.isEmpty())
    {
        QString e("Le bouton " + button + " n'existe pas sur le clavier choisi.");
        throw std::logic_error(e.toStdString());
    }

    return ret;
}

QList<QString> Keyboard::get_buttons_from_note(QString note)
{
    QList<QString> ret = m_map_note_to_buttons.values(note);

    if (ret.length() == 0)
    {
        QString e("Vous ne pouvez pas jouer la note " + note + " avec cet accordéon.");
        throw std::logic_error(e.toStdString());
    }

    return ret;
}

int Keyboard::ranks()
{
   return m_number_of_ranks;
}

/*QStringList Keyboard::keys()
{
    //QList<QString> list = map.keys();
    QStringList list;

    QStringList rang;
    rang << "" << "'" << "''";
    QStringList direction;
    direction << "p" << "t";
    QStringList button;
    button << "1" << "2" <<  "3" << "4" << "5" << "6" << "7" << "8" <<  "9" << "10" << "11" << "12" << "13" << "14";
    QStringList bass;
    bass << "C" << "D" << "E" << "F" << "G" << "A" << "B" << "c" << "d" << "e" << "f" << "g" << "a" << "b";
    QStringList alt;
    alt << "" << "m" << "b" << "#";

    QString cmd;

    for(int d = 0; d < direction.size(); ++d)
    {
        for(int r = 0; r < rang.size(); ++r)
        {

            for(int b = 0; b < button.size(); ++b)
            {
                try
                {
                    cmd = direction[d] + button[b] + rang[r];
                    get_note_from_button(cmd);
                    list << cmd;
                }
                catch(const std::exception &e)
                {
                }
            }
        }
    }


    for(int ba = 0; ba < bass.size(); ++ba)
    {
        for(int a = 0; a < alt.size(); ++a)
        {
            try
            {
                cmd = bass[ba] + alt[a];
                get_note_from_button(cmd);
                list << cmd;
            }
            catch(const std::exception &e)
            {
            }
        }
    }

    return list;
}*/
