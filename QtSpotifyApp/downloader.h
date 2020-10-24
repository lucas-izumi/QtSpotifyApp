#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#include <QFile>
#include <QObject>
#include <QNetworkReply>

class Downloader : public QObject
{
    Q_OBJECT
public:
    explicit Downloader(QObject *parent = 0);
    virtual ~Downloader();
    void startDownload(QString filename, QString url);

private:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;
    QFile *file;

private slots:
    void updateProgress(qint64 read, qint64 total);
    void error(QNetworkReply::NetworkError err);
    void finished();
};

#endif // DOWNLOADER_H
