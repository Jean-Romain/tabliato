#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QProcess>
#include <QFont>
#include <QMediaPlayer>

#include "file.h"
#include "motif.h"
#include "tabulature.h"
#include "keyboard.h"
#include "processor.h"
#include "highlighter.h"
#include "pdfviewer.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

    protected:
        bool eventFilter(QObject *obj, QEvent *event);
        void closeEvent(QCloseEvent *event);

    private slots:
        //void readyRead();
        //void systemcallFinished(int exit);

        void compile();
        void midi2audio();
        void midi2audioReadyRead();
        void midi2audioFinished(int);

        void save();
        void save_as();
        void open();
        void openNew();
        void open_example();

        void openAbout();
        void openHelpTabliato();
        void openHelpSyntax();
        void openHelpLilypond();
        void checkVersion();
        void openContactWebPage();

        void documentSavedTitleChange();

        void scaleDocument(int zoom);

        void displayDocks();

        //void printKeyboard();

        void insertBreak();
        void insertAlternative();
        void insertRest();
        void insertRest1();
        void insertRest2();
        void insertRest4();
        void insertRest8();
        void insertRest16();
        void insertRest32();
        void insertRest64();
        void insertRest128();
        void insertBeginBar();
        void insertEndBar();
        void insertBeginRepeatBar();
        void insertEndRepeatBar();
        void insertRepeat();
        void insertTie();
        void insertSlurs();

        void playMusic();
        void stopMusic();
        void seekMusic(qint64 time);
        void seekMusic();

        void updateRythmComboBx();
        void initRythmComboBx();
        void initAccordionComboBox();
        void initSf2ComboBox();

    private:
        Ui::MainWindow *ui;
        Highlighter *highlighterTab;
        QMediaPlayer *music;
        QProcess *midi2audioCall;
        PdfViewer *pdf;
        QVector<qreal> scaleFactors;

        QString currentOpenedFile;
        bool documentIsSaved;

        QMap<QString, Motif> rythmList;
        QMap<QString, QString> sf2List;
        QMap<QString, QString> accordionList;
        QMap<QString, QString> accordionListReverted;

        enum InsertText
        {
            REPEAT,
            BREAK,
            ALTERNATIVE,

            REST,
            REST1,
            REST2,
            REST4,
            REST8,
            REST16,
            REST32,
            REST64,
            REST128,

            ENDBAR,
            BEGINBAR,
            BEGINREPEATBAR,
            ENDREPEATBAR,

            TIE,
            SLURS
        };

    private:
        Tabulature readMusicFromUI();

        void updateUIFromMusic(Tabulature tab);
        void updateCode(QString bass, QString tabulature, QString staff, QString lilypondCode);
        void terminal(QString str);
        void open(QString filename);
        void insertText(int text);
        void setIcons();
        void updatePreview(QString);
        void exportFiles();
        void writeSettings();
        void readSettings();
        void restoreCheckedDock();
        bool maybeSave();

/*private slots:
//    void checkSearchText(const QString &text);
//    void openFile();
    void scaleDocument(int index);
    void searchDocument();
    void showSelectedText(const QString &text);*/

/*private:
    DocumentWidget *documentWidget;
    QString lastPath;
    QVector<qreal> scaleFactors;*/
};

#endif // MAINWINDOW_H
