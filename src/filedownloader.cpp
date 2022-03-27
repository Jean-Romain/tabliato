#include <QEventLoop>
#include <QSaveFile>
#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include "filedownloader.h"

FileDownloader::FileDownloader()
{

}

FileDownloader::~FileDownloader()
{

}

void FileDownloader::download(QString url, QString dest)
{
    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop event;
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();
    QByteArray content = response->readAll();

    if (content.isEmpty())
        throw std::logic_error((QString("Impossible de télécharger l'url : ") + url).toStdString());

    QSaveFile file(dest);
    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.commit();
    return;
}
