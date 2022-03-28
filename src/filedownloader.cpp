#include <QEventLoop>
#include <QSaveFile>
#include <QObject>
#include <QSslSocket>
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
    QByteArray content = download(url);

    QSaveFile file(dest);
    file.open(QIODevice::WriteOnly);
    file.write(content);
    file.commit();
    return;
}

QByteArray FileDownloader::download(QString url)
{
    //if (QSslSocket::supportsSsl())

    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(QUrl(url)));
    QEventLoop event;
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();

    if (response->error() != QNetworkReply::NoError)
        throw std::logic_error(response->errorString().toStdString());

    QByteArray content = response->readAll();

    if (content.isEmpty())
        throw std::logic_error((QString("Impossible de télécharger l'url : ") + url).toStdString());

    return content;
}
