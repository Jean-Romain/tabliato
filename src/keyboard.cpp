#include <QFile>
#include <QTextStream>
#include <QStringList>

#include <QDebug>
#include <stdexcept>

#include "global.h"
#include "keyboard.h"

Keyboard::Keyboard()
{
    rankNumber = 0;
}

void Keyboard::buildKeyboard(QString name)
{
    if(name.isEmpty())
        throw std::logic_error(QString("Aucun accordéon n'a été choisi.").toStdString());

    QFile file(KEYBOARDS + "/assemblages.csv");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::logic_error(QString("Impossible de lire le fichier de configuration des claviers : " + KEYBOARDS + "/assemblages.csv").toStdString());

    QTextStream stream(&file);
    QStringList list;

    while (!stream.atEnd() && accordionName != name)
    {
        QString line = stream.readLine();
        list = line.split(";");
        accordionName = list[0];
    }

    if(stream.atEnd() && accordionName != name)
        throw std::logic_error(QString("Impossible de trouver le clavier " + name  + " dans le fichier de configuration des claviers").toStdString());

    file.close();

    accordionName = list[1];

    for(int i = 2 ; i < list.length() ; ++i)
        map += loadKeyboard(list[i]);

    int rank;
    QMapIterator<QString, QString> it(map);
    while (it.hasNext())
    {
        it.next();
        revmap.insertMulti(it.value(), it.key());

        extractRankButton.indexIn(it.key());

        if(extractRankButton.cap(0) == "")
            rank = 1;
        else if(extractRankButton.cap(0) == "'")
            rank = 2;
        else
            rank = 3;

        if (rank > rankNumber)
            rankNumber = rank;
    }
}

QMultiMap<QString, QString> Keyboard::loadKeyboard(QString name)
{
    QMultiMap<QString, QString> map;

    if(!name.isEmpty())
    {
        QFile file(KEYBOARDS + "/" + name + ".csv");

        if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
            throw std::logic_error(QString("Impossible de lire le fichier de configuration des claviers : " + KEYBOARDS + "/" + name + ".csv").toStdString());

        QTextStream stream(&file);

        while (!stream.atEnd())
        {
            QString line = stream.readLine();
            QStringList list = line.split(";");

            QString button = list[0];
            QString note = list[1];

            map.insert(button, note);
        }

        file.close();
    }

    return map;
}

QString Keyboard::getNote(QString key)
{
    QString ret = map.value(key, "");

    if(ret.isEmpty())
        throw std::logic_error(QString("Le bouton " + key + " n'existe pas sur le clavier choisi.").toStdString());

    return ret;
}

QList<QString> Keyboard::getButtons(QString key)
{
    QList<QString> ret = revmap.values(key);

    if(ret.length() == 0)
        throw std::logic_error(QString("Vous ne pouvez pas jouer la note " + key + " avec cet accordéon.").toStdString());

    return ret;
}

int Keyboard::ranks()
{
   return rankNumber;
}

QStringList Keyboard::keys()
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
                    getNote(cmd);
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
                getNote(cmd);
                list << cmd;
            }
            catch(const std::exception &e)
            {
            }
        }
    }

    return list;
}
