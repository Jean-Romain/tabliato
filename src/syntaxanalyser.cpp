#include "syntaxanalyser.h"

SyntaxAnalyser::SyntaxAnalyser()
{
    // Detection du code d'indication de bouton. Liste de choses possible:
    // p4, p4', t5', p6:4, p6':4. t7':2~ p6':2.\1~ p6':2.-1~
    QString brgx = "\\$?[ptPT]?(\\d{1,2}'{0,2}){1}(:\\d{0,2})?\\.?((\\\\|-)\\d)?~?";
    QString nrgx = "\\$?[ptPT]?[a-g]{1}(is|es)?(,|'){0,3}(/[123])?(:\\d{0,2}.?)?(/[123])?~?";

    button_rgx.setPattern(brgx);                                        // matches a pressed button e.g. p4, p4', t5', p6:4, p6':4. t7':2~
    comment_rgx.setPattern("%.*");                                      // matches a latex comment e.g. % blablabla
    direction_rgx.setPattern("[ptPT]");                                 // matches a push/pull directive e.g. p, P, T
    impliciteBass_rgx.setPattern("^[a-gA-G]{1}[mb#]?$");                // matches a left hand implicit pattern e.g. A, Am, G, Gb
    expliciteBass_rgx.setPattern("[a-gA-G]{1}[mb#]?:?\\d{1,2}.?~?");    // matches a left hand explicit pattern e.g. A:4 g:4 g:4
    rest_rgx.setPattern("r(:?\\d{1,2}\\.?)?");                          // matches a rest symbol e.g. r, r4 r:4, r:4.
    pushed_rgx.setPattern("^([pP]).*$");                                // matches a p or P at the begining
    command_rgx.setPattern("\\\\\\w+");                                 // matches \blabla
    fingering_rgx.setPattern("(\\\\|-)\\d");
    closeChord_rgx.setPattern(">(:\\d{1,2})?\\.?~?");                   // matches close chord e.g. >, >:2
    openChord_rgx.setPattern("[ptPT]?<");
    note_rgx.setPattern(nrgx);                                          // matches a to g gis ges bes cis ces a, b, c', d', d'', d'':2.
    metric_rgx.setPattern("\\d{1,2}/\\d{1,2}");                         // matches 3/4, 2/2, 6/8

    extractRankNote.setPattern("/(\\d)");
    extractRankButton.setPattern("('+)");
    extractFinger.setPattern("((\\\\|-)\\d)");
    extractDuration.setPattern(":(\\d{1,2}\\.?)");
    extractNumButton.setPattern("[ptPT]?(\\d{1,2}'{0,2})");
    extractNote.setPattern("([a-g]{1}(is|es)?(,|'){0,3})");
    extractDirection.setPattern("^([ptPT])");
    extractTie.setPattern("(~)");

    hasDirection_rgx.setPattern("^[ptPT].*$");
    //isChord_rgx.setPattern("(^[ptPT]?(\\d{1,2}'?,){1,2}(\\d{1,2}'?){1}(:\\d)?\\.?)");
}

/* IS */

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
    return str == "\n";
}

bool SyntaxAnalyser::isOpenChord(QString str){
    return genericTest(openChord_rgx, str);
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

bool SyntaxAnalyser::isOpenBracket(QString str){
    return str == "{";
}

bool SyntaxAnalyser::isCloseBracket(QString str){
    return str == "}";
}

bool SyntaxAnalyser::isDoubleQuote(QString str){
    return str == "\"";
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

/* GET */

int SyntaxAnalyser::getType(QString str)
{
    if (str == "~")
        return TIE;
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
    else if (isImplicitBass(str))
        return IMPLICITBASS;
    else if (isExplicitBass(str))
        return EXPLICITBASS;
    else if (isOpenBracket(str))
        return OPENBRACKET;
    else if (isCloseBracket(str))
        return CLOSEBRACKET;
    else if (isDoubleQuote(str))
        return DOUBLEQUOTE;
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

int SyntaxAnalyser::getRank(QString str)
{
    int rank;
    extractRankButton.indexIn(str);

    if (extractRankButton.cap(0) == "")
        rank = 1;
    else if(extractRankButton.cap(0) == "'")
        rank = 2;
    else
        rank = 3;

    return rank;
}


bool SyntaxAnalyser::genericTest(QRegExp rx, QString str)
{
    return rx.exactMatch(str);
}

