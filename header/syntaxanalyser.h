#ifndef SYNTAXANALYSER_H
#define SYNTAXANALYSER_H

#include <QString>
#include <QRegExp>

class SyntaxAnalyser
{
    public:
        SyntaxAnalyser();

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
        bool isOpenManualBass(QString str);
        bool isCloseManualBass(QString str);

        bool hasDirection(QString str);

        int getType(QString str);
        int getTypeRightHand(QString str);
        int getTypeLeftHand(QString str);

        QString getDirection(QString str);

    public:
        QRegExp button_rgx;
        QRegExp comment_rgx;
        QRegExp direction_rgx;
        QRegExp impliciteBass_rgx;
        QRegExp expliciteBass_rgx;
        QRegExp rest_rgx;
        QRegExp impliciteBassSet_rgx;
        QRegExp pushed_rgx;
        QRegExp command_rgx;
        QRegExp fingering_rgx;
        QRegExp closeChord_rgx;
        QRegExp note_rgx;
        QRegExp metric_rgx;

        QRegExp extractRankButton;
        QRegExp extractRankNote;
        QRegExp extractDuration;
        QRegExp extractNumButton;
        QRegExp extractNote;
        QRegExp extractDirection;
        QRegExp extractTie;

        QRegExp hasDirection_rgx;
        QRegExp isChord_rgx;

        bool genericTest(QRegExp rx, QString str);

        enum EntityTypes {BUTTON, BUTTONCHORD, DIRECTION, KEYWORD, COMMENT, COMMAND, NEWLINE, NOTE, OTHER, IMPLICITBASS, EXPLICITBASS, BASS, BASSCHORD, FINGER, REST, MELODYCHORD, OPENCHORD, CLOSECHORD, OPENMANUALBASS, CLOSEMANUALBASS, TIE};
};

#endif // SYNTAXANALYSER_H
