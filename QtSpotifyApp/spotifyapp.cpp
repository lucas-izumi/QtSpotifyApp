#include "spotifyapp.h"
#include "ui_spotifyapp.h"
#include "config.h"
#include "macros.h"
#include "downloader.h"
#include <QDesktopServices>
#include <QtNetworkAuth>
#include <QUrl>

bool fileExists(QString path)
{
    QFileInfo check_file(path);
    return (check_file.exists() && check_file.isFile()) ? true : false;
}

SpotifyApp::SpotifyApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SpotifyApp)
{
    ui->setupUi(this);
    mediaPlayer = new QMediaPlayer;
    playlist = new QMediaPlaylist;

    connect(mediaPlayer, SIGNAL(currentMediaChanged(const QMediaContent)), this, SLOT(currentMediaChanged(const QMediaContent)));

    readConfig();
    setupConnection();
    connectToSpotify();
}

SpotifyApp::~SpotifyApp()
{
    delete ui;
}

void SpotifyApp::setupConnection()
{
    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, this);
    spotify.setReplyHandler(replyHandler);
    spotify.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    spotify.setClientIdentifier(ClientID);
    spotify.setClientIdentifierSharedKey(ClientIDSharedKey);
    spotify.setScope("user-read-private user-top-read playlist-read-private playlist-modify-public playlist-modify-private");
}

void SpotifyApp::granted()
{
    qDebug() << "Signal granted received";
    QString token = spotify.token();
    qDebug() << "Token: " + token;
    isGranted = true;
}

void SpotifyApp::authStatusChanged(QAbstractOAuth::Status status)
{
    QString s;
    if (status == QAbstractOAuth::Status::Granted)
        s = "Granted";

    if (status == QAbstractOAuth::Status::TemporaryCredentialsReceived)
        s = "TemporaryCredentialsReceived";

    qDebug() << "Auth Status changed: " + s;
}

void SpotifyApp::connectToSpotify()
{
    connect(&spotify, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
    connect(&spotify, &QOAuth2AuthorizationCodeFlow::statusChanged, this, &SpotifyApp::authStatusChanged);
    connect(&spotify, &QOAuth2AuthorizationCodeFlow::granted, this, &SpotifyApp::granted);
    spotify.grant();
}

void SpotifyApp::readConfig()
{
    QSettings settings(CONFIG_FILE, QSettings::IniFormat);
    settings.beginGroup("Auth");
    ClientID = settings.value("clientid").toString();
    ClientIDSharedKey = settings.value("sharedkey").toString();
}

void SpotifyApp::search(QString searchString)
{
    //types = "&type=album,artist,playlist,track,show,episode";
    QString types = "&type=track";
    QUrl requestURL ("https://api.spotify.com/v1/search?q=" + searchString + types);

    ui->lstResultadosBusca->clear();
    searchMap.clear();

    auto reply = spotify.get(requestURL);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            return;
        }
        const auto data = reply->readAll();
        //qDebug() << data;

        const auto document = QJsonDocument::fromJson(data);
        const auto jsonObj = document.object();
        //qDebug() << jsonObj.keys(); //("tracks")

        QJsonObject finalObject = jsonObj["tracks"].toObject();
        //qDebug() << finalObject.keys(); //("href", "items", "limit", "next", "offset", "previous", "total")

        QJsonArray jsonArray = finalObject["items"].toArray();
        int cont = 0;
        foreach (const QJsonValue & value, jsonArray)
        {
            QJsonObject obj = value.toObject();
            //qDebug() << "Name: " + obj["name"].toString() + ", URL: " + obj["preview_url"].toString();
            if (obj["preview_url"].toString() != EMPTY_STR)
            {
                ui->lstResultadosBusca->addItem(obj["name"].toString());
                searchMap.insert(cont++, obj["preview_url"].toString());
            }
        }

        updateAddButton();

        reply->deleteLater();
    });
}

void SpotifyApp::on_lstResultadosBusca_itemDoubleClicked(QListWidgetItem *item)
{
    addToPlaylist(item->clone());
}

void SpotifyApp::on_btnBuscar_clicked()
{
    search(ui->txtBuscar->toPlainText());
}

void SpotifyApp::on_btnAdicionar_clicked()
{
    if (ui->lstResultadosBusca->currentRow() != INVALID_ROW)
    {
        addToPlaylist(ui->lstResultadosBusca->currentItem()->clone());
    }
}

void SpotifyApp::addToPlaylist(QListWidgetItem * item)
{
    QString element = searchMap[ui->lstResultadosBusca->currentRow()];

    if (!playListUrls.contains(element))
    {
        ui->lstPlaylist->addItem(item);
        playListUrls.push_back(element);
    }
}

void SpotifyApp::updateAddButton()
{
    bool isEnabled = (searchMap.size() > SIZE_EMPTY) ? true : false;
    ui->btnAdicionar->setEnabled(isEnabled);
}

void SpotifyApp::on_lstPlaylist_itemClicked()
{
    ui->btnRemover->setEnabled(true);
}

void SpotifyApp::on_btnRemover_clicked()
{
    if (ui->lstPlaylist->currentRow() != INVALID_ROW)
    {
        removeFromPlaylist(ui->lstPlaylist->currentItem());
    }
}

void SpotifyApp::removeFromPlaylist(QListWidgetItem * item)
{
    playListUrls.removeAt(ui->lstPlaylist->currentRow());
    if (playListUrls.size() == SIZE_EMPTY)
    {
        ui->btnRemover->setEnabled(false);
    }

    delete ui->lstPlaylist->takeItem(ui->lstPlaylist->row(item));
}

void SpotifyApp::on_btnLimpaPlaylist_clicked()
{
    ui->lstPlaylist->clear();
    playListUrls.clear();
    ui->btnRemover->setEnabled(false);
}

void SpotifyApp::on_btnPlay_clicked()
{
    for(int i = 0; i < ui->lstPlaylist->count(); ++i)
    {
        playlist->addMedia(QUrl(playListUrls[i]));
    }

    mediaPlayer->setPlaylist(playlist);
    mediaPlayer->setVolume(ui->sldVolume->sliderPosition());
    mediaPlayer->play();
}

void SpotifyApp::on_btnStop_clicked()
{
    mediaPlayer->stop();
}

void SpotifyApp::on_btnAnt_clicked()
{
    mediaPlayer->playlist()->previous();
}

void SpotifyApp::on_btnProx_clicked()
{
    mediaPlayer->playlist()->next();
}

void SpotifyApp::on_sldVolume_sliderMoved(int position)
{
    mediaPlayer->setVolume(position);
}

void SpotifyApp::currentMediaChanged(const QMediaContent &content)
{
    //Alterar o texto do topo para o nome da musica atual aqui
    qDebug() << "Media changed!";
}
