# Qt Spotify App
Este projeto utiliza a API do Spotify (https://developer.spotify.com/web-api/) para criar uma aplicação que controla e executa uma playlist de Samples do Spotify.

## Requisitos
- OpenSSL (https://www.openssl.org/)

## Utilização
É necessário preencher o arquivo **config.ini** com os dados do **Client ID** e **Client ID Shared Key** da sua aplicação do Spotify. Este arquivo deve ficar na raiz do aplicativo compilado. É possível alterar o caminho para este arquivo editando o header `config.h.`

## Pendências, Bugs conhecidos e Melhorias
- A duração do token recebido após a autenticação é de 1 hora. Não está sendo feito um tratamento caso este token expire, requerendo que o aplicativo tenha que ser reiniciado para atualizar o token quando o tempo estourar. (https://developer.spotify.com/documentation/ios/guides/token-swap-and-refresh/)

- A API do Spotify agrupa os resultados da busca em pacotes com 20 elementos, indicando quantos pacotes desse existem no total. Atualmente a aplicação considera apenas o primeiro pacote de 20 elementos recebido, não realizando uma requisição pelos demais. Adicionar um sistema de paginação de busca e obter todos os resultados retornados é algo a ser considerado para uma próxima versão.

- Foi criada uma classe para download de arquivos (ainda não documentada) para adicionar a possibilidade de baixar uma playlist para ser executada localmente. Para esta versão, porém, o escopo foi reduzido em tocar as músicas da playlist através do link de sample delas.

- Melhorar a lógica dos widgets ativos/inativos para refletir melhor as operações que podem ser realizadas pelo usuário.

- A navegação dos botões de **Próximo** e **Anterior** precisam de correção. Ao clicar nos botões de **Próximo**, quando no ultimo elemento, e **Anterior** (quando no primeiro elemento), enquanto uma playlist está tocando, a execução da música é encerrada. Se o usuário continuar pressionando esses botões, eventualmente, a label que indica a musica em execução atualmente é atualizada (mesmo sem nenhum som estar tocando).

## Referências
- https://developer.spotify.com/documentation/general/guides/authorization-guide/
- https://developer.spotify.com/documentation/web-api/reference/tracks/get-track/
- https://developer.spotify.com/documentation/web-api/reference/search/search/
- https://adminundcode.de/using-spotify-webapi-with-qt/
- https://doc.qt.io/qt-5/

------------

# A classe Spotify
`Header: #include "spotify.h"`

Estabele uma interface com a API do Spotify, permitindo buscar músicas e realizar o controle de playlists locais que tocam os Samples (áudio de 30 segundos) das músicas contidas nelas.

## Funções Públicas
| Tipo de retorno  | Método  |
| ------------ | ------------ |
|  bool  | removeFromPlaylist(int index)  |
| bool   |  addToPlaylist(QString element) |
| QString  |  getStringFromSearchMap(int index)  |
| int  |  getPlaylistSize()  |
| void   | saveSettings(int volume)  |
| void   |  savePlaylist(QVector<<x></xQString>QString> playlist) |
| QVector<<x>QString>   |  loadPlaylist() |
| void   |  search(QString searchString, QListWidget * lstSearchResults) |
|  void |  clearPlaylist()  |
| int   | getSearchListSize()  |
| int   | connectToSpotify()  |
|  QString  |  getPlaylistItem(int index) |
| int   | getPlaylistItemIndex(QString item)  |
| void   |  readSettings(int &volume) |

------------

### bool removeFromPlaylist(int index)
Remove um elemento da playlist  
**index** : indice do elemento a ser removido  
**return** : true se a operacao foi realizada, false caso contrario

------------

### bool addToPlaylist(QString element)
Adiciona um elemento na playlist. A adicao nao eh realizada caso a playlist ja contenha o elemento  
**element** : string com a URL (preview_url) da musica  
**return** : true se a insercao foi realizada, false caso contrario

------------

### QString getStringFromSearchMap(int index)
Retorna a URL de uma musica dos resultados de busca.  
**index** : chave do elemento desejado do mapa  
**return** : string contendo a URL da musica correspondente aquela chave

------------

### int getPlaylistSize()
Numero de musicas na playlist  
**return** : inteiro indicando quantidade de musicas na playlist  

------------

### void saveSettings(int volume)
Salva as configuracoes do player em arquivo  
**volume** : inteiro entre 0 e 100 contendo o valor do volume  

------------

### void savePlaylist(QVector<QString> playlist)
Salva a playlist atual em arquivo, no formato:  
`NOME_DISPLAY_MUSICA="URL_PREVIEW_MUSICA"`  
  
**playlist** : Vector de strings contendo os nomes de display das musicas da playlist  

------------

### QVector<QString> loadPlaylist()
Carrega uma playlist salva  
**return** : Vector de string contendo os nomes de display das musicas  

------------

### void search(QString searchString, QListWidget * lstSearchResults)
Realiza uma busca por musica (track) com os parametros enviados e exibe os resultados retornados que possuam uma URL de preview disponível (preview_url). Os resultados ficam disponiveis no container **searchMap**, onde é possivel verificar a quantidade de resultados obtidos ao checar seu tamamnho. Atualmente o maximo de resultados retornados por esta funcao é 20.  

**searchString** : string com as palavras-chave da busca  
**lstSearchResults** : widget do tipo QListWidget onde os resultados vao ser exibidos

------------

### void clearPlaylist()
Limpa todos os itens da playlist.

------------

### int getSearchListSize()
Numero de musicas nos resultados da busca  
**return** : inteiro indicando quantidade de musicas nos resultados da busca

------------

### int connectToSpotify()
Estabelece a conexao com o Spotify
Retorna um inteiro com o codigo de erro obtido durante o processo de conexao. Para a lista de códigos retornados, consultar [Lista de Erros](#Lista de erros).  
**Exemplo:**  
```cpp
Spotify *s = new Spotify();
int err = s->connectToSpotify();
if (err == 0) { qDebug() << "Conexao realizada com sucesso!" };
```
------------

### QString getPlaylistItem(int index)
URL de uma musica da playlist  

**index** : indice do elemento desejado do mapa  
**return** : string contendo a URL da musica correspondente aquele indice

------------

### int getPlaylistItemIndex(QString item)
Obtem o indice de um elemento da playlist, se existir  

**item** : string a ser procurada no vector  
**return** : inteiro com o indice do elemento, caso seja encontrado. Caso contrario, retorna -1

------------

### void readSettings(int &volume)
Lê as configuracoes do player de arquivo  

**volume** : inteiro por referencia que receberá o valor do volume salvo.

------------

## Lista de erros
|  Código | Macro  | Descrição  |
| ------------ | ------------ | ------------ |
| 0  | ERR_NO_ERROR  |  Nenhum erro retornado |
| 100  | ERR_NO_SSL_SUPPORT  | Plataforma não possui suporte a SSL  |


