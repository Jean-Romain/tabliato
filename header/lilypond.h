#ifndef LILYPOND_H
#define LILYPOND_H

#include <QString>

namespace Lilypond
{
    QString get_lilypond_path();
    QString get_lilypond_command();
    bool is_lilypond_installed();
}

#endif // LILYPOND_H
