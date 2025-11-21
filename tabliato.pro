TEMPLATE = app
TARGET = tabliato

# Version definition
VERSION = 1.3.7
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

# Platform-specific settings
win32 {
    INCLUDEPATH += . ./header ./poppler/include/poppler/qt5
    LIBS += -L./poppler/bin -lpoppler-qt5 -lstdc++
    INCLUDEPATH += C:/Qt/Tools/OpenSSL/Win_x86/include
    LIBS += -LC:/Qt/Tools/OpenSSL/Win_x86/bin -llibcrypto-1_1 -llibssl-1_1
    RC_ICONS = ./ressources/icons/tabliato.ico
    Release:DESTDIR = windows
}

unix {
    INCLUDEPATH += . ./header /usr/include/poppler/qt5
    LIBS += -L/usr/local/lib -lpoppler-qt5 -lstdc++
}

# Qt modules
QT += core widgets xml network multimedia

# Deprecated API warnings
DEFINES += QT_DEPRECATED_WARNINGS

# Source files
HEADERS  += $$files(header/*.h)
SOURCES  += $$files(src/*.cpp)
FORMS    += $$files(ui/*.ui)
RESOURCES += Ressources.qrc
