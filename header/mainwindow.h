#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTime>
#include <QProcess>
#include <QFont>
#include <QMediaPlayer>

#include "timeline.h"
#include "file.h"
#include "motif.h"
#include "tabulature.h"
#include "keyboard.h"
#include "processor.h"
#include "highlighter.h"
#include "pdfviewer.h"
#include "filedownloader.h"

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

        void openAbout();
        void checkVersion();
        void download_soundfonts(QString name);

        void documentSavedTitleChange();
        void goto_line(int line);

        void displayDocks();

        //void printKeyboard();

        void playMusic();
        void stopMusic();
        void seekMusic(qint64 time);
        void seekMusic();
        void updateMusic(QMediaPlayer::State);

        void updateRythmComboBx();

        void highlight_notes_from_current_line_in_pdf();
        void highlight_notes_from_current_music_time_in_pdf();

    private:
        Ui::MainWindow *ui;
        Highlighter *highlighterTab;
        QMediaPlayer *music;
        QProcess *midi2audioCall;
        PdfViewer *pdf;
        QVector<qreal> scaleFactors;
        QTimer *timer;

        QString currentOpenedFile;
        bool documentIsSaved;
        Timeline m_timeline;

        QMap<QString, Motif> rythmList;
        QMap<QString, QString> accordionList;
        QMap<QString, QString> accordionListReverted;

    public:
       void open(QString filename);

    private:
        Tabulature readMusicFromUI();
        void initRythmComboBx();
        void initAccordionComboBox();
        void initSf2ComboBox();
        void updateUIFromMusic(Tabulature tab);
        void terminal(QString str);
        void setIcons();
        void updatePreview(QString);
        void updateRessources();
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
