#ifndef TABULATURE_H
#define TABULATURE_H

#include <QMap>
#include <QString>

class Tabulature
{
    public:
        Tabulature();
        ~Tabulature();

        void setTabulature(QString str);
        void setLyrics(QString str);
        void set(QString option, QString value);
        void setOptions(QMap<QString,QString> map);

        QString get(QString option);

    public:
        QString tabulature;
        QString melody;
        QString bass;

        QString lyrics;
        QString firstVerse;
        QString leftVerses;
        QString rightVerses;

        QString lilypond;

        QMap<QString, QString> options;
};

#endif // TABULATURE_H

/*QString title;
QString subtitle;
QString composer;
QString poet;
QString tagline;
QString tempo;
QString time;
QString key;
QString instrument;
QString nomenclature;
QString filename;
QString accordion;
QString bassTime;*/

/*void setTitle(QString str);
void setSubtitle(QString str);
void setComposer(QString str);
void setPoet(QString str);
void setTagline(QString str);
void setTempo(QString str);
void setTime(QString str);
void setKey(QString str);
void setClef(QString str);
void setInstrument(QString str);
void setFilename(QString str);
void setNomenclature(QString str);
void setBassTime(QString num);
void setAccordion(QString str);*/

/*QString getTitle();
QString getSubtitle();
QString getComposer();
QString getPoet();
QString getTagline();
QString getTempo();
QString getTime();
QString getKey();
QString getInstrument();
QString getFilename();
QString getNomenclature();
QString getBassTime();
QString getAccordion();*/
