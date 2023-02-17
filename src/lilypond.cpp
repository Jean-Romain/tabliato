#include <QString>
#include <QSettings>

#include "lilypond.h"

namespace Lilypond
{

QString get_lilypond_path()
{
    #ifdef Q_OS_WINDOWS
    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\wow6432node\\LilyPond", QSettings::NativeFormat); // 32 bits : HKEY_LOCAL_MACHINE\\SOFTWARE\\LilyPond
    QString lilypath = settings.value("Install_Dir").toString();
    return lilypath;
    #endif

    #ifdef Q_OS_LINUX
    return "/usr/bin";
    #endif
}

QString get_lilypond_command()
{
    #ifdef Q_OS_WINDOWS
    QString command = "\"" + get_lilypond_path() + "\\usr\\bin\\lilypond.exe" + "\"";
    #endif

    #ifdef Q_OS_LINUX
    QString command = "lilypond";
    #endif

    return command;
}

bool is_lilypond_installed()
{
    bool installed = (get_lilypond_path() == "") ? false : true;
    return installed;
}

}
