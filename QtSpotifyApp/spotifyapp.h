#ifndef SPOTIFYAPP_H
#define SPOTIFYAPP_H

#include <QMainWindow>
#include <QOAuth2AuthorizationCodeFlow>

QT_BEGIN_NAMESPACE
namespace Ui { class SpotifyApp; }
QT_END_NAMESPACE

class SpotifyApp : public QMainWindow
{
    Q_OBJECT

public:
    SpotifyApp(QWidget *parent = nullptr);
    ~SpotifyApp();

private:
    Ui::SpotifyApp *ui;

    void setupConnection();
    void granted();
    void authStatusChanged (QAbstractOAuth::Status status);
    void connectToSpotify();
    void readConfig();
    void search();

    QOAuth2AuthorizationCodeFlow spotify;
    bool isGranted;
    QString ClientID;
    QString ClientIDSharedKey;
};
#endif // SPOTIFYAPP_H
