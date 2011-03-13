#ifndef XMMS2CON_H
#define XMMS2CON_H

#include <QtCore>

#include <xmmsclient/xmmsclient-glib.h>
#include <xmmsclient/xmmsclient.h>
#include <glib.h>

typedef struct _song_info_str{
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
}song_info_str;

class xmms2con : public QObject
{
        Q_OBJECT
        xmmsc_connection_t *connection;

public:

    explicit xmms2con(QObject *parent);
public slots:

bool connect_xmms();
/*
int on_playback_track_loaded( xmmsv_t* value, void* user_data );
int on_playback_started( xmmsv_t* value, void* user_data );
int on_playback_status_changed( xmmsv_t *value, void *user_data );
void xmms_callbacks();
int on_playback_cur_track_changed( xmmsv_t* value, void* user_data );
*/
void start_playback();
void stop_playback();
void next_song();
void prev_song();
};
#endif // XMMS2CON_H
