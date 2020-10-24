#ifndef SPOTIFYAPP_H
#define SPOTIFYAPP_H

#include <QMainWindow>
#include <QOAuth2AuthorizationCodeFlow>
#include <QListWidgetItem>

QT_BEGIN_NAMESPACE
namespace Ui { class SpotifyApp; }
QT_END_NAMESPACE

class SpotifyApp : public QMainWindow
{
    Q_OBJECT

public:
    SpotifyApp(QWidget *parent = nullptr);
    ~SpotifyApp();

private slots:
    void on_lstResultadosBusca_itemDoubleClicked(QListWidgetItem *item);

    void on_btnBuscar_clicked();

private:
    Ui::SpotifyApp *ui;
    QMap<int, QString> searchMap;
    QVector<QString> playListUrls;

    void setupConnection();
    void granted();
    void authStatusChanged (QAbstractOAuth::Status status);
    void connectToSpotify();
    void readConfig();
    void search(QString searchString);

    QOAuth2AuthorizationCodeFlow spotify;
    bool isGranted;
    QString ClientID;
    QString ClientIDSharedKey;
};
#endif // SPOTIFYAPP_H
