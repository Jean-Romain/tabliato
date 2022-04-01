#ifndef BUTTON_H
#define BUTTON_H

#include <QString>

#include "keyboard.h"
#include "syntaxanalyser.h"

extern QString CURRENTDIRECTION;
extern QString CURRENTDURATION;

class Button
{
public:
    enum Side {RHS, LHS};

protected:
    QString m_direction;
    QString m_duration;
    QString m_button;
    QString m_rank;
    QString m_note;
    QString m_finger;
    Side m_side;

public:
    static float getDurationAsNumber(QString str)
    {
        bool dotted_note = str.at(str.size()-1) == ".";
        if (dotted_note) str.resize(str.size()-1);
        float duration = 4/str.toFloat();
        duration = (dotted_note) ? duration * 1.5 : duration;
        return duration;
    }

    float getDurationAsNumber(){ return getDurationAsNumber(m_duration); }
    QString duration() { return m_duration; }
    QString direction() { return m_direction; }
    QString button() { return m_button; }
    QString rank() { return m_rank; }
    QString note() { return m_note; }
    Side side() { return m_side; }
};

class MultiButton
{
    public:
        QList<Button> m_chord;
};


class ButtonParser : public Button, public SyntaxAnalyser
{
    public:
        ButtonParser(Keyboard &keyboard);
        void set_rhs_button(QString str);
        void set_rhs_note(QString str);
        void set_lhs_button(QString str);
        void clear();
        QString print(bool markup = true);

    public:
       QList<QString> avaibleButton; // pas utile que ce soit un membre

   private:
       Keyboard* kbd;
};


class MultiButtonParser : public MultiButton, public SyntaxAnalyser
{
    public:
       MultiButtonParser(Keyboard &keyboard);
       void set_rhs_multibutton(QString str);
       void set_lhs_multibutton(QString str);
       QString print(bool markup = true);
       float getDurationAsNumber(){ return m_chord[0].getDurationAsNumber(); }
       QString duration() { return m_chord[0].duration(); }
       QString direction() { return m_chord[0].direction(); }

    private:
       Keyboard* m_kbd;

};

#endif // BUTTON_H
