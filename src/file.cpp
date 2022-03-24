#include <QDir>

#include <stdexcept>

#include "file.h"

File::File()
{

}

File::~File()
{

}

QString File::read(QString path)
{
    QFile file(path);
    QString text;

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::logic_error(QString("Impossible d'ouvrir le fichier : " + path).toStdString() );

    text = file.readAll();
    file.close();

    return text;
}


void File::write(QString path, QString text)
{
    QFile file(path);

    if(!file.open(QIODevice::WriteOnly | QIODevice::Text))
        throw std::logic_error(QString("Impossible d'écrire le fichier : " + path).toStdString());

    QTextStream out(&file);
    out.setCodec("UTF-8");
    out << text;

    file.close();
}

Tabulature File::readdtb(QString path)
{
    QDomDocument dom = readXML(path);
    QDomNode n = dom.documentElement().firstChild();
    QMap<QString, QString> options;
    QString tabulature;
    //QString bass
    QString lyric;

    while(!n.isNull())
    {
        QDomElement e = n.toElement();

        for(int i = 0 ; i < e.attributes().count(); i++)
            options.insert(e.attributes().item(i).nodeName(), e.attributes().item(i).nodeValue());

        if(e.tagName() == "tabulature")
            tabulature = e.text();
        /*else if(e.tagName() == "bass")
            bass = e.text();*/
        else if(e.tagName() == "lyric")
            lyric = e.text();

        n = n.nextSibling();
    }

    Tabulature tab;
    tab.setOptions(options);
    //tab.setLeftHand(bass);
    tab.setTabulature(tabulature);
    tab.setLyrics(lyric);

    return tab;
}

void File::writedtb(QString path, Tabulature tab)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("tabliato");
    doc.appendChild(root);

    // Properties
    QDomElement properties = doc.createElement("propertie");
    root.appendChild(properties);

    QMap<QString, QString> options = tab.options;
    QMap<QString, QString>::iterator i;
    for (i = options.begin(); i != options.end(); ++i)
        properties.setAttribute(i.key(), i.value());

    // Tabulature
    QDomElement tabulature = doc.createElement("tabulature");
    root.appendChild(tabulature);
    tabulature.appendChild(doc.createTextNode(tab.tabulature));

    // Bass
    /*QDomElement bass = doc.createElement("bass");
    root.appendChild(bass);
    bass.appendChild(doc.createTextNode(tab.lhand));*/

    // Paroles
    QDomElement lyric = doc.createElement("lyric");
    root.appendChild(lyric);
    lyric.appendChild(doc.createTextNode(tab.lyrics));

    QDomProcessingInstruction instruction;
    instruction = doc.createProcessingInstruction("xml","version=\"1.0\" encoding=\"UTF-8\"");
    doc.insertBefore(instruction,root);

    QFile xml(path);

    if(!xml.open(QFile::WriteOnly))
        throw std::logic_error(QString("Impossible d'écrire dans le fichier xml : " + path).toStdString() );

    QTextStream out(&xml);
    doc.save(out,4,QDomDocument::EncodingFromDocument);
    xml.close();
}

QDomDocument File::readXML(QString path)
{
    QFile xml(path);

    if(!xml.open(QIODevice::ReadOnly | QIODevice::Text))
        throw std::logic_error(QString("Impossible d'ouvrier le fichier xml : " + path).toStdString());

    QDomDocument dom = QDomDocument("dtb");

    if (!dom.setContent(&xml))
        throw std::logic_error(QString("Impossible de parser le fichier xml : " + path).toStdString() );

    xml.close();

    return dom;
}

void File::mkdir(QString path)
{
    QDir dir(path);

    if(!dir.exists())
        dir.mkpath(dir.path());
}
