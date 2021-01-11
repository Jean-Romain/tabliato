#ifndef MOTIF_H
#define MOTIF_H

#include <QString>

#include "syntaxanalyser.h"

class Motif : public SyntaxAnalyser
{
public:
    Motif();
    Motif(QString str);
    Motif(int num);
    Motif(QString metric, int num);
    void set(QString str);
    void setBeat(QString metric);
    QString getBeat(QString metric);
    QString parseBass(QString str);
    QString parseBassSet(QString str);
    QString parseExplicitBass(QString str);
    bool isCompatible(QString metric);

public:
    QString compactMotif;
    QString motif;
    QString beat;
    int number;

private:
    void computeNumMotif();
    void computeCompactMotif();
    void computeMotif();
    //int noteLength();
    QString parseMotif(int num, QString note = "");
    QString interpretLetter(QString str, QString duration);
};

#endif // MOTIF_H
