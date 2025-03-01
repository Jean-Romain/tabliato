#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>
#include <QByteArray>

class FileDownloader : public QObject
{
 Q_OBJECT
 public:
  FileDownloader();
  ~FileDownloader();
  static void download(QString url, QString dest);
  static QByteArray download(QString url);
};

#endif // FILEDOWNLOADER_H
