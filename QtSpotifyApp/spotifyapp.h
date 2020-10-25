#ifndef SPOTIFYAPP_H
#define SPOTIFYAPP_H

#include <QMainWindow>
#include <QOAuth2AuthorizationCodeFlow>
#include <QListWidgetItem>
#include <QMediaPlaylist>
#include <QMediaPlayer>

QT_BEGIN_NAMESPACE
namespace Ui { class SpotifyApp; }
QT_END_NAMESPACE

bool fileExists(QString path);

class SpotifyApp : public QMainWindow
{
    Q_OBJECT

public:
    SpotifyApp(QWidget *parent = nullptr);
    ~SpotifyApp();

private slots:
    void on_lstResultadosBusca_itemDoubleClicked(QListWidgetItem *item);

    void on_btnBuscar_clicked();

    void on_btnAdicionar_clicked();

    void on_lstPlaylist_itemClicked();

    void on_btnRemover_clicked();

    void on_btnLimpaPlaylist_clicked();

    void on_btnPlay_clicked();

    void on_btnStop_clicked();

    void on_btnAnt_clicked();

    void on_btnProx_clicked();

    void on_sldVolume_sliderMoved(int position);

    void currentMediaChanged(const QMediaContent &content);

private:
    Ui::SpotifyApp *ui;
    QMap<int, QString> searchMap;
    QVector<QString> playListUrls;
    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *playlist;

    void setupConnection();
    void granted();
    void authStatusChanged (QAbstractOAuth::Status status);
    void connectToSpotify();
    void readConfig();
    void search(QString searchString);

    void addToPlaylist(QListWidgetItem * item);
    void removeFromPlaylist(QListWidgetItem * item);
    void updateAddButton();

    QOAuth2AuthorizationCodeFlow spotify;
    bool isGranted;
    QString ClientID;
    QString ClientIDSharedKey;
};
#endif // SPOTIFYAPP_H
