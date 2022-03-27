﻿#include <QApplication>
#include <QWidget>
#include <QDir>
#include <QDebug>
#include <QLocale>

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
    QCoreApplication::setApplicationVersion("1.0.1");

    APPDIR = QApplication::applicationDirPath();
    APPPATH = QApplication::applicationFilePath();
    SHARE = (APPDIR == "/usr/bin") ? "/usr/share/tabliato" : APPDIR + "/share";

    #ifdef Q_OS_WINDOWS
    LOCAL = SHARE;
    #endif

    #ifdef Q_OS_LINUX
    LOCAL = QDir::homePath() + "/.local/share/tabliato";
    #endif

    TEMPLATE = SHARE + "/templates";
    KEYBOARDS = SHARE + "/keyboards";
    SOUNDFONTS = LOCAL + "/soundfonts";
    ICON = SHARE + "/icon";
    DOC = SHARE + "/doc";
    HTML = SHARE + "/html";
    EXAMPLE = SHARE + "/exemples";

    if (qobject_cast<QApplication *>(app.data()))
    {
        QTemporaryDir tmpdir;
        TEMP = (APPDIR == "/usr/bin") ? tmpdir.path() : APPDIR;
        OUTPUT = TEMP + "/output";

        MainWindow fenetre;
        fenetre.show();

        return app->exec();
    }
    else
    {
        TEMP = APPDIR;

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
                QTextStream(stderr) << "Erreur inconnue: le document n'a pas été compilé. Code probablement incorrect." << endl;
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

        qDebug() << ogg;

        if (!ogg) return 0;

        QProcess timidity;

        #ifdef Q_OS_WINDOWS
        File::write(APPDIR + "\\TiMidity++\\Timidity.cfg", "soundfont \"" + soundfont + "\"");
        QString command = APPDIR + "\\TiMidity++\\timidity.exe";
        QString ext =  ".wav";
        QString mode = "-Ow";
        QFile::remove(ofolder + "/" + ofile + ext);
        #endif

        #ifdef Q_OS_LINUX
        QString command = "timidity";
        QString ext =  ".ogg";
        QString mode = "-Ov";
        #endif

        QStringList arguments;
        arguments.append(ifile);
        arguments.append(mode);
        #ifdef Q_OS_LINUX
        arguments.append("--config-string=soundfont\\ " + soundfont);
        #endif
        arguments.append("--output-file=" + ofolder + "/" + ofile + ext);

        timidity.start(command, arguments);
        timidity.waitForFinished(10000);
        timidity.readAllStandardOutput();

        return 0;
    }
}
