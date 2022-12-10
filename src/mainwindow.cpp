#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QSize>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QSignalMapper>

#include <algorithm>

#ifdef Q_WS_WIN
#include <windows.h>
#include <shellapi.h>
#endif

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "versiondialog.h"
#include "global.h"
#include "highlighter.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    highlighterTab = new Highlighter(ui->melodie_textarea->document());
    music = new QMediaPlayer(this);
    midi2audioCall = new QProcess(this);
    pdf = new PdfViewer();
    timer = new QTimer(this);

    // ==== Basic stuff such as open/save ====
    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(save_as()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(openNew()));
    connect(ui->actionSave_tool, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionOpen_tool, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionNew_tool, SIGNAL(triggered()), this, SLOT(openNew()));
    connect(ui->actionUndo_tool, SIGNAL(triggered()), ui->melodie_textarea, SLOT(undo()));
    connect(ui->actionRedo_tool, SIGNAL(triggered()), ui->melodie_textarea, SLOT(redo()));

    // ==== Fichier d'exemples ====
    connect(ui->actionOpenExBase,      &QAction::triggered,  [=]() { this->open(":/examples/ressources/exemples/Exemple - basique.dtb"); currentOpenedFile = ""; });
    connect(ui->actionOpenExAnacrouse, &QAction::triggered,  [=]() { this->open(":/examples/ressources/exemples/Exemple - anacrouse.dtb"); currentOpenedFile = ""; });
    connect(ui->actionOpenExAccords,   &QAction::triggered,  [=]() { this->open(":/examples/ressources/exemples/Exemple - accord main droite.dtb"); currentOpenedFile = ""; });
    connect(ui->actionOpenExNotes,     &QAction::triggered,  [=]() { this->open(":/examples/ressources/exemples/Exemple - boutons ou notes.dtb"); currentOpenedFile = ""; });
    connect(ui->actionOpenExRythme,    &QAction::triggered,  [=]() { this->open(":/examples/ressources/exemples/Exemple - rythme special.dtb");  currentOpenedFile = "";});

    // ==== Trigger rendering ====
    connect(ui->actionCompile, SIGNAL(triggered()), this, SLOT(compile()));
    connect(ui->actionCompile_tool, SIGNAL(triggered()), this, SLOT(compile()));

    // ==== Various exernal opening ====
    connect(ui->actionHelpTabliato,  &QAction::triggered, [=]() { QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/doc.html")); });
    connect(ui->actionHelpSyntax,    &QAction::triggered, [=]() { QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/doc.html")); });
    connect(ui->actionHelpLilypond,  &QAction::triggered, [=]() { QDesktopServices::openUrl(QUrl("http://www.lilypond.org/introduction.fr.html")); });
    connect(ui->actionBug,           &QAction::triggered, [=]() { QDesktopServices::openUrl(QUrl("https://github.com/Jean-Romain/tabliato/issues")); });
    connect(ui->actionSuggesstions,  &QAction::triggered, [=]() { QDesktopServices::openUrl(QUrl("https://github.com/Jean-Romain/tabliato/issues")); });

     // ==== Various internal opening ====
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAbout()));
    connect(ui->actionCheckUpdate, SIGNAL(triggered()), this, SLOT(checkVersion()));

    // ==== Show/hide dock widget ====
    connect(ui->actionDisplayToolBar, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayPlayer, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayInformations, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayConsole, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayInsertFast, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayPreview, SIGNAL(triggered()), this, SLOT(displayDocks()));

    // ==== When the text changed the document is no longer saved =====
    connect(ui->melodie_textarea, SIGNAL(textChanged()), this, SLOT(documentSavedTitleChange()));

    // ==== Insertion rapide ====
    connect(ui->actionInsertRepeat,      &QAction::triggered,   [=]() { this->ui->melodie_textarea->insertPlainText("\\repeat volta 2\n{\n\n}\n"); });
    connect(ui->actionInsertBreak,       &QAction::triggered,   [=]() { this->ui->melodie_textarea->insertPlainText("\\break"); });
    connect(ui->actionInsertAlternative, &QAction::triggered,   [=]() { this->ui->melodie_textarea->insertPlainText("\\alternative\n{\n  {  }\n  {  }\n}"); });
    connect(ui->actionInsertEndBar,      &QAction::triggered,   [=]() { this->ui->melodie_textarea->insertPlainText("\\bar \"|.\" "); });
    connect(ui->endbar,                  &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("\\bar \"|.\" "); });
    connect(ui->beginbar,                &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("\\bar \".|\" "); });
    connect(ui->beginrepaeat,            &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("\\bar \".|:\" "); });
    connect(ui->endrepeat,               &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("\\bar \":|.\" "); });
    connect(ui->rest1,                   &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r1 "); });
    connect(ui->rest2,                   &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r2 "); });
    connect(ui->rest4,                   &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r4 "); });
    connect(ui->rest8,                   &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r8 "); });
    connect(ui->rest16,                  &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r16 "); });
    connect(ui->rest32,                  &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r32 "); });
    connect(ui->rest64,                  &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r64 "); });
    connect(ui->rest128,                 &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("r128"); });
    connect(ui->tie,                     &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("~ "); });
    connect(ui->slurs,                   &QPushButton::clicked, [=]() { this->ui->melodie_textarea->insertPlainText("( ) "); });

    // ==== Music player ====
    connect(ui->play_pushButton, SIGNAL(clicked()), this, SLOT(playMusic()));
    connect(ui->stop_pushButton, SIGNAL(clicked()), this, SLOT(stopMusic()));
    connect(music, SIGNAL(positionChanged(qint64)), this, SLOT(seekMusic(qint64)));
    connect(ui->time_slider, SIGNAL(sliderReleased()), this, SLOT(seekMusic()));
    connect(music, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(updateMusic(QMediaPlayer::State)));

    // ==== Music renderer ====
    connect(midi2audioCall,SIGNAL(readyReadStandardOutput()),this,SLOT(midi2audioReadyRead()));
    connect(midi2audioCall,SIGNAL(finished(int)),this,SLOT(midi2audioFinished(int)));

    // ===- Music download soundfonts ====
    connect(ui->actionBallone_Burini,  &QAction::triggered, [=]() { this->download_soundfonts("BalloneBurini.sf2"); });
    connect(ui->actionLoffet,          &QAction::triggered, [=]() { this->download_soundfonts("Loffet.sf2"); });
    connect(ui->actionGaillard,        &QAction::triggered, [=]() { this->download_soundfonts("Gaillard.sf2"); });
    connect(ui->actionSalta_Bourroche, &QAction::triggered, [=]() { this->download_soundfonts("SaltaBourroche_Light.sf2"); });
    connect(ui->actionSerafini,        &QAction::triggered, [=]() { this->download_soundfonts("Serafini.sf2"); });
    connect(ui->showsf2ddl_pushButton, &QPushButton::clicked, [=]() { QMessageBox::information(this, "Soundfonts", "Pour télécharger d'autres fichiers de rendu audio allez dans: Audio > Installer des fontes sonores"); });

    // ==== PDF interaction between ui and rendering ====
    connect(ui->nextPagePushButton, &QPushButton::clicked, [=]() { pdf->nextPage(); });
    connect(ui->previousPagePushButton, &QPushButton::clicked, [=]() { pdf->previousPage(); });
    connect(pdf, &PdfViewer::pageChanged, [=]() { ui->nextPagePushButton->setDisabled(false);  ui->previousPagePushButton->setDisabled(false); });
    connect(pdf, &PdfViewer::firstPage, [=]() { ui->previousPagePushButton->setDisabled(true); });
    connect(pdf, &PdfViewer::lastPage, [=]() { ui->nextPagePushButton->setDisabled(true); });
    connect(ui->zoomInPushButton, &QPushButton::clicked, [=]() { pdf->zoomIn(); });
    connect(ui->zoomOutPushButton, &QPushButton::clicked, [=]() { pdf->zoomOut(); });
    connect(ui->zoomFitPushButton, &QPushButton::clicked, [=]() { pdf->zoomFit(); });
    connect(ui->pdfZoomSlider, &QSlider::valueChanged, [=]() { pdf->setScale((float)ui->pdfZoomSlider->value()/100.0); });
    connect(pdf, SIGNAL(scaleChanged(int)), ui->pdfZoomSlider, SLOT(setValue(int)));

    // === PDF interaction between code and rendering ===
    connect(pdf, SIGNAL(linkClicked(int)), this, SLOT(goto_line(int)));
    connect(ui->melodie_textarea, SIGNAL(cursorPositionChanged()), this, SLOT(highlight_notes_from_current_line_in_pdf()));

    // === PDF interaction between music player and rendering ===
    connect(timer, SIGNAL(timeout()), this, SLOT(highlight_notes_from_current_music_time_in_pdf()));

    // === Misc ====
    connect(ui->time_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateRythmComboBx()));

    // Fix #19 window only. This works natively on linux
    #ifdef Q_OS_WINDOWS
    ui->time_combobox->setStyleSheet("QComboBox QAbstractItemView {min-width: 70;}");
    ui->key_combobox->setStyleSheet("QComboBox QAbstractItemView {min-width: 100;}");
    ui->accordion_comboBox->setStyleSheet("QComboBox QAbstractItemView {min-width: 300;}");
    #endif

    ui->scrollArea->setWidget(pdf);
    ui->toolBox->setCurrentIndex(0);
    ui->toolBox_2->setCurrentIndex(0);

    setIcons();
    initRythmComboBx();
    initAccordionComboBox();
    initSf2ComboBox();
    readSettings();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete highlighterTab;
    delete music;
    delete midi2audioCall;
    delete pdf;
}

void MainWindow::compile()
{
    if (!currentOpenedFile.isEmpty()) save();

    terminal("Compilation");

    Tabulature tab = readMusicFromUI();

    try
    {
        QElapsedTimer timer;
        timer.start();

        TabliatoProcessor proc(tab);
        m_timeline = proc.m_timeline;
        //terminal(proc.get_logs());
        terminal("Tablature parsée en " + QString::number(timer.elapsed()) + " milliseconds");
    }
    catch(const std::exception &e)
    {
        QMessageBox::critical(this, "Erreur", QString(e.what()));
        terminal(e.what());
        return;
    }

    QString dtb = OUTPUT + "/output.dtb";
    File::mkdir(OUTPUT);
    File::writedtb(dtb, tab);

    QElapsedTimer timer;
    timer.start();

    QProcess tabliato;
    QStringList arguments;
    arguments.append("--png");
    arguments.append("--pdf");
    arguments.append("--midi");
    arguments.append("--ly");
    arguments.append(dtb);

    tabliato.start(APPPATH, arguments);
    tabliato.waitForFinished(5000);

    if (tabliato.exitCode() != 0)
    {
        QString err = tabliato.readAllStandardError();
        QMessageBox::critical(this, "Erreur :", err);
        return;
    }

    if (!tabliato.readAllStandardOutput().isEmpty())
        terminal(tabliato.readAllStandardOutput());

    terminal("Gravure terminée en " + QString::number(timer.elapsed()) + " milliseconds");
    updatePreview(OUTPUT + "/output.pdf");
    stopMusic();
    midi2audio();
}

void MainWindow::midi2audio()
{
    terminal("Génération du rendu audio");
    ui->play_pushButton->setEnabled(false);

    QStringList arguments;
    arguments.append("--soundfont=" + SOUNDFONTS + "/" + ui->sf2_combobox->currentText());
    arguments.append(OUTPUT + "/output" + MIDIEXT);
    midi2audioCall->start(APPPATH, arguments);
}

void MainWindow::midi2audioReadyRead()
{
    terminal(midi2audioCall->readAllStandardOutput());
}

void MainWindow::midi2audioFinished(int exit)
{
    exportFiles();
    if (exit) QMessageBox::critical(this, "Erreur", "La musique n'a pas pu être générée pour une raison inconnue");
    ui->play_pushButton->setEnabled(true);
    QUrl audio = QUrl::fromLocalFile(OUTPUT+ "/output" + AUDIOEXT);
    music->setMedia(QMediaContent());
    music->setMedia(audio);
    terminal("Génération du rendu audio terminé");
}

void MainWindow::updatePreview(QString path)
{
    ui->previousPagePushButton->setDisabled(false);
    ui->nextPagePushButton->setDisabled(false);

    if (!pdf->setDocument(path))
    {
        QMessageBox::warning(this, "PDF Viewer - Failed to Open File", "The specified file could not be opened.");
        return;
    }

    QElapsedTimer timer;
    timer.start();
    pdf->show();
    terminal("Rendu graphique terminée en " + QString::number(timer.elapsed()) + " milliseconds");
    ui->pdfZoomSlider->setEnabled(true);
}

void MainWindow::updateUIFromMusic(Tabulature tab)
{
    initRythmComboBx();

    //ui->basse_textarea->setPlainText(tab.lhand);
    ui->melodie_textarea->setPlainText(tab.tabulature);
    ui->lyrics_textarea->setPlainText(tab.lyrics);
    ui->title_LineEdit->setText(tab.get("title"));
    ui->subtitle_LineEdit->setText(tab.get("subtitle"));
    ui->composer_LineEdit->setText(tab.get("composer"));
    ui->poet_LineEdit->setText(tab.get("poet"));
    ui->tagline_LineEdit->setText(tab.get("tagline"));
    ui->instrument_LineEdit->setText(tab.get("instrument"));
    ui->time_combobox->setCurrentIndex(ui->time_combobox->findText(tab.get("time")));
    ui->key_combobox->setCurrentIndex(ui->key_combobox->findText(tab.get("key")));
    ui->rythm_comboBox->setCurrentIndex(ui->rythm_comboBox->findText(tab.get("rythm")));
    ui->accordion_comboBox->setCurrentIndex(ui->accordion_comboBox->findText(accordionListReverted.value(tab.get("accordion"))));

    QStringList tempo = tab.get("tempo").split("=");
    ui->tempo_comboBox->setCurrentIndex(ui->tempo_comboBox->findText(tempo[0]));
    ui->tempo_spinBox->setValue(tempo[1].toInt());

    if(tab.get("system") == "cadb")
        ui->cadb_radio->setChecked(true);
    else
        ui->cadb_radio->setChecked(true);

    if ((tab.get("displayFingering") == "true") || (tab.get("displayFingering") == "auto"))
        ui->displayFingering_yes->setChecked(true);
    else
        ui->displayFingering_no->setChecked(true);
}

Tabulature MainWindow::readMusicFromUI()
{
    Tabulature tab;

    //tab.lhand = ui->basse_textarea->toPlainText();
    tab.tabulature = ui->melodie_textarea->toPlainText();
    tab.lyrics = ui->lyrics_textarea->toPlainText();
    tab.set("title", ui->title_LineEdit->text());
    tab.set("subtitle", ui->subtitle_LineEdit->text());
    tab.set("composer", ui->composer_LineEdit->text());
    tab.set("poet", ui->poet_LineEdit->text());
    tab.set("tagline", ui->tagline_LineEdit->text());
    tab.set("time", ui->time_combobox->currentText());
    tab.set("key", ui->key_combobox->currentText());
    tab.set("instrument", ui->instrument_LineEdit->text());
    tab.set("rythm", ui->rythm_comboBox->currentText());
    tab.set("motif", QString::number(rythmList.value(ui->rythm_comboBox->currentText()).number));
    tab.set("tempo", ui->tempo_comboBox->currentText() + "=" + QString::number(ui->tempo_spinBox->value()));
    tab.set("accordion", accordionList.value(ui->accordion_comboBox->currentText()));
    //tab.setFilePath(currentOpenedFile);

    if (ui->cadb_radio->isChecked())
        tab.set("system", "cadb");
    else
        tab.set("system", "cogeron");

    if (ui->displayFingering_yes->isChecked())
        tab.set("displayFingering", "true");
    else
        tab.set("displayFingering", "false");

    return tab;
}

void MainWindow::terminal(QString str)
{
    ui->console->appendPlainText(str);
}

void MainWindow::save()
{
    if (currentOpenedFile.isEmpty())
    {
        save_as();
        return;
    }

    QFileInfo fi(currentOpenedFile);

    try
    {
        File::writedtb(currentOpenedFile, readMusicFromUI());
        documentIsSaved = true;
        setWindowTitle("Tabliato - " + fi.baseName());
        terminal("Sauvegarde de : " + currentOpenedFile);
    }
    catch (std::exception &e)
    {
        QMessageBox::critical(this, "Erreur", e.what());
    }
}

void MainWindow::save_as()
{
    QString filename = QFileDialog::getSaveFileName(this, tr("Enregistrer une tablature"), QDir::homePath(), tr("Tabliato Files (*.dtb)"));
    QFileInfo fi(filename);

    if(!filename.isEmpty())
    {
        if(fi.completeSuffix() != "dtb")
            filename.append(".dtb");

        currentOpenedFile = filename;
        save();

        setWindowTitle("Tabliato - " + fi.baseName());
    }
    else
    {
        terminal("Sauvegarde du fichier annulée\n");
    }
}

void MainWindow::open()
{
    QString path;

    if (currentOpenedFile != "")
        path = QFileInfo(currentOpenedFile).path();
    else
        path = QDir::homePath();

    QString filename = QFileDialog::getOpenFileName(this, tr("Ouvrir une tablature"), path, tr("Tabliato Files (*.dtb)"));
    open(filename);
}

void MainWindow::open(QString filename)
{
    if (filename.isEmpty()) return;

    QFile file(filename);
    QFileInfo fileinfo(filename);

    stopMusic();

    if(!file.exists()) return;

    try
    {
        Tabulature tab = File::readdtb(filename);
        updateUIFromMusic(tab);

        currentOpenedFile = filename;
        documentIsSaved = true;

        setWindowTitle("Tabliato - " + QFileInfo(currentOpenedFile).baseName());
        terminal("Chargement de : " + filename);

        QString pdf = fileinfo.path() + "/" + fileinfo.completeBaseName() + ".pdf";
        if (QFile(pdf).exists())
        {
            terminal("Chargement de : " + pdf);
            updatePreview(pdf);
        }

        QString audio = fileinfo.path() + "/" + fileinfo.completeBaseName() + AUDIOEXT;

        if (QFile(audio).exists())
        {
            terminal("Chargement de : " + audio);
            music->setMedia(QMediaContent());
            music->setMedia(QUrl::fromLocalFile(audio));
        }

        try
        {
            TabliatoProcessor proc(tab);
            m_timeline = proc.m_timeline;
        }
        catch(const std::exception &e)
        {
            QMessageBox::critical(this, "Erreur", QString(e.what()));
            terminal(e.what());
            return;
        }
    }
    catch(const std::exception &e)
    {
        QString err("Un problème est survenu. Le logiciel renvoie l'erreur suivante :\n ");
        err += QString(e.what());
        QMessageBox::critical(this, "Erreur", err);
        terminal(e.what());
    }
}

void MainWindow::openAbout()
{
     QMessageBox msgBox;
     QString html = File::read(HTML + "/about.html");
     html.replace("[version]", qApp->applicationVersion());
     msgBox.setText(html);
     msgBox.exec();
}

void MainWindow::checkVersion()
{
     VersionDialog *verDlg = new VersionDialog(this);
     verDlg->exec();
     delete verDlg;
}

void MainWindow::openNew()
{
    //ui->basse_textarea->clear();
    ui->melodie_textarea->clear();
    ui->title_LineEdit->clear();
    ui->subtitle_LineEdit->clear();
    ui->composer_LineEdit->clear();
    ui->poet_LineEdit->clear();
    ui->tagline_LineEdit->clear();
    ui->time_combobox->setCurrentIndex(0);
    ui->key_combobox->setCurrentIndex(0);
    ui->instrument_LineEdit->clear();

    ui->tempo_comboBox->setCurrentIndex(2);
    ui->tempo_spinBox->setValue(130);

    ui->cadb_radio->setChecked(true);
    ui->cogeron_radio->setChecked(false);

    currentOpenedFile.clear();

    this->setWindowTitle("Tabliato - Sans titre");
}

void MainWindow::setIcons()
{
    QIcon download(ICON + "/download.svg");
    QIcon check(ICON + "/check.svg");
    QIcon logo(ICON + "/tabliato.svg");

    if (!QFile::exists(SOUNDFONTS + "/Gaillard.sf2"))
        ui->actionGaillard->setIcon(download);
    else
        ui->actionGaillard->setIcon(check);

    if (!QFile::exists(SOUNDFONTS + "/Loffet.sf2"))
        ui->actionLoffet->setIcon(download);
    else
        ui->actionLoffet->setIcon(check);

    if (!QFile::exists(SOUNDFONTS + "/Serafini.sf2"))
       ui->actionSerafini->setIcon(download);
    else
       ui->actionSerafini->setIcon(check);

    if (!QFile::exists(SOUNDFONTS + "/SaltaBourroche_Light.sf2"))
        ui->actionSalta_Bourroche->setIcon(download);
    else
        ui->actionSalta_Bourroche->setIcon(check);

    if (!QFile::exists(SOUNDFONTS + "/BalloneBurini.sf2"))
        ui->actionBallone_Burini->setIcon(download);
    else
        ui->actionBallone_Burini->setIcon(check);

    this->setWindowIcon(logo);
}


void MainWindow::restoreCheckedDock()
{
    if(!ui->consoleDock->isHidden())
        ui->actionDisplayConsole->setChecked(true);
    if(!ui->toolBar->isHidden())
        ui->actionDisplayToolBar->setChecked(true);
    if(!ui->playerDock->isHidden())
        ui->actionDisplayPlayer->setChecked(true);
    if(!ui->previewDock->isHidden())
        ui->actionDisplayPreview->setChecked(true);
    if(!ui->formDock->isHidden())
        ui->actionDisplayInformations->setChecked(true);
    if(!ui->insertTextDock->isHidden())
        ui->actionDisplayInsertFast->setChecked(false);
}

void MainWindow::playMusic()
{
    if (music->state() == QMediaPlayer::PlayingState)
    {
        music->pause();
        timer->stop();
        QIcon play(ICON + "/play.svg");
        ui->play_pushButton->setIcon(play);
    }
    else
    {
        music->play();
        timer->start(50);
        QIcon pause(ICON + "/pause.svg");
        ui->play_pushButton->setIcon(pause);
    }
}

void MainWindow::updateMusic(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState)
    {
        seekMusic(0);
        timer->stop();
        QIcon play(ICON + "/play.svg");
        ui->play_pushButton->setIcon(play);
    }
}

void MainWindow::stopMusic()
{
    music->stop();
    timer->stop();
    QIcon play(ICON + "/play.svg");
    ui->play_pushButton->setIcon(play);
}

void MainWindow::seekMusic(qint64 time)
{
    QTime displayTime(0, (time / 60000) % 60, (time / 1000) % 60);
    qint64 duration = music->duration();
    float percentage = ((double)time/(double)duration)*100;
    ui->time_slider->setValue(percentage);
    ui->time_label->setText(displayTime.toString("mm:ss"));
}

void MainWindow::seekMusic()
{
    qint64 duration = music->duration();
    int percentage = ui->time_slider->value();
    qint64 time = duration*((float)percentage/100);
    music->setPosition(time);
}


void MainWindow::exportFiles()
{
    if (!currentOpenedFile.isEmpty())
    {
        QFileInfo fi(currentOpenedFile);

        QFile::remove(fi.path() + "/" + fi.baseName() + ".pdf");
        QFile::remove(fi.path() + "/" + fi.baseName() + ".png");
        QFile::remove(fi.path() + "/" + fi.baseName() + ".ly");
        QFile::remove(fi.path() + "/" + fi.baseName() + MIDIEXT);
        QFile::remove(fi.path() + "/" + fi.baseName() + AUDIOEXT);

        if (ui->actionExportPDF_tool->isChecked())
            QFile::copy(OUTPUT + "/output.pdf", fi.path() + "/" + fi.baseName() + ".pdf");

        if (ui->actionExportPNG_tool->isChecked())
            QFile::copy(OUTPUT + "/output.png", fi.path() + "/" + fi.baseName() + ".png");

        if (ui->actionExportMIDI_tool->isChecked())
            QFile::copy(OUTPUT + "/output" + MIDIEXT, fi.path() + "/" + fi.baseName() + MIDIEXT);

        if (ui->actionExportLY_tool->isChecked())
            QFile::copy(OUTPUT + "/output.ly", fi.path() + "/" + fi.baseName() + ".ly");

        if (ui->actionExportWAV_tool->isChecked())
            QFile::copy(OUTPUT + "/output" + AUDIOEXT, fi.path() + "/" + fi.baseName() + AUDIOEXT);
    }
}

void MainWindow::documentSavedTitleChange()
{
    if(documentIsSaved)
    {
        documentIsSaved = false;
        setWindowTitle("*" + windowTitle());
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  if (event->type() == QEvent::Resize && obj == ui->previewDock)
  {
    pdf->scale();
  }

  return QWidget::eventFilter(obj, event);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave())
    {
        writeSettings();
        QFile::remove(OUTPUT + "/output.png");
        event->accept();
    }
    else
        event->ignore();
}

bool MainWindow::maybeSave()
{
    if (!documentIsSaved)
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("Le document a été modifié.\n"
                        "Voulez-vous sauvegarder vos changements ?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
        {
            save();
            return true;
        }
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}

void MainWindow::displayDocks()
{
    if(ui->actionDisplayToolBar->isChecked() && ui->toolBar->isHidden())
        ui->toolBar->show();
    if(!ui->actionDisplayToolBar->isChecked() && !ui->toolBar->isHidden())
        ui->toolBar->hide();

    if(ui->actionDisplayPlayer->isChecked() && ui->playerDock->isHidden())
        ui->playerDock->show();
    if(!ui->actionDisplayPlayer->isChecked() && !ui->playerDock->isHidden())
        ui->playerDock->hide();

    if(ui->actionDisplayConsole->isChecked() && ui->consoleDock->isHidden())
        ui->consoleDock->show();
    if(!ui->actionDisplayConsole->isChecked() && !ui->consoleDock->isHidden())
        ui->consoleDock->hide();

    if(ui->actionDisplayInformations->isChecked() && ui->formDock->isHidden())
        ui->formDock->show();
    if(!ui->actionDisplayInformations->isChecked() && !ui->formDock->isHidden())
        ui->formDock->hide();

    if(ui->actionDisplayPreview->isChecked() && ui->previewDock->isHidden())
        ui->previewDock->show();
    if(!ui->actionDisplayPreview->isChecked() && !ui->previewDock->isHidden())
        ui->previewDock->hide();

    if(ui->actionDisplayInsertFast->isChecked() && ui->insertTextDock->isHidden())
        ui->insertTextDock->show();
    if(!ui->actionDisplayInsertFast->isChecked() && !ui->insertTextDock->isHidden())
        ui->insertTextDock->hide();
}

void MainWindow::writeSettings()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());

    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());

    settings.setValue("exportWAV", ui->actionExportWAV_tool->isChecked());
    settings.setValue("exportLY", ui->actionExportLY_tool->isChecked());
    settings.setValue("exportMIDI", ui->actionExportMIDI_tool->isChecked());
    settings.setValue("exportPDF", ui->actionExportPDF_tool->isChecked());
    settings.setValue("exportPNG",  ui->actionExportPNG_tool->isChecked());
    settings.setValue("sf2", ui->sf2_combobox->currentText());
    settings.setValue("lastOpenedFile", currentOpenedFile);
    settings.setValue("scalePDF", pdf->scale());
    settings.setValue("previewWidth", ui->previewDock->width());
    settings.setValue("editorWidth", ui->centralwidget->width());
    settings.setValue("v110", 0);
}

void MainWindow::readSettings()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());

    if (settings.contains("v110"))
    {
        terminal("Restoration des parametres de session");

        restoreGeometry(settings.value("geometry").toByteArray());
        restoreState(settings.value("windowState").toByteArray());

        ui->actionExportWAV_tool->setChecked(settings.value("exportWAV").toBool());
        ui->actionExportLY_tool->setChecked(settings.value("exportLY").toBool());
        ui->actionExportMIDI_tool->setChecked(settings.value("exportMIDI").toBool());
        ui->actionExportPDF_tool->setChecked(settings.value("exportPDF").toBool());
        ui->actionExportPNG_tool->setChecked(settings.value("exportPNG").toBool());

        int index = ui->sf2_combobox->findText(settings.value("sf2").toString());
        if (index != -1) ui->sf2_combobox->setCurrentIndex(index);

        ui->pdfZoomSlider->setValue(settings.value("scalePDF").toFloat()*100);
        pdf->setScale(settings.value("scalePDF").toFloat());

        ui->centralwidget->resize(settings.value("editorWidth").toInt(), ui->centralwidget->height());
        ui->previewDock->resize(settings.value("previewWidth").toInt(), ui->previewDock->height());

        restoreCheckedDock();

        QString lastFile = settings.value("lastOpenedFile").toString();
        terminal("Ouverture du dernier fichier: " + lastFile);
        open(lastFile);
    }
    else // it's the first time, register is not written yet
    {
        QFont font;
        font.setFamily("Courier");
        font.setStyleHint(QFont::Monospace);
        font.setFixedPitch(true);

        ui->melodie_textarea->setFont(font);

        ui->actionDisplayConsole->setChecked(false);
        ui->actionDisplayToolBar->setChecked(true);
        ui->actionDisplayPlayer->setChecked(true);
        ui->actionDisplayPreview->setChecked(true);
        ui->actionDisplayInformations->setChecked(true);
        ui->actionDisplayInsertFast->setChecked(false);
        displayDocks();

        ui->tempo_spinBox->setValue(130);
        ui->tempo_comboBox->setCurrentIndex(2);
        ui->time_combobox->setCurrentIndex(1);
        ui->rythm_comboBox->setCurrentIndex(4);

        pdf->setScale(0.5);

        tabifyDockWidget(ui->previewDock, ui->consoleDock);
        ui->previewDock->raise();
        ui->insertTextDock->close();

        documentIsSaved = true;
        currentOpenedFile = "";

        this->setWindowTitle("Tabliato - Sans titre");

        File::mkdir(OUTPUT);
        open(":/examples/ressources/exemples/Polka piquée.dtb");
        currentOpenedFile = "";
        terminal("Premiere utilisation de tabliato");
    }
}

void MainWindow::initRythmComboBx()
{
    rythmList.insert("B a (binaire)", Motif(2));
    rythmList.insert("B a B a ", Motif(22));
    rythmList.insert("B a B a B a", Motif(222));
    rythmList.insert("B a B a B a B a", Motif(2222));

    rythmList.insert("B a (ternaire)", Motif(1));
    rythmList.insert("B aB a", Motif(11));
    rythmList.insert("B aB aB a", Motif(111));

    rythmList.insert("B a a ", Motif(3));
    rythmList.insert("B a a B a a ", Motif(33));

    rythmList.insert("B a a B a ", Motif(32));
    rythmList.insert("B a B a a ", Motif(23));

    rythmList.insert("B a B a B a a", Motif(223));
    rythmList.insert("B a B a a B a ", Motif(232));
    rythmList.insert("B a a B a B a ", Motif(322));

    rythmList.insert("B a a B a a B a a B a", Motif(3332));
    rythmList.insert("B a a B a a B a B a a", Motif(3323));
    rythmList.insert("B a a B a B a a B a a", Motif(3233));
    rythmList.insert("B a B a a B a a B a a", Motif(2333));

    rythmList.insert("Manuel", Motif(-1));

    ui->rythm_comboBox->clear();
    ui->rythm_comboBox->addItems(rythmList.keys());
}

void MainWindow::initAccordionComboBox()
{
    // File are ressources but for keyboards users must be able to add
    // or remove keyboards manually by modifiying the file so we make
    // a physical copy in user's directory
    QDir directory(KEYBOARDS);
    if (!directory.exists())
    {
      QDir().mkpath(KEYBOARDS);
      QDir keyboard_dir(":/keyboards/ressources/keyboards/");
      QStringList csv = keyboard_dir.entryList(QStringList() << "*.csv", QDir::Files);
      for(auto f : csv)
      {
          QFileInfo info(f);
          QFile::copy(":/keyboards/ressources/keyboards/" + f,  KEYBOARDS + "/" + f);
          QFile::setPermissions(KEYBOARDS + "/" + f, QFileDevice::ReadOwner|QFileDevice::WriteOwner);
      }
    }

    QFile file(KEYBOARDS +  "/assemblages.csv");

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "Erreur", "Impossible de lire le fichier de configuration des claviers : " + KEYBOARDS + "/assemblages.csv");
        return;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    ui->accordion_comboBox->clear();
    accordionList.clear();

    while (!stream.atEnd())
    {
        QString line = stream.readLine();
        QStringList list = line.split(";");

        accordionList.insert(list[1], list[0]);
        accordionListReverted.insert(list[0], list[1]);
        ui->accordion_comboBox->addItem(list[1]);
    }

    file.close();

}

void MainWindow::initSf2ComboBox()
{
    // File are ressources but for soundfonts users must be able to add
    // or remove soundfonts manually by modifiying the folder so we make
    // a physical copy in user's directory
    QDir directory(SOUNDFONTS);
    if (!directory.exists())
    {
      QDir().mkpath(SOUNDFONTS);
      QFile::copy(":/soundfonts/ressources/soundfonts/Accordion.sf2",  SOUNDFONTS + "/Accordion.sf2");
    }

    QStringList sf2 = directory.entryList(QStringList() << "*.sf2", QDir::Files);

    ui->sf2_combobox->clear();
    for(auto f : sf2)
       ui->sf2_combobox->addItem(f);
}

void MainWindow::updateRythmComboBx()
{
    QStringList list = rythmList.keys();
    ui->rythm_comboBox->clear();

    for (int i = 0; i < list.length(); ++i)
    {
        Motif motif;
        motif = rythmList.value(list[i]);

        if(motif.isCompatible(ui->time_combobox->currentText()))
            ui->rythm_comboBox->addItem(list[i]);
    }

}

void MainWindow::download_soundfonts(QString name)
{
    QString url = "http://jmi.ovh/DiatonicTab/SoundFonts/" + name;
    QString dest = SOUNDFONTS + "/" + name;

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "", "Voulez vous télécharger le fichier " + name + " ?" , QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes)
    {
        terminal("Téléchargement de: " + name);

        try
        {
            FileDownloader::download(url, dest);
            initSf2ComboBox();
            setIcons();
            QMessageBox::information(this, "Information", "Téléchargement de " + name + " terminé");
        }
        catch(std::exception &e)
        {
            QMessageBox::critical(this, "Erreur", QString(e.what()));
            terminal(e.what());
        }
    }
}

void MainWindow::goto_line(int line)
{
    int offset1 = 269;
    int offset2 = 309;
    int nline = ui->melodie_textarea->document()->blockCount();
    int ln = (line - offset1 <= nline) ? line - offset1 : line - offset2 - nline + 1;
    QTextCursor cursor(ui->melodie_textarea->document()->findBlockByLineNumber(ln)); // ln-1 because line number starts from 0
    ui->melodie_textarea->setTextCursor(cursor);
}

void MainWindow::highlight_notes_from_current_line_in_pdf()
{
    int offset1 = 269;
    int offset2 = 309;
    int line = ui->melodie_textarea->textCursor().blockNumber();
    int nline = ui->melodie_textarea->document()->blockCount();
    int line1 = line + offset1;
    int line2 = line + offset2 + nline - 1;
    QVector<int> lines = {line1, line2};
    pdf->highlight_link_from_lines(lines);
}

void MainWindow::highlight_notes_from_current_music_time_in_pdf()
{
    float pos_in_second = (double)music->position()/1000;
    int pos = m_timeline.search_note_at(pos_in_second);

    if (pos != -1)
    {
        int offset2 = 307;
        int nline = ui->melodie_textarea->document()->blockCount();
        int offset = offset2 + nline - 1;
        pdf->highlight_note(pos, offset);
    }
}

/*void MainWindow::printKeyboard()
{
    try
    {
        QFile::remove(OUTPUT + "/output.png");

        Tabulature tab = readMusicFromUI();
        tab.lhand("\\printkeyboard");
        //tab.setFilePath(OUTPUT + "/output.dtb");

        File::mkdir(OUTPUT);

        QStringList arguments;
        arguments << "--png" << "--pdf" << "--output=" + OUTPUT;

        TabliatoProcessor processor(tab);
        processor.compile(arguments, true);

        updatePreview();
        midi2audio();
        exportFiles();
    }
    catch(QString &e)
    {
        QMessageBox::critical(this, "Erreur", e);
        terminal(e);
    }
    catch(std::exception &e)
    {
        QMessageBox::critical(this, "Erreur", e.what());
        terminal(e.what());
    }
}*/
