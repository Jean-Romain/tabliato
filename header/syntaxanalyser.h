﻿#ifndef SYNTAXANALYSER_H
#define SYNTAXANALYSER_H

#include <QString>
#include <QRegExp>

class SyntaxAnalyser
{
    public:
        SyntaxAnalyser();

        /* IS */
        bool isButton(QString str);
        bool isComment(QString str);
        bool isPushed(QString str);
        bool isDirection(QString str);
        bool isImplicitBass(QString str);
        bool isExplicitBass(QString str);
        bool isBassSet(QString str);
        bool isRest(QString str);
        bool isCommand(QString str);
        bool isFingering(QString str);
        bool isNewLine(QString str);
        bool isNote(QString str);
        bool isMetric(QString str);
        bool isChord(QString str);
        bool isOpenChord(QString str);
        bool isCloseChord(QString str);
        bool isOpenBracket(QString str);
        bool isCloseBracket(QString str);
        bool isDoubleQuote(QString str);
        bool isOpenManualBass(QString str);
        bool isCloseManualBass(QString str);

        /* HAS */
        bool hasDirection(QString str);

        /* GET */
        int getType(QString str);
        int getRank(QString str);
        QString getDirection(QString str);

    protected:
        QRegExp button_rgx;
        QRegExp comment_rgx;
        QRegExp direction_rgx;
        QRegExp impliciteBass_rgx;
        QRegExp expliciteBass_rgx;
        QRegExp rest_rgx;
        QRegExp pushed_rgx;
        QRegExp command_rgx;
        QRegExp fingering_rgx;
        QRegExp closeChord_rgx;
        QRegExp openChord_rgx;
        QRegExp note_rgx;
        QRegExp metric_rgx;

        QRegExp extractRankButton;
        QRegExp extractRankNote;
        QRegExp extractFinger;
        QRegExp extractDuration;
        QRegExp extractNumButton;
        QRegExp extractNote;
        QRegExp extractDirection;
        QRegExp extractTie;

        QRegExp hasDirection_rgx;

        bool genericTest(QRegExp rx, QString str);

        enum EntityTypes {BUTTON,
                          BUTTONCHORD,
                          DIRECTION,
                          KEYWORD,
                          COMMENT,
                          COMMAND,
                          NEWLINE,
                          NOTE,
                          OTHER,
                          IMPLICITBASS,
                          EXPLICITBASS,
                          BASS,
                          FINGER,
                          REST,
                          OPENCHORD,
                          CLOSECHORD,
                          OPENBRACKET,
                          CLOSEBRACKET,
                          OPENMANUALBASS,
                          CLOSEMANUALBASS,
                          TIE,
                          DOUBLEQUOTE};
};

#endif // SYNTAXANALYSER_H
