#include "spotifyapp.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SpotifyApp app;
    app.show();
    return a.exec();
}
