#ifndef TBOHIGHLIGHTER_H
#define TBOHIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>

class Highlighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    enum Construct
    {
        Entity,
        Tag,
        Comment,
        Push,
        Pull,
        LastConstruct = Comment
    };

    Highlighter(QTextDocument *parent = 0);

    void setFormatFor(Construct construct, const QTextCharFormat &format);
    QTextCharFormat formatFor(Construct construct) const{ return m_formats[construct]; }

protected:
     void highlightBlock(const QString &text);

private:
     struct HighlightingRule
     {
         QRegExp pattern;
         QTextCharFormat format;
     };

     enum State
     {
         NormalState = -1,
         InComment,
         InTag,
         InPush,
         InPull
     };

     QVector<HighlightingRule> highlightingRules;

     QTextCharFormat keywordFormat;
     QTextCharFormat commentFormat;
     QTextCharFormat timeFormat;
     QTextCharFormat bassFormat;
     QTextCharFormat chordFormat;
     QTextCharFormat restFormat;
     QTextCharFormat bracketFormat;
     QTextCharFormat fingeringFormat;

     QTextCharFormat m_formats[LastConstruct + 1];
};

#endif // TBOHIGHLIGHTER_H
