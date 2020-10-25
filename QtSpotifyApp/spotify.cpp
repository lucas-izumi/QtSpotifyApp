#include "spotify.h"
#include "macros.h"
#include "config.h"

Spotify::Spotify()
{
    readConfig();
}

/*
 * URL de uma musica dos resultados de busca
 * @param index : chave do elemento desejado do mapa
 * @return string contendo a URL da musica correspondente aquela chave
 */
QString Spotify::getStringFromSearchMap(int index)
{
    return searchMap[index];
}

/*
 * Numero de musicas na playlist
 * @return inteiro indicando quantidade de musicas na playlist
 */
int Spotify::getPlaylistSize()
{
    return playListUrls.size();
}

/*
 * Numero de musicas nos resultados da busca
 * @return inteiro indicando quantidade de musicas nos resultados da busca
 */
int Spotify::getSearchListSize()
{
    return searchMap.size();
}

/*
 * URL de uma musica da playlist
 * @param index : indice do elemento desejado do mapa
 * @return string contendo a URL da musica correspondente aquele indice
 */
QString Spotify::getPlaylistItem(int index)
{
    return playListUrls[index];
}

/*
 * Obtem o indice de um elemento da playlist, se existir
 * @param item : string a ser procurada no vector
 * @return inteiro com o indice do elemento, caso seja encontrado. Caso contrario, retorna -1
 */
int Spotify::getPlaylistItemIndex(QString item)
{
    return playListUrls.indexOf(item);
}

/*
 * Remove um elemento da playlist
 * @param index : indice do elemento a ser removido
 * @return true se a operacao foi realizada, false caso contrario
 */
bool Spotify::removeFromPlaylist(int index)
{
    if (index >= playListUrls.size() || index < 0)
        return false;

    playListUrls.removeAt(index);
    return true;
}

/*
 * Adiciona um elemento na playlist. A adicao nao eh realizada caso a playlist ja contenha o elemento
 * @param element : string com a URL (preview_url) da musica
 * @return true se a insercao foi realizada, false caso contrario
 */
bool Spotify::addToPlaylist(QString element)
{
    if (!playListUrls.contains(element))
    {
        playListUrls.push_back(element);
        return true;
    }
    return false;
}

/*
 * Realiza uma busca por musica (track) com os parametros enviados e exibe
 * os resultados retornados que possuam uma URL de preview disponível (preview_url).
 * Os resultados ficam disponiveis no container "searchMap", onde eh possivel verificar
 * a quantidade de resultados obtidos ao checar seu tamamnho. Atualmente o maximo de resultados
 * retornados por esta funcao eh 20.
 * @param searchString : string com as palavras-chave da busca
 * @param lstSearchResults : widget do tipo QListWidget onde os resultados vao ser exibidos
 */
void Spotify::search(QString searchString, QListWidget * lstSearchResults)
{
    //types = "&type=album,artist,playlist,track,show,episode";
    QString types = "&type=track";
    QUrl requestURL ("https://api.spotify.com/v1/search?q=" + searchString + types);

    searchMap.clear();

    auto reply = spotify.get(requestURL);

    connect(reply, &QNetworkReply::finished, [=]() {
        if (reply->error() != QNetworkReply::NoError)
        {
            qDebug() << reply->errorString();
            return;
        }
        const auto data = reply->readAll();
        //qDebug() << data;

        const auto document = QJsonDocument::fromJson(data);
        const auto jsonObj = document.object();
        //qDebug() << jsonObj.keys(); //("tracks")

        QJsonObject finalObject = jsonObj["tracks"].toObject();
        //qDebug() << finalObject.keys(); //("href", "items", "limit", "next", "offset", "previous", "total")

        QJsonArray jsonArray = finalObject["items"].toArray();
        int cont = 0;
        foreach (const QJsonValue & value, jsonArray)
        {
            QJsonObject obj = value.toObject();
            //qDebug() << "Name: " + obj["name"].toString() + ", URL: " + obj["preview_url"].toString();
            if (obj["preview_url"].toString() != EMPTY_STR)
            {
                lstSearchResults->addItem(obj["name"].toString());
                searchMap.insert(cont++, obj["preview_url"].toString());
            }
        }

        reply->deleteLater();
    });
}

/*
 * Salva as configuracoes do player em arquivo
 * @param volume : inteiro entre 0 e 100 contendo o valor do volume
 */
void Spotify::saveSettings(int volume)
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.beginGroup("Player");
    settings.setValue("volume",volume);
    settings.endGroup();
}

/*
 * Le as configuracoes do player de arquivo
 * @param volume : inteiro por referencia que recebera o valor do volume salvo
 */
void Spotify::readSettings(int &volume)
{
    QSettings settings(SETTINGS_FILE, QSettings::IniFormat);
    settings.beginGroup("Player");
    volume = settings.value("volume", 50).toInt();
}

/*
 * Salva a playlist atual em arquivo, no formato:
 * NOME_DISPLAY_MUSICA="URL_PREVIEW_MUSICA"
 * @param playlist : Vector de strings contendo os nomes de display das musicas da playlist
 */
void Spotify::savePlaylist(QVector<QString> playlist)
{
    QFile::remove(PLAYLIST_FILE);
    QSettings settings(PLAYLIST_FILE, QSettings::IniFormat);
    settings.beginGroup("Musicas");
    int playlistSize = playListUrls.size();
    for (int i=0; i < playlistSize; ++i)
    {
        settings.setValue(playlist[i], playListUrls[i]);
    }
}

/*
 * Carrega uma playlist salva
 * @return Vector de string contendo os nomes de display das musicas
 */
QVector<QString> Spotify::loadPlaylist()
{
    QSettings settings(PLAYLIST_FILE, QSettings::IniFormat);
    settings.beginGroup("Musicas");
    QStringList keys = settings.allKeys();
    QVector<QString> songNames;
    for (int i=0; i<keys.size(); ++i)
    {
        playListUrls.push_back(settings.value(keys[i]).toString());
        songNames.push_back(keys[i]);
    }
    return songNames;
}

/*
 * Configura os campos do QOAuth2AuthorizationCodeFlow para realizar a conexao com o Spotify
 */
void Spotify::setupConnection()
{
    auto replyHandler = new QOAuthHttpServerReplyHandler(8080, this);
    spotify.setReplyHandler(replyHandler);
    spotify.setAuthorizationUrl(QUrl("https://accounts.spotify.com/authorize"));
    spotify.setAccessTokenUrl(QUrl("https://accounts.spotify.com/api/token"));
    spotify.setClientIdentifier(ClientID);
    spotify.setClientIdentifierSharedKey(ClientIDSharedKey);
    spotify.setScope("user-read-private user-top-read playlist-read-private playlist-modify-public playlist-modify-private");
}

/*
 * SLOT acionado quando o SIGNAL de granted é disparado durante o processo de autenticacao
 */
void Spotify::granted()
{
    qDebug() << "Signal granted received";
    //qDebug() << "Token: " + spotify.token();
    isGranted = true;
}

/*
 * SLOT acionado quando o status muda durante o processo de autenticacao
 * @param status : status recebido do SIGNAL QOAuth2AuthorizationCodeFlow::statusChanged
 */
void Spotify::authStatusChanged(QAbstractOAuth::Status status)
{
    QString s;
    if (status == QAbstractOAuth::Status::Granted)
        s = "Granted";

    if (status == QAbstractOAuth::Status::TemporaryCredentialsReceived)
        s = "TemporaryCredentialsReceived";

    qDebug() << "Auth Status changed: " + s;
}

/*
 * Estabelece a conexao com o Spotify
 * @return int : codigo de erro obtido durante o processo de conexao
 */
int Spotify::connectToSpotify()
{
    try
    {
        if (!QSslSocket::supportsSsl())
        {
            throw ERR_NO_SSL_SUPPORT;
        }

        setupConnection();
        connect(&spotify, &QOAuth2AuthorizationCodeFlow::authorizeWithBrowser, &QDesktopServices::openUrl);
        connect(&spotify, &QOAuth2AuthorizationCodeFlow::statusChanged, this, &Spotify::authStatusChanged);
        connect(&spotify, &QOAuth2AuthorizationCodeFlow::granted, this, &Spotify::granted);
        spotify.grant();
        return ERR_NO_ERROR;

    }
    catch (const int error)
    {
        return error;
    }
}

/*
 * Le as configuracoes de Client ID e Client ID Shared Key do arquivo
 * Formato:
 * clientid=string
 * sharedkey=string
 */
void Spotify::readConfig()
{
    QSettings settings(CONFIG_FILE, QSettings::IniFormat);
    settings.beginGroup("Auth");
    ClientID = settings.value("clientid").toString();
    ClientIDSharedKey = settings.value("sharedkey").toString();
}

/*
 * Limpa todos os itens da playlist
 */
void Spotify::clearPlaylist()
{
    playListUrls.clear();
}
