#ifndef MUSICPROCESSOR_H
#define MUSICPROCESSOR_H

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QTextStream>

#include "tabulature.h"
#include "syntaxanalyser.h"
#include "keyboard.h"
#include "button.h"

class TabliatoProcessor : public SyntaxAnalyser
{
    public:
        TabliatoProcessor(Tabulature &tabulature);
        QString getLogs();
        void compile();
        void compile(QStringList option, bool ly = false);

    private:
        void parseMusic();
        //void parseBass();
        //void parseTabulature();
        void parseLyric();
        void printKeyboard();
        void generateLilypondCode();
        void log(QString str);

    private:
        Keyboard keyboard;
        Tabulature *tab;
        QStringList logs;
};

#endif // MUSICPROCESSOR_H
