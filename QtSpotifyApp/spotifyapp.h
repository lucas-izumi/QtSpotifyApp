#ifndef SPOTIFYAPP_H
#define SPOTIFYAPP_H

#include <QMainWindow>

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
};
#endif // SPOTIFYAPP_H
