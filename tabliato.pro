TEMPLATE = app
TARGET = tabliato

win32{
INCLUDEPATH += . ./header ./poppler/include/poppler/qt5
LIBS += -L./poppler/bin -lpoppler-qt5 -lstdc++
INCLUDEPATH += C:/Qt/Tools/OpenSSL/Win_x86/include
LIBS += -LC:/Qt/Tools/OpenSSL/Win_x86/bin -llibcrypto-1_1 -llibssl-1_1
Release:DESTDIR = windows
RC_ICONS = ./ressources/icons/tabliato.ico
}

unix{
INCLUDEPATH += . ./header /usr/include/poppler/qt5
LIBS += -L/usr/local/lib -lpoppler-qt5 -lstdc++
}

QT += core\
    widgets\
    xml\
    network\
    multimedia

QMAKE_CXXFLAGS += -DQT_COMPILING_QSTRING_COMPAT_CPP

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


HEADERS += header/*.h
FORMS += ui/*.ui
SOURCES += src/*.cpp

QT_LOGGING_RULES=*.debug=true qt-creator

RESOURCES += \
    Ressources.qrc
