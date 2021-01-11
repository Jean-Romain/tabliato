#ifndef BUTTON_H
#define BUTTON_H

#include <QString>

#include "keyboard.h"
#include "syntaxanalyser.h"

class Button
{
    public:
        static QString direction;
        static QString duration;
        QString button;
        QString rank;
        QString note;
        QString finger;

    public:
        float getDurationAsNumber(){
            return getDurationAsNumber(duration);
        }

        static float getDurationAsNumber(QString str) {
            bool dotted_note = str.at(str.size()-1) == ".";
            if (dotted_note) str.resize(str.size()-1);
            float duration = 4/str.toFloat();
            duration = (dotted_note) ? duration * 1.5 : duration;
            return duration;
        }
};

class MultiButton
{
    public:
        QList<Button> chord;
};

class ButtonParser : public Button, public SyntaxAnalyser
{
    public:
        ButtonParser(Keyboard &keybord);
        void setButton(QString str);
        void setNote(QString str);
        void setBass(QString str);
        void clear();
        QString print(bool markup = true);

    public:
       QList<QString> avaibleButton;

   private:
       Keyboard* kbd;
};

class MultiButtonParser : public MultiButton, public SyntaxAnalyser
{
    public:
       MultiButtonParser(Keyboard &keyboard);
       void setMultiButton(QString str);
       QString print(bool markup = true);
       float getDurationAsNumber(){ return chord[0].getDurationAsNumber(); }

    private:
       Keyboard* kbd;

};

#endif // BUTTON_H
