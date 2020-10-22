#include "spotifyapp.h"
#include "ui_spotifyapp.h"
#include "config.h"
#include <QDesktopServices>
#include <QtNetworkAuth>

SpotifyApp::SpotifyApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SpotifyApp)
{
    ui->setupUi(this);

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
