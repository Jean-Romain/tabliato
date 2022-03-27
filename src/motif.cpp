#include <QStringList>
#include <QDebug>

#include <stdexcept>

#include "motif.h"

Motif::Motif()
{
}

Motif::Motif(QString str)
{
    motif = str;
}

Motif::Motif(int num)
{
    number = num;
    if (num >= 0)
        computeCompactMotif();
}

Motif::Motif(QString metric, int num)
{
    number = num;           // e.g. 22
    if (number > -1)
    {
        computeCompactMotif();  // From 22 returns the motif BrarBrar
        setBeat(metric);        // From the metrics e.g. 4/4 and the motif BrarBrar computes the duration of each element.
                                // here 4 quarter per measure 8 element to place i.e. each element is a eigth.
        computeMotif();         // get the true rythm B8 r8 a8 r8 B8 r8 a8 r8 B8
    }
}

void Motif::set(QString str)
{  
    if(str.isEmpty())
        throw std::logic_error(QString("Le motif rythmique est vide").toStdString());

    if(str.toInt() != 0)
    {
        number = str.toInt();
        computeMotif();
    }
    else
    {
        motif = str;
        computeNumMotif();
    }
}

QString Motif::getBeat(QString metric)
{
    if (!isMetric(metric))
        throw std::logic_error(QString("La metrique " + metric + "  n'est pas une metrique valide").toStdString());

    double numerateur = metric.split("/")[0].toInt();
    double denominateur = metric.split("/")[1].toInt();
    double element = compactMotif.length();
    double time = denominateur/(numerateur/element);

    if (time == 4)
        return "4";
    else if(time == 8)
        return "8";
    else if(time == 16)
        return "16";
    else if(time == 16.0/3.0)
        return "8.";
    else
        return "-1";
}

void Motif::setBeat(QString metric)
{
    beat = getBeat(metric);

    if (beat == "-1")
        throw std::logic_error(QString("Le motif rythmique " + motif + " choisi n'est pas cohérant avec la mesure définie : " + metric).toStdString());
}

void Motif::computeNumMotif()
{
    QString num;

    for(int i = 0 ;  i < motif.length() ; i++)
    {
        if(motif[i] == 'B')
            num.append("4");
        else if (motif[i] == 'a')
            num.append("5");
        else if (motif[i] == 'r')
            num.append("0");
        else
            throw std::logic_error(QString("Le motif rythmique " + motif + " n'est pas valide.").toStdString());
    }

    number = num.toInt();
}

void Motif::computeCompactMotif()
{
    QString str = QString::number(number);
    QString ret;

    for(int i = 0 ;  i < str.length() ; i++)
    {
        switch (QString(str[i]).toInt())
        {
        case 0:
            ret.append("r");
            break;
        case 1:
            ret.append("Bra");
            break;
        case 2:
            ret.append("Brar");
            break;
        case 3:
            ret.append("Brarar");
            break;
        case 4:
            ret.append("B");
            break;
        case 5:
            ret.append("a");
            break;
        default:
            throw std::logic_error(QString("Le motif rythmique " + motif + "est incorrect").toStdString());
            break;
        }
    }

    compactMotif = ret;
}

void Motif::computeMotif()
{
    motif = "";
    for (auto ch : compactMotif)
       motif.append(QString(ch) + ":" + beat + " ");
}

QString Motif::parseBass(QString str)
{
    if (motif.isEmpty()) return str;

    QStringList tmp = motif.split(" ");
    for (QString &elem : tmp)
    {
        if (elem != "")
        {
            QStringList elems = elem.split(":");
            QString duration = elems[1];
            QString letter = elems[0];
            if (letter == "B")
                elem = interpretLetter(str.replace(0, 1, str[0].toUpper()), duration);
            else if (letter == "a")
                elem = interpretLetter(str.toLower(), duration);
        }
    }

    return tmp.join(" ");
}

QString Motif::parseBassSet(QString str)
{
    if(motif.isEmpty()) return str;

    QStringList tmp = motif.split(" ");

    QString out;
    int j = 0;

    QStringList list = str.split(QRegExp(":"));

    for(int i = 0 ; i < tmp.length() ; i++)
    {
        if (tmp[i] == "")
        {

        }
        else if(tmp[i].at(0) == 'r')
        {
            out.append(tmp[i] + " ");
        }
        else if(j < list.length())
        {
            QStringList elems = tmp[i].split(":");
            QString duration = elems[1];
            QString letter = elems[0];

            if (letter == "B")
                list[j] = interpretLetter(list[j].toUpper(), duration);
            else if (letter == "a")
                list[j] = interpretLetter(list[j].toLower(), duration);

            out.append(list[j] + " ");
            j++;
        }
        else
        {
            throw std::logic_error(QString("L'ensemble basses accords " + str.split(":").join(" ") + " n'est pas conforme au motif rythmique").toStdString());
        }
    }

    if(j != list.length())
        throw std::logic_error(QString("L'ensemble basses accords " + str.split(":").join(" ") + " n'est pas conforme au motif rythmique").toStdString());

    return out;
}

QString Motif::parseExplicitBass(QString str)
{
    QString duration;
    if (extractDuration.indexIn(str) >= 0)
        duration = extractDuration.cap(1);

    QString note = str.mid(0, str.indexOf(":"));
    note = interpretLetter(note, duration);
    return note;
}

QString Motif::interpretLetter(QString str, QString duration)
{
    str.replace(QRegExp("(^A$)"), "<a, a>" + duration + "^\"A\"");
    str.replace(QRegExp("(^a$)"), "<a c e>" + duration + "^\"a\"");
    str.replace(QRegExp("(^C$)"), "<c, c>" + duration + "^\"C\"");
    str.replace(QRegExp("(^c$)"), "<c e g>" + duration + "^\"c\"");
    str.replace(QRegExp("(^E$)"), "<e, e>" + duration + "^\"E\"");
    str.replace(QRegExp("(^e$)"), "<e gis b>" + duration + "^\"e\"");
    str.replace(QRegExp("(^F$)"), "<f, f>" + duration + "^\"F\"");
    str.replace(QRegExp("(^f$)"), "<f a c>" + duration + "^\"f\"");
    str.replace(QRegExp("(^G$)"), "<g, g>" + duration + "^\"G\"");
    str.replace(QRegExp("(^g$)"), "<g b d>" + duration + "^\"g\"");
    str.replace(QRegExp("(^D$)"), "<d, d>" + duration + "^\"D\"");
    str.replace(QRegExp("(^d$)"), "<d fis a>" + duration + "^\"d\"");

    str.replace(QRegExp("(^Am$)"), "<a, a>" + duration + "^\"A\"");
    str.replace(QRegExp("(^am$)"), "<a c e>" + duration + "^\"a\"");
    str.replace(QRegExp("(^Cm$)"), "<c, c>" + duration + "^\"C\"");
    str.replace(QRegExp("(^cm$)"), "<c e g>" + duration + "^\"c\"");
    str.replace(QRegExp("(^Em$)"), "<e, e>" + duration + "^\"E\"");
    str.replace(QRegExp("(^em$)"), "<e g b>" + duration + "^\"e\"");
    str.replace(QRegExp("(^Fm$)"), "<f, f>" + duration + "^\"F\"");
    str.replace(QRegExp("(^fm$)"), "<f a c>" + duration + "^\"f\"");
    str.replace(QRegExp("(^Gm$)"), "<g, g>" + duration + "^\"G\"");
    str.replace(QRegExp("(^gm$)"), "<g b d>" + duration + "^\"g\"");
    str.replace(QRegExp("(^Dm$)"), "<d, d>" + duration + "^\"D\"");
    str.replace(QRegExp("(^dm$)"), "<d f a>" + duration + "^\"d\"");
    //str.replace(QRegExp("(^r$)"), "r" + duration);
    return str;
}

QString Motif::parseMotif(int num, QString note)
{
   switch (num)
   {
   case 0:
       return "r" + beat + " ";
       break;
   case 1: // B r a
       return parseMotif(4, note) + parseMotif(0) + parseMotif(5, note);
       break;
   case 2: // B r a r
       return parseMotif(4, note) + parseMotif(0) + parseMotif(5, note) + parseMotif(0);
       break;
   case 3: // B r a r a r
       return parseMotif(4, note) + parseMotif(0) + parseMotif(5, note) + parseMotif(0)  + parseMotif(5, note) + parseMotif(0);
       break;
   case 4: // Basse
       note[0] = note[0].toUpper();
       return note + " "; //":" + beat + " ";
       break;
   case 5: // Accord
       note[0] = note[0].toLower();
       return note + " "; //":" + beat + " ";
       break;

   default:
       QString str = QString::number(num);
       QString ret;

       for(int i = 0 ; i < str.length() ; i++)
           ret += parseMotif(QString(str[i]).toInt(), note);

       return ret;
       break;
   }
}

/*int Motif::noteLength()
{
    QString str = parseMotif(number, "A");
    return str.split(" ").length() -1;
}*/

bool Motif::isCompatible(QString metric)
{
    if (number == -1) return true;
    return (getBeat(metric) == "-1")?false:true;
}
