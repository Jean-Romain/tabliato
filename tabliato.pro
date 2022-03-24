TEMPLATE = app
TARGET = tabliato

win32{
INCLUDEPATH += . ./header ./poppler/include/poppler/qt5
LIBS += -L./poppler/bin -lpoppler-qt5 -lstdc++
Release:DESTDIR = windows
#RC_ICONS = ./share/icon/tabliato.svg
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


HEADERS += \
           header/global.h \
           header/mainwindow.h \
           header/versiondialog.h \
           header/highlighter.h \
           header/pdfviewer.h \
           header/codeeditor.h \
           header/button.h \
           header/file.h \
           header/keyboard.h \
           header/motif.h \
           header/processor.h \
           header/syntaxanalyser.h \
           header/tabulature.h
FORMS += ui/mainwindow.ui \
         ui/versiondialog.ui
SOURCES += \
           src/global.cpp \
           src/main.cpp \
           src/mainwindow.cpp \
           src/tabulature.cpp \
           src/versiondialog.cpp \
           src/highlighter.cpp \
           src/pdfviewer.cpp \
           src/codeeditor.cpp \
           src/button.cpp \
           src/file.cpp \
           src/keyboard.cpp \
           src/motif.cpp \
           src/syntaxanalyser.cpp \
           src/processor.cpp

QT_LOGGING_RULES=*.debug=true qt-creator
