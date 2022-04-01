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
        QString get_logs() { return logs.join("\n"); }

    private:
        void parseMusic();
        void parseLyric();
        void generateLilypondCode();
        void log(QString str);

    private:
        Keyboard keyboard;
        Tabulature *tab;
        QStringList logs;
};

#endif // MUSICPROCESSOR_H
