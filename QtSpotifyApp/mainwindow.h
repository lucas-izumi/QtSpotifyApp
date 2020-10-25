#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QMediaPlaylist>
#include <QMediaPlayer>
#include "spotify.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

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

    void on_btnSalvarPlaylist_clicked();

private:
    Ui::MainWindow *ui;
    QMediaPlayer *mediaPlayer;
    QMediaPlaylist *playlist;
    Spotify *spotify;

    void updateAddButton();

    void setSettings();

    void loadPlaylist();
};

void connectToSpotify(Spotify * s, MainWindow *w);
bool fileExists(QString path);

#endif // MAINWINDOW_H
