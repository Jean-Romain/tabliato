#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <QMultiMap>

class Keyboard
{
public:
    Keyboard();
    Keyboard(QString name);
    QString get_note_from_button(QString button);
    QList<QString> get_buttons_from_note(QString note);
    //QStringList keys();
    int ranks();

private:
    void read_keyboard_from_assemblage(QString name);
    void build_note_to_button_map();
    void set_number_of_ranks();
    QMultiMap<QString, QString> read_keyboard_from_name(QString name);

private:
    int m_number_of_ranks;
    QString m_name;
    QMultiMap<QString, QString> m_map_button_to_note;
    QMultiMap<QString, QString> m_map_note_to_buttons;
};

#endif // KEYBOARD_H
