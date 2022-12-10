﻿#include <QApplication>
#include <QWidget>
#include <QDir>
#include <QDebug>
#include <QLocale>
#include <QStandardPaths>
#include <QMessageBox>
#include <QDesktopServices>

#include "global.h"
#include "mainwindow.h"
#include "file.h"
#include "filedownloader.h"

QCoreApplication* createApplication(int &argc, char *argv[])
{
    if (argc > 1){
        return new QCoreApplication(argc, argv);
    }
    return new QApplication(argc, argv);
}

int main(int argc, char *argv[])
{
    QScopedPointer<QCoreApplication> app(createApplication(argc, argv));
    QCoreApplication::setOrganizationName("tabliato");
    QCoreApplication::setApplicationName("tabliato");
    QCoreApplication::setApplicationVersion("1.3.1");

    APPDIR = QApplication::applicationDirPath();
    APPPATH = QApplication::applicationFilePath();
    LOCAL = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    TEMPLATE = ":/templates/ressources/templates";
    KEYBOARDS = LOCAL + "/keyboards";
    ICON = ":/icons/ressources/icons";
    HTML = ":/html/ressources/html";
    OUTPUT = LOCAL + "/output";
    SOUNDFONTS = LOCAL + "/soundfonts";

    #ifdef Q_OS_WINDOWS
    MIDIEXT = ".mid";
    AUDIOEXT = ".wav";
    #endif

    #ifdef Q_OS_LINUX
    MIDIEXT = ".midi";
    AUDIOEXT = ".ogg";
    #endif


    if (qobject_cast<QApplication *>(app.data()))
    {
        // Check if a new version is available before to start the program
        try
        {
            QString res = FileDownloader::download("https://raw.githubusercontent.com/Jean-Romain/tabliato/master/version");
            QString ans = QString(res);
            QVersionNumber new_version = QVersionNumber::fromString(ans);
            QVersionNumber current_version = QVersionNumber::fromString(qApp->applicationVersion());
            QString fileName(LOCAL + "/no-update");
            QFile skip_update(fileName);

            QMessageBox msgBox;
            msgBox.setText("Une nouvelle version de tabliato est disponible");
            msgBox.setInformativeText(QString("Voulez vous installer tabliato " + new_version.toString()));
            msgBox.setIcon(QMessageBox::Question);
            msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Ignore);
            msgBox.setDefaultButton(QMessageBox::Yes);
            msgBox.setButtonText(QMessageBox::Yes, "Oui");
            msgBox.setButtonText(QMessageBox::No, "Non");
            msgBox.setButtonText(QMessageBox::Ignore, "Plus tard");

            if (new_version > current_version)
            {
                // Si il n'y a pas de fichier c'est que l'utilisateur n'a jamais cliqué sur Non pour l'update
                // donc il on demande s'il veut mettre à jour
                bool suggest_update = false;
                if (!skip_update.exists())
                {
                    suggest_update = true;
                }
                // Si le fichier existe c'est que l'utilisateur a dit non pour l'update
                // mais quand même ca bloque pour l'éternité. Donc si c'est une version encore
                // plus grande que celle refusée par l'utilisateur on propose quand mêm
                else
                {
                    QVersionNumber skiped_version = QVersionNumber::fromString(File::read(fileName));
                    if (new_version > skiped_version)
                        suggest_update = true;
                }

                if (suggest_update)
                {
                    int ret = msgBox.exec();
                    if (ret == QMessageBox::Yes)
                    {
                        QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/download.html"));
                        skip_update.remove();
                        return 0;
                    }
                    else if (ret == QMessageBox::No)
                    {
                        File::write(fileName, new_version.toString());
                    }
                }
            }
        }
        catch (std::exception &e)
        {

        }

        // Start the program
        MainWindow fenetre;
        fenetre.show();

        return app->exec();
    }
    else
    {
        QCommandLineParser parser;
        parser.setApplicationDescription("Generate tabulature for accordion from FILE");
        parser.addHelpOption();
        parser.addVersionOption();

        parser.addPositionalArgument("FILE", "Tabliato .dtb file to parse. Can optionnaly be a midi file generated by tabliato");

        QCommandLineOption opt_verbose(QStringList() << "V" << "verbose", "Verbose mode");
        parser.addOption(opt_verbose);

        QCommandLineOption opt_all(QStringList() << "a" << "all", "Export all.");
        parser.addOption(opt_all);

        QCommandLineOption opt_pdf("pdf", "Export pdf.");
        parser.addOption(opt_pdf);

        QCommandLineOption opt_ly("ly", "Export ly.");
        parser.addOption(opt_ly);

        QCommandLineOption opt_png("png", "Export png.");
        parser.addOption(opt_png);

        QCommandLineOption opt_ogg("ogg", "Export ogg.");
        parser.addOption(opt_ogg);

        QCommandLineOption opt_midi("midi", "Export midi.");
        parser.addOption(opt_midi);

        QCommandLineOption opt_sf2("soundfont", "Soundfont to generate audio from midi (implies --ogg)", "file.sf2");
        parser.addOption(opt_sf2);

        QCommandLineOption opt_output(QStringList() << "o" << "output", "Write output in file or folder FILE. File extension is automatically added", "FILE");
        parser.addOption(opt_output);

        parser.process(*app);

        bool verbose = parser.isSet(opt_verbose);
        bool pdf = parser.isSet(opt_pdf);
        bool ly  = parser.isSet(opt_ly);
        bool png = parser.isSet(opt_png);
        bool ogg = parser.isSet(opt_ogg);
        bool midi = parser.isSet(opt_midi);
        bool all = parser.isSet(opt_all);
        bool sf2 = parser.isSet(opt_sf2);
        bool out = parser.isSet(opt_output);

        const QStringList args = parser.positionalArguments();

        const QString rundir = QDir::currentPath();

        const QFileInfo input(args.at(0));
        const QString ifile = input.canonicalFilePath();
        const QString ifolder = input.canonicalPath();
        const QString iformat = input.suffix();

        const QFileInfo output(parser.value(opt_output));
        const QString ofile = out ? output.baseName() : input.baseName();
        const QString ofolder = out ? output.canonicalPath() : ifolder;

        const QString soundfont = sf2 ? parser.value(opt_sf2) : SOUNDFONTS + "/Loffet.sf2";

        // If it is a midi file move jump to audio conversion
        if (iformat == "midi" || iformat == "mid")
        {
            ogg = true;
            goto audio;
        }

        if (all)
        {
            pdf = true;
            ly = true;
            png = true;
            midi = true;
            ogg = true;
        }

        if (!pdf && !png)
        {
            pdf = true;
        }

        // ---------------------------------------
        // Parse tabliato code into lilypond code
        // ---------------------------------------

        {
            Tabulature tab = File::readdtb(ifile);

            try
            {
                TabliatoProcessor engine(tab);
            }
            catch(std::exception &e)
            {
                qCritical() << "Erreur:" << e.what();
                exit(1);
            }

            // ---------------------------------------
            // Call lilypond to create music sheet
            // ---------------------------------------

            QProcess lilypond;

            QFileInfo path(ifile);
            QString filebasepath = path.absolutePath() + "/" + path.baseName();

            QFile::remove(filebasepath + ".midi");
            QFile::remove(filebasepath + ".mid");
            QFile::remove(filebasepath + ".pdf");
            QFile::remove(filebasepath + ".png");
            QFile::remove(filebasepath + ".ogg");
            QFile::remove(filebasepath + ".wav");

            File::write(filebasepath + ".ly", tab.lilypond);

            QStringList arguments;
            if (png) arguments.append("--png");
            if (pdf) arguments.append("--pdf");
            arguments.append(QString("--output=") + ofolder + "/" + ofile);
            arguments.append(filebasepath + ".ly");


            #ifdef Q_OS_WINDOWS
            QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\wow6432node\\LilyPond", QSettings::NativeFormat); // 32 bits : HKEY_LOCAL_MACHINE\\SOFTWARE\\LilyPond
            QString lilypath = settings.value("Install_Dir").toString();
            QString command = "\"" + lilypath + "\\usr\\bin\\lilypond.exe" + "\"";
            #endif

            #ifdef Q_OS_LINUX
            QString command = "lilypond";
            #endif

            if (verbose) qDebug() << command << " " << arguments;

            lilypond.start(command, arguments);
            lilypond.waitForFinished(5000);
            lilypond.readAllStandardOutput();

            if(!QFile(filebasepath + ".pdf").exists() && !QFile(filebasepath + ".png").exists())
            {
                //QTextStream(stderr) << lilypond.readAllStandardError() << endl;
                QTextStream(stderr) << QString::fromUtf8("Erreur inconnue: le document n'a pas été compilé. Code probablement incorrect.");
                return 1;
            }

            if(!ly) QFile::remove(filebasepath + ".ly");
            if(!midi)
            {
                QFile::remove(filebasepath + ".mid");
                QFile::remove(filebasepath + ".midi");
            }

        }

        // ---------------------------------------
        // Call timidity to create audio file
        // ---------------------------------------

        audio:

        if (!ogg) return 0;

        QFile::remove(ofolder + "/" + ofile + AUDIOEXT);

        QProcess timidity;

        #ifdef Q_OS_WINDOWS
        QString command = APPDIR + "\\TiMidity++\\timidity.exe";
        QString mode = "-Ow";
        #endif

        #ifdef Q_OS_LINUX
        QString command = "timidity";
        QString mode = "-Ov";
        #endif

        QStringList arguments;
        arguments.append(ifile);
        arguments.append(mode);
        arguments.append("--config-string=soundfont\\ " + soundfont);
        arguments.append("--output-file=" + ofolder + "/" + ofile + AUDIOEXT);

        timidity.start(command, arguments);
        timidity.waitForFinished(10000);
        timidity.readAllStandardOutput();

        return 0;
    }
}
