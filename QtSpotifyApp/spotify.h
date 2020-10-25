#ifndef SPOTIFY_H
#define SPOTIFY_H

#include <QObject>
#include <QListWidgetItem>
#include <QOAuth2AuthorizationCodeFlow>
#include <QFileInfo>
#include <QSettings>
#include <QtNetworkAuth>
#include <QDesktopServices>
#include <QMessageBox>
#include <QSslSocket>

class Spotify : public QObject
{
    Q_OBJECT
public:
    Spotify();
    bool removeFromPlaylist(int index);
    bool addToPlaylist(QString element);
    QString getStringFromSearchMap(int index);
    int getPlaylistSize();
    void saveSettings(int volume);
    void savePlaylist(QVector<QString> playlist);
    QVector<QString> loadPlaylist();
    void search(QString searchString, QListWidget * lstSearchResults);
    void clearPlaylist();
    int getSearchListSize();
    int connectToSpotify();
    QString getPlaylistItem(int index);
    int getPlaylistItemIndex(QString item);

    void readSettings(int &volume);


private:
    QMap<int, QString> searchMap;
    QVector<QString> playListUrls;

    void granted();
    void authStatusChanged (QAbstractOAuth::Status status);
    void readConfig();
    void setupConnection();

    QOAuth2AuthorizationCodeFlow spotify;
    bool isGranted;
    QString ClientID;
    QString ClientIDSharedKey;
};

#endif // SPOTIFY_H
