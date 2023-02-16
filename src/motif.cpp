#include <QStringList>
#include <QDebug>
#include <cstdlib>

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
                                // here 4 quarter per measure 8 elements to place i.e. each element is a eigth.
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
    else if(std::abs(time - 16.0/3.0) < 0.001) // time == 16.0/3.0 fails on windows because of floating point error #39
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

QStringList Motif::decompact_motif(QString str)
{
    QStringList pattern = motif.split(" ");
    bool has_duration = str.split(":").size() > 1;
    bool has_bass_only = str.toUpper() == str;

    // Implicit: received something like A or C:2.
    // It needs to be unpacked A:4 a:4 a:4 or C:2. for examples
    if (str.split(" ").size() == 1)
    {
        // Case C:2
        if (isExplicitBass(str) | isRest(str))
        {
            return QStringList(str);
        }

        // Case A
        for (QString &elem : pattern)
        {
            if (elem != "")
            {
                QStringList elems = elem.split(":");
                QString duration = (elems.size() > 1) ? elems[1] : "";
                QString letter = elems[0];
                if (letter == "B")
                    elem = str.replace(0, 1, str[0].toUpper()) + ":" + duration;
                else if (letter == "a")
                    elem = str.toLower() + ":" + duration;
            }
        }
     }
    // Semi explicit: received A a c
    // It needs to be unpacked A:4 a:4 c:4
    else if (!has_duration & !has_bass_only)
    {
        QStringList out;
        int j = 0;

        QStringList list = str.split(" ");

        for(int i = 0 ; i < pattern.length() ; i++)
        {
            if (pattern[i] == "" || isRest(pattern[i]))
            {

            }
            else if (j < list.length())
            {
                QStringList elems = pattern[i].split(":");
                QString duration = elems[1];
                QString letter = elems[0];

                if (letter == "B")
                    pattern[i] = list[j].toUpper() + ":" + duration;
                else if (letter == "a")
                    pattern[i] = list[j].toLower() + ":" + duration;

                j++;
            }
            else
            {
                throw std::logic_error(QString("L'ensemble basses accords " + str.split(":").join(" ") + " n'est pas conforme au motif rythmique").toStdString());
            }
        }

        if (j != list.length())
            throw std::logic_error(QString("L'ensemble basses accords " + str.split(":").join(" ") + " n'est pas conforme au motif rythmique").toStdString());
    }
    // Semi explicit: received A C
    // It needs to be unpacked A:8 a:8 C:8 c:8
    else if (!has_duration & has_bass_only)
    {
        QStringList out;
        int j = -1;

        QStringList list = str.split(" ");

        for(int i = 0 ; i < pattern.length() ; i++)
        {
            if (pattern[i] == "" || isRest(pattern[i]))
            {

            }
            else if (j < list.length())
            {
                QStringList elems = pattern[i].split(":");
                QString duration = elems[1];
                QString letter = elems[0];

                if (letter == "B")
                    j++;

                if (letter == "B")
                    pattern[i] = list[j].toUpper() + ":" + duration;
                else if (letter == "a")
                    pattern[i] = list[j].toLower() + ":" + duration;
            }
            else
            {
                throw std::logic_error(QString("L'ensemble basses accords " + str.split(":").join(" ") + " n'est pas conforme au motif rythmique").toStdString());
            }
        }
    }
    // Explicit: received A:4 a:4 c:4
    // It needs to be kept as is but validity check
    else
    {
        pattern = str.split(" ");
        for (QString &sym : pattern)
        {
            if (extractDuration.indexIn(sym) == -1)
            {
              throw std::logic_error((QString("Impossible de mélanger des termes explicites avec durée et des termes implicite sans durée dans ") + str).toStdString());
            }
        }
    }

    return pattern;
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

bool Motif::isCompatible(QString metric)
{
    if (number == -1) return true;
    QString beat = getBeat(metric);
    return (beat == "-1")?false:true;
}
