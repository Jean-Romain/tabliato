#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QTextStream>
#include <QFile>
#include <QtXml>

#include "tabulature.h"

class File
{
    public:
        File();
        ~File();

        static QString read(QString path);
        static void write(QString path, QString text);

        static Tabulature readdtb(QString path);
        static void writedtb(QString path, Tabulature tab);

        static void mkdir(QString path);
        static QDomDocument readXML(QString path);
};

#endif // FILE_H
