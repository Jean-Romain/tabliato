#ifndef FILEDOWNLOADER_H
#define FILEDOWNLOADER_H

#include <QObject>

class FileDownloader : public QObject
{
 Q_OBJECT
 public:
  FileDownloader();
  ~FileDownloader();
  static void download(QString url, QString dest);
};

#endif // FILEDOWNLOADER_H
