#ifndef VERSIONDIALOG_H
#define VERSIONDIALOG_H

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QTimer>

#include "ui_versiondialog.h"

class VersionDialog : public QDialog
{
   Q_OBJECT

    public:
        VersionDialog(QWidget *parent=0);
        ~VersionDialog();
        Ui::VersionDialog ui;
    private slots:
      void gotoDownloadPage();
      void launchChecker();
};


#endif
