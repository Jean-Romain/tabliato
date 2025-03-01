#ifndef GLOBALS_H
#define GLOBALS_H

// ALL THE GLOBAL DECLARATIONS

#include <QString>

extern QString APPDIR;     //Directory where tabliato is running
extern QString APPPATH;    //Path where tabliato is running (including binary name)

extern QString LOCAL;      // was .local/share/tabliato on linux but is now /tmp on linux and APPDIR on windows
extern QString TEMP;

extern QString OUTPUT;     // where are stored the .pdf .ly .png, .midi generated for the current music sheet
extern QString ICON;       // where are stored the incons i.e. SHARE/icon
extern QString TEMPLATE;   // where is stored the lilypond template i.e. SHARE/template
extern QString EXAMPLE;    // where are stored the example files
extern QString KEYBOARDS;  // where are stored the keyboard configuration files
extern QString HTML;
extern QString SOUNDFONTS; // where are stored the soundfont files
extern QString MIDIEXT;
extern QString AUDIOEXT;

#endif // GLOBALS_H
