#include <QCoreApplication>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QSize>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QSignalMapper>

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
    signalMapper = new QSignalMapper(this) ;

    connect(ui->actionSave, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionSaveAs, SIGNAL(triggered()), this, SLOT(save_as()));
    connect(ui->actionOpen, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionNew, SIGNAL(triggered()), this, SLOT(openNew()));
    connect(ui->actionOpenExample, SIGNAL(triggered()), this, SLOT(open_example()));
    connect(ui->actionCompile, SIGNAL(triggered()), this, SLOT(compile()));

    connect(ui->actionSave_tool, SIGNAL(triggered()), this, SLOT(save()));
    connect(ui->actionOpen_tool, SIGNAL(triggered()), this, SLOT(open()));
    connect(ui->actionNew_tool, SIGNAL(triggered()), this, SLOT(openNew()));
    connect(ui->actionUndo_tool, SIGNAL(triggered()), ui->melodie_textarea, SLOT(undo()));
    connect(ui->actionRedo_tool, SIGNAL(triggered()), ui->melodie_textarea, SLOT(redo()));
    connect(ui->actionCompile_tool, SIGNAL(triggered()), this, SLOT(compile()));

    connect(ui->actionHelpTabliato, SIGNAL(triggered()), this, SLOT(openHelpTabliato()));
    connect(ui->actionHelpSyntax, SIGNAL(triggered()), this, SLOT(openHelpSyntax()));
    connect(ui->actionHelpLilypond, SIGNAL(triggered()), this, SLOT(openHelpLilypond()));
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(openAbout()));
    connect(ui->actionCheckUpdate, SIGNAL(triggered()), this, SLOT(checkVersion()));
    connect(ui->actionBug, SIGNAL(triggered()), this, SLOT(openContactWebPage()));
    connect(ui->actionSuggesstions, SIGNAL(triggered()), this, SLOT(openContactWebPage()));

    connect(ui->actionDisplayToolBar, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayPlayer, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayInformations, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayConsole, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayInsertFast, SIGNAL(triggered()), this, SLOT(displayDocks()));
    connect(ui->actionDisplayPreview, SIGNAL(triggered()), this, SLOT(displayDocks()));

    connect(ui->melodie_textarea, SIGNAL(textChanged()), this, SLOT(documentSavedTitleChange()));
    //connect(ui->basse_textarea, SIGNAL(textChanged()), this, SLOT(documentSavedTitleChange()));

    connect(ui->actionInsertRepeat, SIGNAL(triggered()), this, SLOT(insertRepeat()));
    connect(ui->actionInsertBreak, SIGNAL(triggered()), this, SLOT(insertBreak()));
    connect(ui->actionInsertAlternative, SIGNAL(triggered()), this, SLOT(insertAlternative()));
    connect(ui->actionInsertEndBar, SIGNAL(triggered()), this, SLOT(insertEndBar()));

    //connect(ui->infoButton_accordion, SIGNAL(clicked()), this, SLOT(printKeyboard()));

    connect(ui->endbar, SIGNAL(clicked()), this, SLOT(insertEndBar()));
    connect(ui->beginbar, SIGNAL(clicked()), this, SLOT(insertBeginBar()));
    connect(ui->beginrepaeat, SIGNAL(clicked()), this, SLOT(insertEndRepeatBar()));
    connect(ui->endrepeat, SIGNAL(clicked()), this, SLOT(insertEndRepeatBar()));
    connect(ui->rest1, SIGNAL(clicked()), this, SLOT(insertRest1()));
    connect(ui->rest2, SIGNAL(clicked()), this, SLOT(insertRest2()));
    connect(ui->rest4, SIGNAL(clicked()), this, SLOT(insertRest4()));
    connect(ui->rest8, SIGNAL(clicked()), this, SLOT(insertRest8()));
    connect(ui->rest16, SIGNAL(clicked()), this, SLOT(insertRest16()));
    connect(ui->rest32, SIGNAL(clicked()), this, SLOT(insertRest32()));
    connect(ui->rest64, SIGNAL(clicked()), this, SLOT(insertRest64()));
    connect(ui->rest128, SIGNAL(clicked()), this, SLOT(insertRest128()));
    connect(ui->tie, SIGNAL(clicked()), this, SLOT(insertTie()));
    connect(ui->slurs, SIGNAL(clicked()), this, SLOT(insertSlurs()));

    connect(ui->play_pushButton, SIGNAL(clicked()), this, SLOT(playMusic()));
    connect(ui->stop_pushButton, SIGNAL(clicked()), this, SLOT(stopMusic()));
    connect(music, SIGNAL(positionChanged(qint64)), this, SLOT(seekMusic(qint64)));
    connect(ui->time_slider, SIGNAL(sliderReleased()), this, SLOT(seekMusic()));
    //connect(ui->time_slider, SIGNAL(sliderPressed()), this, SLOT(seekMusic()));
    connect(ui->showsf2ddl_pushButton, SIGNAL(clicked()), this, SLOT(openSF2About()));

    connect(midi2audioCall,SIGNAL(readyReadStandardOutput()),this,SLOT(midi2audioReadyRead()));
    connect(midi2audioCall,SIGNAL(finished(int)),this,SLOT(midi2audioFinished(int)));

    connect(ui->pageSpinBox, SIGNAL(valueChanged(int)), pdf, SLOT(setPage(int)));
    connect(pdf, SIGNAL(pageChanged(int)), ui->pageSpinBox, SLOT(setValue(int)));
    connect(ui->pdfZoomSlider, SIGNAL(valueChanged(int)), this, SLOT(scaleDocument(int)));
    connect(pdf, SIGNAL(scaleChanged(int)), ui->pdfZoomSlider, SLOT(setValue(int)));

    signalMapper->setMapping(ui->actionBallone_Burini, "BalloneBurini.sf2") ;
    signalMapper->setMapping(ui->actionLoffet, "Loffet.sf2") ;
    signalMapper->setMapping(ui->actionGaillard, "Gaillard.sf2") ;
    signalMapper->setMapping(ui->actionSalta_Bourroche, "SaltaBourroche.sf2") ;
    signalMapper->setMapping(ui->actionSerafini, "Serafini.sf2") ;

    connect(ui->actionBallone_Burini, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
    connect(ui->actionLoffet, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
    connect(ui->actionGaillard, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
    connect(ui->actionSalta_Bourroche, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
    connect(ui->actionSerafini, SIGNAL(triggered()), signalMapper, SLOT(map())) ;
    connect(signalMapper, SIGNAL(mapped(QString)), this, SLOT(download_soundfonts(QString))) ;

    connect(ui->time_combobox,SIGNAL(currentIndexChanged(int)),this,SLOT(updateRythmComboBx()));

    // Fix #19 window only. This works natively on linux
    #ifdef Q_OS_WINDOWS
    ui->time_combobox->setStyleSheet("QComboBox QAbstractItemView {min-width: 70;}");
    ui->key_combobox->setStyleSheet("QComboBox QAbstractItemView {min-width: 100;}");
    ui->accordion_comboBox->setStyleSheet("QComboBox QAbstractItemView {min-width: 300;}");
    #endif

    //ui->player_layout->addWidget(slider);
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
    delete signalMapper;
}

void MainWindow::compile()
{
    if (!currentOpenedFile.isEmpty()) save();

    terminal("Compilation");

    Tabulature tab = readMusicFromUI();
    QString dtb = OUTPUT + "/output.dtb";

    File::mkdir(OUTPUT);
    File::writedtb(dtb, tab);

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

    terminal("Compilation terminée");
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
    int page = ui->pageSpinBox->value();

    if (pdf->setDocument(path))
    {
        ui->pageSpinBox->setMinimum(1);
        ui->pageSpinBox->setMaximum(pdf->document()->numPages());
        ui->pdfZoomSlider->setEnabled(true);
        ui->pageSpinBox->setEnabled(true);
        scaleDocument(ui->pdfZoomSlider->value());
        ui->pageSpinBox->setValue(page);
    }
    else
    {
        QMessageBox::warning(this, "PDF Viewer - Failed to Open File", "The specified file could not be opened.");
    }
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

    if(tab.get("displayFingering") == "auto")
        ui->displayFingering_auto->setChecked(true);
    else if(tab.get("displayFingering") == "true")
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

    if(ui->cadb_radio->isChecked())
        tab.set("system", "cadb");
    else
        tab.set("system", "cogeron");

    if(ui->displayFingering_auto->isChecked())
        tab.set("displayFingering", "auto");
    else if(ui->displayFingering_yes->isChecked())
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
        save_as();
    else
    {
        QFileInfo fi(currentOpenedFile);
        File::writedtb(currentOpenedFile, readMusicFromUI());

        documentIsSaved = true;
        setWindowTitle("Tabliato - " + fi.baseName());

        terminal("Sauvegarde de : " + currentOpenedFile);
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

void MainWindow::open_example()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Ouvrir une tablature"), EXAMPLE, tr("Tabliato Files (*.dtb)"));
    open(filename);
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
    if (filename.isEmpty())return;

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
     msgBox.setText(File::read(HTML + "/about.html"));
     msgBox.exec();
}

void MainWindow::openSF2About()
{
    QMessageBox::information(this, "Soundfonts", "Pour télécharger d'autres fichiers de rendu audio allez dans: Audio > Installer des fontes sonores");
}

void MainWindow::checkVersion()
{
     VersionDialog *verDlg = new VersionDialog(this);
     verDlg->exec();
     delete verDlg;
}

void MainWindow::openHelpLilypond(){ QDesktopServices::openUrl(QUrl("http://www.lilypond.org/introduction.fr.html"));}
void MainWindow::openHelpTabliato(){ QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/doc.html"));}
void MainWindow::openHelpSyntax(){ QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/doc.html"));}
void MainWindow::openContactWebPage(){ QDesktopServices::openUrl(QUrl("https://github.com/Jean-Romain/tabliato/issues"));}

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
    // Actions icons

    QIcon save(ICON + "/save.svg");
    QIcon file(ICON + "/new.svg");
    QIcon open(ICON + "/open.svg");
    QIcon compile(ICON + "/compile.svg");

    QIcon undo(ICON + "/undo.svg");
    QIcon redo(ICON + "/redo.svg");

    QIcon repeat(ICON + "/repeat.svg");
    QIcon question(ICON + "/question.svg");
    QIcon information(ICON + "/information.svg");
    QIcon update(ICON + "/update.svg");
    QIcon bug(ICON + "/bug.svg");
    QIcon request(ICON + "/request.svg");

    QIcon play(ICON + "/play.svg");
    QIcon stop(ICON + "/stop.svg");
    QIcon more(ICON + "/more.svg");

    QIcon download(ICON + "/download.svg");
    QIcon check(ICON + "/check.svg");

    // Files icons

    QIcon pdf(ICON + "/pdf.svg");
    QIcon midi(ICON + "/midi.svg");
    QIcon png(ICON + "/png.svg");
    QIcon ly(ICON + "/ly.svg");
    QIcon wav(ICON + "/wav.svg");

    // Solfège icons

    QIcon croche(ICON + "/croche.svg");
    QIcon crochedot(ICON + "/crochedot.svg");
    QIcon noire(ICON + "/noire.svg");
    QIcon blanche(ICON + "/blanche.svg");
    QIcon noiredot(ICON + "/noire-dot.svg");
    QIcon blanchedot(ICON + "/blanche-dot.svg");
    QIcon ronde(ICON + "/ronde.svg");

    QIcon rest1(ICON + "/rest1.svg");
    QIcon rest2(ICON + "/rest2.svg");
    QIcon soupir(ICON + "/rest4.svg");
    QIcon rest8(ICON + "/rest8.svg");
    QIcon rest16(ICON + "/rest16.svg");
    QIcon rest32(ICON + "/rest32.svg");
    QIcon rest64(ICON + "/rest64.svg");
    QIcon rest128(ICON + "/rest128.svg");

    QIcon beginbar(ICON + "/beginbar.svg");
    QIcon endbar(ICON + "/endbar.svg");
    QIcon beginrepeat(ICON + "/beginrepeat.svg");
    QIcon endrepeat(ICON + "/endrepeat.svg");

    QIcon tie(ICON + "/tie.svg");
    QIcon slurs(ICON + "/slurs.svg");

    QIcon amajor(ICON + "/A-major.svg");
    QIcon bmajor(ICON + "/B-major.svg");
    QIcon cmajor(ICON + "/C-major.svg");
    QIcon cismajor(ICON + "/C-sharp-major.svg");
    QIcon dmajor(ICON + "/D-major.svg");
    QIcon emajor(ICON + "/E-major.svg");
    QIcon fismajor(ICON + "/F-sharp-major.svg");
    QIcon gmajor(ICON + "/G-major.svg");

    QIcon logo(ICON + "/tabliato.svg");

    ui->actionNew_tool->setIcon(file);
    ui->actionOpen_tool->setIcon(open);
    ui->actionSave_tool->setIcon(save);
    ui->actionUndo_tool->setIcon(undo);
    ui->actionRedo_tool->setIcon(redo);

    ui->actionCompile_tool->setIcon(compile);
    ui->actionExportPDF_tool->setIcon(pdf);
    ui->actionExportPNG_tool->setIcon(png);
    ui->actionExportMIDI_tool->setIcon(midi);
    ui->actionExportLY_tool->setIcon(ly);
    ui->actionExportWAV_tool->setIcon(wav);

    ui->actionNew->setIcon(file);
    ui->actionOpen->setIcon(open);
    ui->actionSave->setIcon(save);

    ui->actionInsertRepeat->setIcon(repeat);
    ui->actionHelpSyntax->setIcon(question);
    ui->actionHelpTabliato->setIcon(question);
    ui->actionHelpLilypond->setIcon(question);
    ui->actionAbout->setIcon(information);
    ui->actionCheckUpdate->setIcon(update);
    ui->actionBug->setIcon(bug);
    ui->actionSuggesstions->setIcon(request);

    ui->actionCompile->setIcon(compile);

    ui->tempo_comboBox->setItemIcon(0, ronde);
    ui->tempo_comboBox->setItemIcon(1, blanche);
    ui->tempo_comboBox->setItemIcon(2, noire);
    ui->tempo_comboBox->setItemIcon(3, noiredot);
    ui->tempo_comboBox->setItemIcon(4, croche);
    ui->tempo_comboBox->setItemIcon(5, crochedot);

    ui->key_combobox->setItemIcon(0, cmajor);
    ui->key_combobox->setItemIcon(1, gmajor);
    ui->key_combobox->setItemIcon(2, dmajor);
    ui->key_combobox->setItemIcon(3, amajor);
    ui->key_combobox->setItemIcon(4, emajor);
    ui->key_combobox->setItemIcon(5, bmajor);
    ui->key_combobox->setItemIcon(6, fismajor);
    ui->key_combobox->setItemIcon(7, cismajor);

    ui->rest1->setIcon(rest1);
    ui->rest2->setIcon(rest2);
    ui->rest4->setIcon(soupir);
    ui->rest8->setIcon(rest8);
    ui->rest16->setIcon(rest16);
    ui->rest32->setIcon(rest32);
    ui->rest64->setIcon(rest64);
    ui->rest128->setIcon(rest128);

    ui->tie->setIcon(tie);
    ui->slurs->setIcon(slurs);

    ui->endbar->setIcon(endbar);
    ui->beginbar->setIcon(beginbar);
    ui->endrepeat->setIcon(endrepeat);
    ui->beginrepaeat->setIcon(beginrepeat);

    ui->play_pushButton->setIcon(play);
    ui->stop_pushButton->setIcon(stop);

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

    if (!QFile::exists(SOUNDFONTS + "/SaltaBourroche.sf2"))
        ui->actionSalta_Bourroche->setIcon(download);
    else
        ui->actionSalta_Bourroche->setIcon(check);

    if (!QFile::exists(SOUNDFONTS + "/BalloneBurini.sf2"))
        ui->actionBallone_Burini->setIcon(download);
    else
        ui->actionBallone_Burini->setIcon(check);

    ui->showsf2ddl_pushButton->setIcon(more);

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

        QIcon play(ICON + "/play.svg");
        ui->play_pushButton->setIcon(play);
    }
    else
    {
        music->play();
        QIcon pause(ICON + "/pause.svg");
        ui->play_pushButton->setIcon(pause);
    }
}

void MainWindow::stopMusic()
{
    music->stop();

    QIcon play(ICON + "/play.svg");
    ui->play_pushButton->setIcon(play);
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

void MainWindow::insertText(int text)
{
    QPlainTextEdit *textarea;

    //if(ui->basse_textarea->hasFocus())
    //    textarea = ui->basse_textarea;
    //else
        textarea = ui->melodie_textarea;

    switch (text)
    {
    case REPEAT:
        textarea->insertPlainText("\\repeat volta 2\n{\n\n}\n");
        break;
    case ALTERNATIVE:
        textarea->insertPlainText("\\alternative\n{\n  {  }\n  {  }\n}");
        break;
    case ENDBAR:
        textarea->insertPlainText("\\bar \"|.\" ");
        break;
    case BEGINBAR:
        textarea->insertPlainText("\\bar \".|\" ");
        break;
    case ENDREPEATBAR:
        textarea->insertPlainText("\\bar \":|.\" ");
        break;
    case BEGINREPEATBAR:
        textarea->insertPlainText("\\bar \".|:\" ");
        break;
    case BREAK:
        textarea->insertPlainText("\\break");
        break;
    case REST:
        textarea->insertPlainText("r:");
        break;
    case REST1:
        textarea->insertPlainText("r1 ");
        break;
    case REST2:
        textarea->insertPlainText("r2 ");
        break;
    case REST4:
        textarea->insertPlainText("r4 ");
        break;
    case REST8:
        textarea->insertPlainText("r8 ");
        break;
    case REST16:
        textarea->insertPlainText("r16 ");
        break;
    case REST32:
        textarea->insertPlainText("r32 ");
        break;
    case REST64:
        textarea->insertPlainText("r64 ");
        break;
    case REST128:
        textarea->insertPlainText("r128 ");
        break;
    case TIE:
        textarea->insertPlainText("~ ");
        break;
    case SLURS:
        textarea->insertPlainText("( ) ");
        break;
    default:
        break;
    }
}

void MainWindow::insertRepeat(){insertText(REPEAT);}
void MainWindow::insertBreak(){insertText(BREAK);}
void MainWindow::insertAlternative(){insertText(ALTERNATIVE);}

void MainWindow::insertRest(){insertText(REST);}
void MainWindow::insertRest1(){insertText(REST1);}
void MainWindow::insertRest2(){insertText(REST2);}
void MainWindow::insertRest4(){insertText(REST4);}
void MainWindow::insertRest8(){insertText(REST8);}
void MainWindow::insertRest16(){insertText(REST16);}
void MainWindow::insertRest32(){insertText(REST32);}
void MainWindow::insertRest64(){insertText(REST64);}
void MainWindow::insertRest128(){insertText(REST128);}

void MainWindow::insertBeginBar(){insertText(BEGINBAR);}
void MainWindow::insertEndBar(){insertText(ENDBAR);}
void MainWindow::insertBeginRepeatBar(){insertText(BEGINREPEATBAR);}
void MainWindow::insertEndRepeatBar(){insertText(ENDREPEATBAR);}

void MainWindow::insertTie(){insertText(TIE);}
void MainWindow::insertSlurs(){insertText(SLURS);}

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
}

void MainWindow::readSettings()
{
    QSettings settings(qApp->organizationName(), qApp->applicationName());

    if (settings.contains("geometry"))
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

        ui->tempo_spinBox->setValue(130);

        ui->actionDisplayConsole->setChecked(true);
        ui->actionDisplayToolBar->setChecked(true);
        ui->actionDisplayPlayer->setChecked(true);
        ui->actionDisplayPreview->setChecked(true);
        ui->actionDisplayInformations->setChecked(true);
        ui->actionDisplayInsertFast->setChecked(false);
        displayDocks();

        tabifyDockWidget(ui->previewDock, ui->consoleDock);
        ui->previewDock->raise();
        ui->insertTextDock->close();

        documentIsSaved = true;
        currentOpenedFile = "";

        this->setWindowTitle("Tabliato - Sans titre");

        File::mkdir(OUTPUT);

        open(EXAMPLE + "/exemple.dtb");

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
    QFile file(KEYBOARDS +  "/assemblages.csv");
    ui->accordion_comboBox->clear();
    accordionList.clear();

    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
        QMessageBox::critical(this, "Erreur", "Impossible de lire le fichier de configuration des claviers : " + KEYBOARDS + "/assemblages.csv");
    else
    {
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

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
}

void MainWindow::initSf2ComboBox()
{
    QDir directory(SOUNDFONTS);
    if (!directory.exists())
    {
      QDir().mkpath(SOUNDFONTS);
      QFile::copy(SHARE + "/soundfonts/Accordion.sf2",  SOUNDFONTS + "/Accordion.sf2");
    }

    QStringList sf2 = directory.entryList(QStringList() << "*.sf2", QDir::Files);

    ui->sf2_combobox->clear();
    foreach(QString f, sf2)
    {
       ui->sf2_combobox->addItem(f);
    }
}

void MainWindow::updateRythmComboBx()
{
    QStringList list = rythmList.keys();
    ui->rythm_comboBox->clear();

    for (int i = 0; i < list.length(); ++i)
    {
        Motif motif;
        motif = rythmList.value(list[i]);

        //qDebug() << list[i];

        if(motif.isCompatible(ui->time_combobox->currentText()))
            ui->rythm_comboBox->addItem(list[i]);
    }

}

void MainWindow::scaleDocument(int zoom)
{
    pdf->setScale((double) zoom/100);
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

void MainWindow::download_soundfonts(QString name)
{
    QString url = "https://media.githubusercontent.com/media/Jean-Romain/tabliato/master/soundfonts/" + name;
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
