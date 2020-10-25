#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "config.h"
#include "macros.h"
#include "downloader.h"
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mediaPlayer = new QMediaPlayer;
    playlist = new QMediaPlaylist;

    connect(mediaPlayer, SIGNAL(currentMediaChanged(const QMediaContent)), this, SLOT(currentMediaChanged(const QMediaContent)));

    spotify = new Spotify();
    setSettings();
    loadPlaylist();
    connectToSpotify(spotify, this);
}

void MainWindow::setSettings()
{
    int vol;
    spotify->readSettings(vol);
    mediaPlayer->setVolume(vol);
    ui->sldVolume->setSliderPosition(vol);
}

void MainWindow::loadPlaylist()
{
    QVector<QString> playlist = spotify->loadPlaylist();
    for (int i=0; i<playlist.size(); ++i)
    {
        ui->lstPlaylist->addItem(playlist[i]);
    }
}

MainWindow::~MainWindow()
{
    spotify->saveSettings(mediaPlayer->volume());
    delete ui;
}

void MainWindow::on_lstResultadosBusca_itemDoubleClicked(QListWidgetItem *item)
{
    QString element = spotify->getStringFromSearchMap(ui->lstResultadosBusca->currentRow());

    if (spotify->addToPlaylist(element))
    {
        ui->lstPlaylist->addItem(item->clone());
    }
}

void MainWindow::on_btnBuscar_clicked()
{
    ui->lstResultadosBusca->clear();
    spotify->search(ui->txtBuscar->toPlainText(), ui->lstResultadosBusca);
}

void MainWindow::on_btnAdicionar_clicked()
{
    if (ui->lstResultadosBusca->currentRow() != INVALID_ROW)
    {
        QString element = spotify->getStringFromSearchMap(ui->lstResultadosBusca->currentRow());
        if (spotify->addToPlaylist(element))
        {
            ui->lstPlaylist->addItem(ui->lstResultadosBusca->currentItem()->clone());
        }
    }
}

void MainWindow::on_lstPlaylist_itemClicked()
{
    ui->btnRemover->setEnabled(true);
}

void MainWindow::on_btnRemover_clicked()
{
    if (ui->lstPlaylist->currentRow() != INVALID_ROW)
    {
        if (spotify->removeFromPlaylist(ui->lstPlaylist->currentRow()))
        {
            if (spotify->getPlaylistSize() == SIZE_EMPTY)
            {
                ui->btnRemover->setEnabled(false);
            }
            delete ui->lstPlaylist->takeItem(ui->lstPlaylist->row(ui->lstPlaylist->currentItem()));
        }
    }
}

void MainWindow::on_btnLimpaPlaylist_clicked()
{
    ui->lstPlaylist->clear();
    spotify->clearPlaylist();
    ui->btnRemover->setEnabled(false);
}

void MainWindow::on_btnPlay_clicked()
{
    for(int i = 0; i < ui->lstPlaylist->count(); ++i)
    {
        playlist->addMedia(QUrl(spotify->getPlaylistItem(i)));
    }

    mediaPlayer->setPlaylist(playlist);
    mediaPlayer->play();
}

void MainWindow::on_btnStop_clicked()
{
    mediaPlayer->stop();
}

void MainWindow::on_btnAnt_clicked()
{
    mediaPlayer->playlist()->previous();
}

void MainWindow::on_btnProx_clicked()
{
    mediaPlayer->playlist()->next();
}

void MainWindow::on_sldVolume_sliderMoved(int position)
{
    mediaPlayer->setVolume(position);
}

void MainWindow::currentMediaChanged(const QMediaContent &content)
{
    int pos = spotify->getPlaylistItemIndex(content.request().url().toString());

    if (pos >= 0)
    {
        ui->lblMusicaAtual->setText("Tocando: " + ui->lstPlaylist->item(pos)->text());
    }
    else
    {
        ui->lblMusicaAtual->setText("Spotify Player");
    }
}

void MainWindow::on_btnSalvarPlaylist_clicked()
{
    QVector<QString> musicList;
    for (int i=0; i<spotify->getPlaylistSize(); ++i)
    {
        musicList.push_back(ui->lstPlaylist->item(i)->text());
    }
    spotify->savePlaylist(musicList);
    QMessageBox::information(this, tr("Spotify Player"), tr("Playlist salva com sucesso!"));
}

void connectToSpotify(Spotify * s, MainWindow *w)
{
    int err = s->connectToSpotify();

    switch (err)
    {
        case ERR_NO_SSL_SUPPORT:
            QMessageBox::critical(w, w->tr("Spotify Player"), w->tr("Suporte a SSL não encontrado!"));
            exit(1);
        case ERR_NO_ERROR:
        default:
            return;

    }
}

bool fileExists(QString path)
{
    QFileInfo check_file(path);
    return (check_file.exists() && check_file.isFile()) ? true : false;
}
