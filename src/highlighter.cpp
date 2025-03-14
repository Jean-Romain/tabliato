﻿#include "highlighter.h"

Highlighter::Highlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    QTextCharFormat keywordFormat;
    keywordFormat.setForeground(QColor(0, 153, 255));
    keywordFormat.setFontWeight(QFont::Bold);

    QTextCharFormat timeFormat;
    timeFormat.setForeground(QColor(204, 164, 0));

    QTextCharFormat bassFormat;
    bassFormat.setFontWeight(QFont::Bold);

    QTextCharFormat chordFormat;
    chordFormat.setFontWeight(QFont::Normal);

    QTextCharFormat restFormat;
    restFormat.setFontWeight(QFont::Normal);

    QTextCharFormat commentFormat;
    commentFormat.setForeground(QColor(120, 120, 120));

    QTextCharFormat stringFormat;
    stringFormat.setForeground(QColor(90, 134, 0));

    QTextCharFormat bracketFormat;
    bracketFormat.setForeground(QColor(122, 0, 157));

    QTextCharFormat fingeringFormat;
    fingeringFormat.setForeground(QColor(73, 144, 0));

    // Pull
    QTextCharFormat entityFormat;
    entityFormat.setForeground(QColor(180, 0, 0));

    // Push
    QTextCharFormat tagFormat;
    tagFormat.setForeground(QColor(0, 0, 180));

    // Pattern de reconnaissance des crochets
    rule.pattern = QRegExp("[\\[{}\\]]");
    rule.format = bracketFormat;
    highlightingRules.append(rule);

    // Pattern de reconnaissance des informations de temps
    QStringList timePatterns;
    timePatterns << "(:[0-9]{1,2}\\.?)"              // matches :8.
                 << "\\s\\d{1,2}/\\d{1,2}"           // matches 3/4
                 << "\\s\\d{1,2}\\*\\d{1,2}"         // matches multiplication 3*8
                 << "\\\\partial ([0-9]{1,2}\\.? )"; // matches for anacrouse
    foreach (const QString &pattern, timePatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = timeFormat;
        highlightingRules.append(rule);
    }

    // Pattern de reconnaissance des basses
    QStringList bassPatterns;
    bassPatterns << "([ABCDEFG]{1}[m]?\\d?\\d?)";
    foreach (const QString &pattern, bassPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = bassFormat;
        highlightingRules.append(rule);
    }

    // Pattern de reconnaissance des accords
    QStringList chordPatterns;
    chordPatterns << "([abcdefg]{1}[m]?\\d?\\d?)";
    foreach (const QString &pattern, chordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = chordFormat;
        highlightingRules.append(rule);
    }

    // Pattern de reconnaissance des silences
    rule.pattern = QRegExp("r\\d?\\d?");
    rule.format = restFormat;
    highlightingRules.append(rule);

    // Pattern de reconnaissance des mots clés
    QStringList keywordPatterns;
    keywordPatterns << "(\\\\[a-z]+)"                   // matches \abcdef
                    << "(volta[ ]?[0-9]?)"              // matches volta 2
                    << "(segno[ ]?[0-9]?)"              // matches segno 2
                    << "(Timing.[a-zA-Z]+ ?={1} ?.*)";  // matches Timing.beamExceptions = #'() | Timing.beatStructure=1,1,1
    foreach (const QString &pattern, keywordPatterns)
    {
        rule.pattern = QRegExp(pattern);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    // Pattern de reconnaissance des strings
    rule.pattern = QRegExp("\"[^\"]*\"");
    rule.format = stringFormat;
    highlightingRules.append(rule);

    // Pattern de reconnaissance des commentaires
    rule.pattern = QRegExp("%[^\n]*");
    rule.format = commentFormat;
    highlightingRules.append(rule);

    // Pattern de reconnaissance des notations de doigté
    rule.pattern = QRegExp("(\\\\|-)[1-5]+");
    rule.format = fingeringFormat;
    highlightingRules.append(rule);

    setFormatFor(Entity, entityFormat);
    setFormatFor(Tag, tagFormat);
}

void Highlighter::setFormatFor(Construct construct, const QTextCharFormat &format)
{
    m_formats[construct] = format;
    rehighlight();
}


void Highlighter::highlightBlock(const QString &text)
{
    int state = previousBlockState();
    int len = text.length();
    int start = 0;
    int pos = 0;

    while (pos < len)
    {
        switch (state)
        {
        case NormalState:

        default:
            while (pos < len)
            {
                QChar ch = text.at(pos);
                if (ch == '<')
                {
                    if (text.mid(pos, 4) == "<!--")
                    {
                        state = InComment;
                    }
                    else
                    {
                        state = InTag;
                    }
                    break;
                }
                else if (ch == '&')
                {
                    start = pos;
                    while (pos < len
                           && text.at(pos++) != ';')
                        ;
                    setFormat(start, pos - start, m_formats[Entity]);
                }
                else if(ch == 'p' || ch == 'P')
                {
                    state = InPush;
                    break;
                }
                else if(ch == 't' || ch == 'T')
                {
                    state = InPull;
                    break;
                }
                else
                {
                    ++pos;
                }
            }
            break;

        case InPush:
            start = pos;

            while (pos < len)
            {
                QChar ch = text.at(pos);

                if (ch == 't' || ch == 'T') //|| ch == '}' || ch == '{' || ch == '\\')
                {
                    state = NormalState;
                    break;
                }

                ++pos;
            }

            setFormat(start, pos - start, m_formats[Tag]);
            break;

        case InPull:
            start = pos;

            while (pos < len)
            {
                QChar ch = text.at(pos);

                if (ch == 'p' || ch == 'P') // || ch == '}' || ch == '{' || ch == '\\')
                {
                    state = NormalState;
                    break;
                }

                ++pos;
            }

            setFormat(start, pos - start, m_formats[Entity]);
            break;
        }

        setCurrentBlockState(state);

        foreach (const HighlightingRule &rule, highlightingRules)
        {
            QRegExp expression(rule.pattern);
            int index = expression.indexIn(text);

            while (index >= 0)
            {
                int length = expression.matchedLength();
                setFormat(index, length, rule.format);
                index = expression.indexIn(text, index + length);
            }
        }
    }
}
