#include "timeline.h"
#include "button.h"

Timeline::Timeline()
{
    QString tempo("4=120");
    float basetime = Button::get_duration_as_whole_note(tempo.split("=").at(0));
    int bps = tempo.split("=").at(1).toInt();
    m_speed = (1.0/((float)bps/60.0)) / basetime;
    m_speed_factor = 1;
}

Timeline::Timeline(QString tempo)
{
    float basetime = Button::get_duration_as_whole_note(tempo.split("=").at(0));
    int bps = tempo.split("=").at(1).toInt();
    m_speed = (1.0/((float)bps/60.0)) / basetime;
    m_speed_factor = 1;
}

void Timeline::append(QString duration, int pos)
{
    float t = Button::get_duration_as_whole_note(duration); // durée equivalente en noire. Si croche -> 0.5
    float duration_in_second = t * m_speed * m_speed_factor;    // durée de la note en second d'après le tempo
    m_duration.append(duration_in_second);
    m_pos.append(pos);

    if (!m_time.isEmpty())
        m_time.append(m_time.last() + duration_in_second);
    else
        m_time.append(duration_in_second);

    return;
}

int Timeline::search_note_at(float time)
{
    auto lower = std::lower_bound(m_time.begin(), m_time.end(), time);
    int pos = std::distance(m_time.begin(), lower);

    if (pos < m_time.size())
        return m_pos[pos];
    else
        return -1;
}

void Timeline::set_speed_factor(QString factor)
{
    // TODO
}
