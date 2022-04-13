#ifndef MUSICPROCESSOR_H
#define MUSICPROCESSOR_H

#include <QString>
#include <QRegExp>
#include <QFile>
#include <QTextStream>

#include "timeline.h"
#include "tabulature.h"
#include "syntaxanalyser.h"
#include "keyboard.h"
#include "button.h"

class TabliatoProcessor : public SyntaxAnalyser
{
    public:
        TabliatoProcessor(Tabulature &tabulature);
        QString get_logs() { return m_logs.join("\n"); }
        Timeline m_timeline;

    private:
        void parseMusic();
        void parseLyric();
        void generateLilypondCode();
        void update_rhs_spanner_state(float);
        bool open_rhs_spanner();
        bool close_rhs_spanner();
        void update_lhs_spanner_state(float);
        bool open_lhs_spanner();
        bool close_lhs_spanner();
        QString insert_rhs_spanners(QString);
        QString insert_lhs_spanners(QString);
        void log(QString str);


    private:
        enum Scope {REPEAT, ALTERNATIVE, ALTERNATIVESEQUENCE, TUPLET, NONAME};

        Keyboard m_keyboard;
        Tabulature *m_tab;
        QStringList m_logs;
        QList<Scope> m_scope;
        bool m_rhs_spanner_is_open;
        bool m_lhs_spanner_is_open;
        bool m_rhs_spanner_must_be_closed;
        bool m_lhs_spanner_must_be_closed;
        bool m_rhs_close_spanner;
        bool m_lhs_close_spanner;
        bool m_rhs_open_spanner;
        bool m_lhs_open_spanner;
        bool m_rhs_markup;
};

#endif // MUSICPROCESSOR_H
