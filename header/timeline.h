#ifndef TIMELINE_H
#define TIMELINE_H

#include <QVector>
#include <QDebug>

struct TimeUnit
{
    TimeUnit();
    TimeUnit(float duration, int pos);
    float duration;
    int pos;
    float time;

    friend QDataStream& operator <<(QDataStream& stream, const TimeUnit& tu);

};

typedef QVector<TimeUnit> TimeUnits;

class Timeline
{
public:
    Timeline();
    Timeline(QString tempo);
    void append(Timeline);
    void append(QString duration, int pos);
    void add_alternative();
    void set_repetition(int rep);
    void set_speed_factor(QString fraction);
    int search_note_at(float time);

private:
    void unfold();

public:
    int m_repeat;
    float m_speed;
    float m_speed_factor;
    TimeUnits main;
    QVector<TimeUnits> alternatives;

    friend class Timelines;
    friend QDataStream& operator <<(QDataStream& stream, const Timeline& tl);
};

class Timelines
{
public:
    Timelines();
    Timelines(QString tempo);
    void append(QString duration, int pos);
    void end_timeline();
    void not_end_timeline();
    void add_timeline();
    void add_alternative();
    void set_repetition(int);
    void set_speed_factor(QString fraction);
    Timeline timeline();

private:
    bool m_end_signal;
    QString m_tempo;
    QVector<Timeline> timelines;
};

QDebug operator<<(QDebug stream, const TimeUnit &tu);
QDebug operator<<(QDebug stream, const Timeline &tl);


#endif // TIMELINE_H
