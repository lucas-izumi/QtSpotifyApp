#include "downloader.h"
#include <QEventLoop>

Downloader::Downloader(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager;
}

Downloader::~Downloader()
{
    manager->deleteLater();
}

void Downloader::startDownload(QString filename, QString url)
{
    QNetworkRequest request;
    QUrl requestURL (url);
    request.setUrl(requestURL);
    reply = manager->get(request);
    file = new QFile(filename + ".mp3");

    if (!file->open(QIODevice::WriteOnly))
    {
        qDebug() << "Erro ao criar arquivo " + filename + ".mp3";
        return;
    }

    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
                this, SLOT(error(QNetworkReply::NetworkError)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
                this, SLOT(updateProgress(qint64, qint64)));
    connect(reply, SIGNAL(finished()),
                this, SLOT(finished()));

    // Necessario para manter o objeto vivo ate que o download seja concluido
    QEventLoop eventLoop; connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit())); eventLoop.exec();
}

void Downloader::error(QNetworkReply::NetworkError err)
{
    // Para verificar os codigos de erro, consultar:
    // https://doc.qt.io/qt-5/qnetworkreply.html
    qDebug() << "Erro durante o download. Cod: " << err;
    reply->deleteLater();
}

void Downloader::updateProgress(qint64 read, qint64 total)
{
    qDebug() << "Baixando";
    // Pega o stream baixado ate o momento e ja salva no arquivo para desocupar a memoria
    QByteArray b = reply->readAll();
    file->write(b);
    // Adicionar barra de progresso no layout usando o read e o total emitidos pelo downloadProgress()
}

void Downloader::finished()
{
    file->close();
    reply->deleteLater();
}
