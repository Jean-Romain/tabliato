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
    QStringList decompact_motif(QString str);
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
    QString parseMotif(int num, QString note = "");
};

#endif // MOTIF_H
