#ifndef TIMELINE_H
#define TIMELINE_H

#include <QVector>

class Timeline
{
public:
    Timeline();
    Timeline(QString tempo);
    void append(QString duration, int pos);
    void set_speed_factor(QString factor);
    int search_note_at(float time);

private:
    float m_speed;
    float m_speed_factor;
    QVector<float> m_time;
    QVector<float> m_duration;
    QVector<int> m_pos;
};


#endif // TIMELINE_H
