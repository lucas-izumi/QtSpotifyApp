#include "spotifyapp.h"
#include "ui_spotifyapp.h"

SpotifyApp::SpotifyApp(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SpotifyApp)
{
    ui->setupUi(this);
}

SpotifyApp::~SpotifyApp()
{
    delete ui;
}

