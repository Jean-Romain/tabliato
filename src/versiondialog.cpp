#include "versiondialog.h"
#include "global.h"
#include "filedownloader.h"

#include <QtCore/QUrl>
#include <QDesktopServices>
#include <QVersionNumber>
#include <QMessageBox>

VersionDialog::VersionDialog(QWidget *parent):QDialog( parent)
{
    ui.setupUi(this);
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
    QDesktopServices::openUrl(QUrl("https://jean-romain.github.io/tabliato/download.html"));
}

void VersionDialog::launchChecker()
{
    try
    {
        QString res = FileDownloader::download("https://raw.githubusercontent.com/Jean-Romain/tabliato/master/version");
        QString ans = QString(res);
        QVersionNumber new_version = QVersionNumber::fromString(ans);
        QVersionNumber current_version = QVersionNumber::fromString(qApp->applicationVersion());
        ui.lineEditAvailable->setText(ans);

        // Same version
        if (new_version.majorVersion() == current_version.majorVersion() && new_version.minorVersion() == current_version.minorVersion())
        {
            // Seulement des bug fix
            if (new_version.microVersion() > current_version.microVersion())
            {
                ui.pushButtonDownload->setText("Mettre à jour");
            }
        }

        ui.pushButtonCheck->setEnabled(true);

        if (new_version > current_version)
            ui.pushButtonDownload->setEnabled(true);
    }
    catch(std::exception &e)
    {
        QMessageBox::critical(this, "Erreur", "Pas de réponse du serveur");
    }
}
