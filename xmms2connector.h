#ifndef XMMS2CONNECTOR_H
#define XMMS2CONNECTOR_H

#include <QtCore>

typedef struct xmmsc_connection_St xmmsc_connection_t;
typedef struct xmmsv_St xmmsv_t;

/*
typedef struct
{
    const char *artist;
    const char *album;
    const char *title;
    const char *url;
    const char *mime;
    const char *comment;
    int32_t duration;
    int32_t isvbr;
    int32_t bitrate;
    int32_t size;
    int _currentStatus;
    bool isNothingPlaying;
} SongData;
*/
\
typedef struct
{
    QString artist, album, title;
    bool isNothingPlaying;
} SongData;





class Xmms2Connector : public QObject
{
    Q_OBJECT
    xmmsc_connection_t *connection;
    SongData _currentSongData;
    int _currentStatus;

    SongData extractSongData(int id);

    static int on_playback_track_loaded(xmmsv_t* value, void* user_data);
    static int on_playback_status_changed(xmmsv_t *value, void *user_data);
    static int on_playback_cur_track_changed(xmmsv_t* value, void* user_data);

public:
    enum Status { Playing, Paused, Stopped };

    explicit Xmms2Connector(QObject *parent);
    ~Xmms2Connector();
    SongData currentSong();
    Status currentStatus();

public slots:
    bool xmms2Connect();
    void refresh();
    void startPlayback();
    void pausePlayback();
    void stopPlayback();
    void jumpToNextSong();
    void jumpToPreviousSong();

signals:
    void currentSongChanged(SongData song);
    void statusChanged(Status status);
    void started();
    void paused();
    void stopped();

};

#endif // XMMS2CONNECTOR_H
