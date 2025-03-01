#include "timeline.h"
#include "button.h"
#include <QDebug>
#include <cmath>

QDebug operator<<(QDebug stream, const TimeUnit &tu)
{
    stream << "[" << std::ceil(tu.duration * 100.0) / 100.0 <<  " " << tu.pos << " " << std::ceil(tu.time * 100.0) / 100.0  << "]";
    return stream;
}

QDebug operator<<(QDebug stream, const Timeline &tl)
{
    stream << "Main =" << tl.main << "\n";
    for (auto& alt : tl.alternatives)
        stream << "   Alternative =" << alt << "\n";
    return stream;
}


TimeUnit::TimeUnit()
{
    this->duration = 0;
    this->pos = 0;
    this->time = 0;
}

TimeUnit::TimeUnit(float duration, int pos)
{
    this->duration = duration;
    this->pos = pos;
}

Timeline::Timeline()
{
    m_repeat = 1;
}

Timeline::Timeline(QString tempo)
{
    float basetime = Button::get_duration_as_whole_note(tempo.split("=").at(0));
    int bps = tempo.split("=").at(1).toInt();
    m_speed = (1.0/((float)bps/60.0)) / basetime;
    m_speed_factor = 1;
    m_repeat = 1;
}

void Timeline::set_repetition(int rep)
{
    m_repeat = rep;
}

void Timeline::append(QString duration, int pos)
{
    float t = Button::get_duration_as_whole_note(duration);     // durée equivalente en noire. Si croche -> 0.5
    float duration_in_second = t * m_speed * m_speed_factor;    // durée de la note en second d'après le tempo
    TimeUnit tu(duration_in_second, pos);

    TimeUnits* target = (alternatives.size() == 0) ? &main : &alternatives.last();

    if (!target->isEmpty())
        tu.time = (target->last().time + duration_in_second);
    else
        tu.time = duration_in_second;

    target->append(tu);
    return;
}

void Timeline::append(Timeline line)
{
    this->unfold();
    line.unfold();
    int n1 = main.size();
    int n2 = line.main.size();
    int n = n1 + n2;

    TimeUnits units;
    units.reserve(n);

    for (int i = 0 ; i < n1 ; i++)
        units.insert(units.end(), main[i]);

    for (int i = 0 ; i < n2 ; i++)
        units.insert(units.end(), line.main[i]);


    units[0].time = units[0].duration;
    for (int i = 1 ; i < n ; i++)
        units[i].time = units[i].duration + units[i-1].time;

    main = units;
    m_repeat = 1;
    return;
}

void Timeline::add_alternative()
{
    if (m_repeat == 1)
        throw std::logic_error("Détection d'une alternative sans répétition.");

    alternatives.append(TimeUnits());
}

int Timeline::search_note_at(float time)
{
    auto myComp = [&](TimeUnit e1, float e2)
    {
        return e1.time < e2;
    };

    auto lower = std::lower_bound(main.begin(), main.end(), time, myComp);
    int pos = std::distance(main.begin(), lower);

    if (pos < main.size())
        return main[pos].pos;
    else
        return -1;
}

void Timeline::set_speed_factor(QString fraction)
{
    QStringList frac = fraction.split("/");
    m_speed_factor = frac[1].toFloat()/frac[0].toFloat();
}

void Timeline::unfold()
{
     //qDebug() << "unfold" << m_repeat << alternatives.size();

    if (m_repeat > 1 && alternatives.size() > 0 && alternatives.size() != m_repeat)
        throw std::logic_error("Erreur interne: incompatibilité dans une structure de répétition");

    int n1 = main.size();
    int r1 = m_repeat;
    int n = n1*r1;

    TimeUnits units;
    units.reserve(n);

    if (n1 > 0)
    {
        for (int k = 0 ; k < r1 ; k++)
        {
            for (int i = 0 ; i < n1 ; i++)
                units.insert(units.end(), main[i]);

            if (alternatives.size() > 0)
            {
                for (int i = 0 ; i < alternatives[k].size() ; i++)
                    units.insert(units.end(), alternatives[k][i]);
            }
        }

        units[0].time = units[0].duration;
        for (int i = 1 ; i < units.size() ; i++)
             units[i].time = units[i].duration + units[i-1].time;
    }

    main = units;
    m_repeat = 1;


    //qDebug() << units;
    return;
}

Timelines::Timelines()
{
}

Timelines::Timelines(QString tempo)
{
    m_end_signal = false;
    m_tempo = tempo;
}

void Timelines::append(QString duration, int pos)
{
    if (m_end_signal || timelines.isEmpty())
        add_timeline();

    timelines.last().append(duration, pos);

    //for (auto tl : timelines)
        //qDebug() << tl;
}

void Timelines::add_timeline()
{
    //qDebug() << "Add timeline: new size =" << timelines.size()+1;

    if (m_end_signal)
        m_end_signal = false;

    timelines.append(Timeline(m_tempo));
}

void Timelines::end_timeline()
{
    //qDebug() << "End timeline: new state =" << !m_end_signal;
    m_end_signal = true;
}

void Timelines::not_end_timeline()
{
    //qDebug() << "End timeline: new state =" << !m_end_signal;
    m_end_signal = false;
}

void Timelines::add_alternative()
{
    timelines.last().add_alternative();
}


void Timelines::set_repetition(int rep)
{
    timelines.last().set_repetition(rep);
}

void Timelines::set_speed_factor(QString fraction)
{
    if (m_end_signal || timelines.isEmpty())
        add_timeline();

    timelines.last().set_speed_factor(fraction);
}


Timeline Timelines::timeline()
{
    //qDebug() << "Build final timeline";

    if (timelines.size() == 1)
    {
        timelines[0].unfold();
        return timelines[0];
    }

    Timeline res;
    for (Timeline tl : timelines)
    {
        //qDebug() << "Merging:\n" << tl << "with\n" <<  res;
        res.append(tl);
        //qDebug() << "gives:" << res;
    }

    return res;
}


