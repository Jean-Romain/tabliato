#include <QRegExp>
#include <QDebug>

#include <stdexcept>

#include "tabulature.h"

Tabulature::Tabulature(){}
Tabulature::~Tabulature(){}

void Tabulature::setTabulature(QString str)
{
    tabulature = str;
}

void Tabulature::setLyrics(QString str)
{
    lyrics = str;
}

void Tabulature::set(QString option, QString value)
{
    if(options.value(option, "") == "")
        options.insert(option, value);
    else
        options[option] = value;
}

void Tabulature::setOptions(QMap<QString,QString> map)
{
    QList<QString> required;
    required << "accordion" << "tempo" << "time" << "key" << "rythm";

    for(int i = 0 ; i < required.length() ; i++)
    {
        if (map.value(required[i], "FAILURE") == "FAILURE")
            throw std::logic_error(QString("L'option " + required[i] + " est requise mais n'est pas définie").toStdString());
    }

    options = map;
}

QString Tabulature::get(QString option){ return options.value(option, ""); }
