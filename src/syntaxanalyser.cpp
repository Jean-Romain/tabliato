#include "syntaxanalyser.h"

SyntaxAnalyser::SyntaxAnalyser()
{
    button_rgx.setPattern("\\$?[ptPT]?(\\d{1,2}'{0,2}){1}(:\\d{0,2})?\\.?~?");                  // matches a pressed button e.g. p4, p4, t5', p6:4, p6':4. t7':2~
    comment_rgx.setPattern("%.*");                                                              // matches a latex comment e.g. % blablabla
    direction_rgx.setPattern("[ptPT]");                                                         // matches a push/pull directive e.g. p, P, T
    impliciteBass_rgx.setPattern("^[a-gA-G]{1}[mb#]?$");                                        // matches a left hand implicit pattern e.g. A, Am, G, Gb
    expliciteBass_rgx.setPattern("[a-gA-G]{1}[mb#]?:?\\d{1,2}.?~?");                            // matches a left hand explicit pattern e.g. A:4 g:4 g:4
    rest_rgx.setPattern("r(:?\\d{1,2}\\.?)?");                                                  // matches a rest symbol e.g. r, r:4, r:4.
    impliciteBassSet_rgx.setPattern("([A-Ga-gr]{1}[mb#]?[,:]){1,10}[A-Ga-gr][mb#]?");
    pushed_rgx.setPattern("^([pP]).*$");                                                        // matches a p or P at the begining
    command_rgx.setPattern("\\\\\\w+");                                                         // matches \blabla
    fingering_rgx.setPattern("/([a-zA-Z]+|\\d)");
    closeChord_rgx.setPattern(">(:\\d{1,2})?\\.?~?");                                           // matches close chord e.g. >, >:2
    note_rgx.setPattern("\\$?[a-g]{1}(is|es)?(,|'){0,2}(/[123])?(:\\d{0,2}.?)?(/[123])?~?");    // matches a to g gis ges bes cis ces a, b, c', d', d'', d'':2.
    metric_rgx.setPattern("\\d{1,2}/\\d{1,2}");                                                 // matches 3/4, 2/2, 6/8

    extractRankNote.setPattern("/(\\d)");
    extractRankButton.setPattern("('+)");
    extractDuration.setPattern(":(\\d{1,2}\\.?)");
    extractNumButton.setPattern("[ptPT]?(\\d{1,2}'{0,2})");
    extractNote.setPattern("([a-g]{1}(is|es)?(,|'){0,2})");
    extractDirection.setPattern("^([ptPT])");
    extractTie.setPattern("(~)");

    hasDirection_rgx.setPattern("^[ptPT].*$");
    isChord_rgx.setPattern("(^[ptPT]?(\\d{1,2}'?,){1,2}(\\d{1,2}'?){1}(:\\d)?\\.?)");
}

bool SyntaxAnalyser::isButton(QString str){
    return genericTest(button_rgx, str);
}

bool SyntaxAnalyser::isComment(QString str){
    return genericTest(comment_rgx, str);
}

bool SyntaxAnalyser::isDirection(QString str){
    return genericTest(direction_rgx, str);
}

bool SyntaxAnalyser::isImplicitBass(QString str){
    return genericTest(impliciteBass_rgx, str);
}

bool SyntaxAnalyser::isExplicitBass(QString str){
    return genericTest(expliciteBass_rgx, str);
}

bool SyntaxAnalyser::isRest(QString str){
    return genericTest(rest_rgx, str);
}

bool SyntaxAnalyser::isBassSet(QString str){
    return genericTest(impliciteBassSet_rgx, str);
}

bool SyntaxAnalyser::isPushed(QString str){
    return genericTest(pushed_rgx, str);
}

bool SyntaxAnalyser::isCommand(QString str){
    return genericTest(command_rgx, str);
}

bool SyntaxAnalyser::isFingering(QString str){
    return genericTest(fingering_rgx, str);
}

bool SyntaxAnalyser::isNewLine(QString str){
    return str == "newline";
}

bool SyntaxAnalyser::isChord(QString str)
{
    return genericTest(isChord_rgx, str);
}

bool SyntaxAnalyser::isOpenChord(QString str){
    return str == "<";
}

bool SyntaxAnalyser::isCloseChord(QString str){
    return genericTest(closeChord_rgx, str);
}

bool SyntaxAnalyser::isOpenManualBass(QString str){
    return str == "[";
}

bool SyntaxAnalyser::isCloseManualBass(QString str){
    return str == "]";
}

bool SyntaxAnalyser::isNote(QString str){
    return genericTest(note_rgx, str);
}

bool SyntaxAnalyser::isMetric(QString str){
    return genericTest(metric_rgx, str);
}

bool SyntaxAnalyser::hasDirection(QString str)
{
    return genericTest(hasDirection_rgx, str);
}

bool SyntaxAnalyser::genericTest(QRegExp rx, QString str)
{
    return rx.exactMatch(str);
}

int SyntaxAnalyser::getType(QString str)
{
    if (str == "~")
        return TIE;
    if (isChord(str))
        return MELODYCHORD;
    else if(isButton(str))
        return BUTTON;
    else if (isDirection(str))
        return DIRECTION;
    else if (isComment(str))
        return COMMENT;
    else if (isRest(str))
        return REST;
    else if (isFingering(str))
        return FINGER;
    else if (isCommand(str))
        return COMMAND;
    else if (isNewLine(str))
        return NEWLINE;
    else if (isNote(str))
        return NOTE;
    else if (isOpenChord(str))
        return OPENCHORD;
    else if (isCloseChord(str))
        return CLOSECHORD;
    else if (isOpenManualBass(str))
        return OPENMANUALBASS;
    else if (isCloseManualBass(str))
        return CLOSEMANUALBASS;
    else if (isBassSet(str))
        return BASSCHORD;
    else if (isImplicitBass(str))
        return IMPLICITBASS;
    else if (isExplicitBass(str))
        return EXPLICITBASS;
    else
        return OTHER;
}

int SyntaxAnalyser::getTypeRightHand(QString str)
{
    if (str == "~")
        return TIE;
    if (isChord(str))
        return MELODYCHORD;
    else if(isButton(str))
        return BUTTON;
    else if (isDirection(str))
        return DIRECTION;
    else if (isComment(str))
        return COMMENT;
    else if (isRest(str))
        return REST;
    else if (isFingering(str))
        return FINGER;
    else if (isCommand(str))
        return COMMAND;
    else if (isNewLine(str))
        return NEWLINE;
    else if (isNote(str))
        return NOTE;
    else if (isOpenChord(str))
        return OPENCHORD;
    else if (isCloseChord(str))
        return CLOSECHORD;
    else
        return OTHER;
}

int SyntaxAnalyser::getTypeLeftHand(QString str)
{
    if (str == "~")
        return TIE;
    else if (isComment(str))
        return COMMENT;
    else if (isBassSet(str))
        return BASSCHORD;
    else if (isImplicitBass(str))
        return IMPLICITBASS;
    else if (isExplicitBass(str))
        return EXPLICITBASS;
    else if (isRest(str))
        return REST;
    else if (isCommand(str))
        return COMMAND;
    else if (isNewLine(str))
        return NEWLINE;
    else if (isNote(str))
        return NOTE;
    else if (isOpenChord(str))
        return OPENCHORD;
    else if (isCloseChord(str))
        return CLOSECHORD;
    else
        return OTHER;
}

QString SyntaxAnalyser::getDirection(QString str)
{
    if(isPushed(str))
        return QString("p");
    else
        return QString("t");
}
