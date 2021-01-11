#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QMultiMap>

#include "syntaxanalyser.h"

class Keyboard : public SyntaxAnalyser
{
    public:
        Keyboard();
        void buildKeyboard(QString str);
        QString getNote(QString key);
        QStringList keys();
        QList<QString> getButtons(QString key);
        int ranks();

        QString accordionName;

    private:
        int rankNumber;
        QMultiMap<QString, QString>  loadKeyboard(QString name);
        QMultiMap<QString, QString> map;
        QMultiMap<QString, QString> revmap;
};

#endif // KEYBOARD_H
