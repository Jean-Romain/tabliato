#include "versiondialog.h"
#include "global.h"

#include <QtCore/QUrl>
#include <QDesktopServices>
#include <QVersionNumber>

VersionDialog::VersionDialog(QWidget *parent):QDialog( parent)
{
    ui.setupUi(this);
    timer.setSingleShot(true);
    connect(&timer, SIGNAL(timeout()), this, SLOT(stopChecker()));
    ui.lineEditCurrent->setText(qApp->applicationVersion());
    ui.lineEditAvailable->setText(QString::fromUtf8("?.?.?"));
    connect(ui.pushButtonDownload, SIGNAL( clicked() ), this, SLOT( gotoDownloadPage() ) );
    connect(ui.pushButtonCheck, SIGNAL( clicked() ), this, SLOT( launchChecker() ) );
}

VersionDialog::~VersionDialog()
{
}

void VersionDialog::gotoDownloadPage()
{
    QVersionNumber new_version = QVersionNumber::fromString(ui.lineEditAvailable->text());
    QVersionNumber current_version = QVersionNumber::fromString(qApp->applicationVersion());

    // Same version
    if (new_version.majorVersion() == current_version.majorVersion() && new_version.minorVersion() == current_version.minorVersion())
    {
        // Seulement des bug fix
        if (new_version.microVersion() > current_version.microVersion())
        {
            // Télécharger uniquement tabliato.exe sous windows sachant qu'il n'y a pas de nouvelles dll ou icones.
            #ifdef Q_OS_WINDOWS
            QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/download.html"));
            #endif

            // Sous linux on renvoit toujours à la page de téléchargement
            #ifdef Q_OS_LINUX
            QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/download.html"));
            #endif

            return;
        }
    }

    QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/download.html"));

}

void VersionDialog::launchChecker()
{
    ui.pushButtonCheck->setEnabled(false);
    timer.start(10000);
    reply = manager.get(QNetworkRequest(QUrl("https://raw.githubusercontent.com/Jean-Romain/tabliato/master/version")));
    QObject::connect (reply, SIGNAL (finished()),this, SLOT(showResultChecker()));
}

void VersionDialog::showResultChecker()
{
    timer.stop();

    if (reply->error())
    {
        ui.lineEditAvailable->setText(tr("Erreur"));
    }
    else
    {
        QString ans = reply->readAll();
        QVersionNumber new_version = QVersionNumber::fromString(ans);
        QVersionNumber current_version = QVersionNumber::fromString(qApp->applicationVersion());
        ui.lineEditAvailable->setText(ans);

        // Same version
        if (new_version.majorVersion() == current_version.majorVersion() && new_version.minorVersion() == current_version.minorVersion())
        {
            // Seulement des bug fix
            if (new_version.microVersion() > current_version.microVersion())
            {
                ui.pushButtonCheck->setText("Mettre à jour");
            }
        }

        ui.pushButtonCheck->setEnabled(true);

        if (new_version > current_version)
            ui.pushButtonDownload->setEnabled(true);
    }
}

void VersionDialog::stopChecker()
{
    ui.lineEditAvailable->setText(tr("Erreur"));

    QObject::disconnect (reply, SIGNAL (finished()),this, SLOT(showResultChecker()));
    reply->abort();
    ui.pushButtonCheck->setEnabled(true);
}
