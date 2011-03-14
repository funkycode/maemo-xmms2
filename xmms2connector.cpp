#include "xmms2connector.h"
#include <xmms2/xmmsclient/xmmsclient.h>
#include <xmmsclient/xmmsclient-glib.h>

int Xmms2Connector::on_playback_track_loaded(xmmsv_t* value, void* user_data)
{
    const char* err;
    if (xmmsv_get_error (value, &err))
    {
        qDebug() << "Server error:" << err;
        return FALSE;
    }

    ((Xmms2Connector*)user_data)->refresh();
    return TRUE;
}

int Xmms2Connector::on_playback_status_changed(xmmsv_t *value, void *user_data)
{
    int playback_status;
    if (!xmmsv_get_int(value, &playback_status))
    {
        return FALSE;
    }

    Xmms2Connector *instance = (Xmms2Connector*) user_data;

    switch( playback_status )
    {
    case XMMS_PLAYBACK_STATUS_PLAY:
        instance->_currentStatus = Playing;
        emit instance->started();
        break;
    case XMMS_PLAYBACK_STATUS_PAUSE:
        emit instance->paused();
        instance->_currentStatus = Paused;
        break;
    case XMMS_PLAYBACK_STATUS_STOP:
        instance->_currentStatus = Stopped;
        emit instance->stopped();
        break;
    }

    return TRUE;
}

int Xmms2Connector::on_playback_cur_track_changed(xmmsv_t* value, void* user_data)
{
    Xmms2Connector *instance = (Xmms2Connector*) user_data;
    int cur_track_id;
    if( xmmsv_get_int(value, &cur_track_id) && cur_track_id != 0)
    {
        xmmsc_result_t *result2;
        result2 = xmmsc_medialib_get_info(instance->connection, cur_track_id);
        xmmsc_result_notifier_set(result2, on_playback_track_loaded, user_data);
    }

    return TRUE;
}

Xmms2Connector::Xmms2Connector(QObject *parent) : QObject(parent)
{
    _currentStatus = Stopped;
}

Xmms2Connector::~Xmms2Connector()
{
    xmmsc_mainloop_gmain_shutdown(connection, 0);
    xmmsc_quit(connection);
}

Xmms2Connector::Status Xmms2Connector::currentStatus()
{
    return (Xmms2Connector::Status)_currentStatus;
}

bool Xmms2Connector::xmms2Connect()
{
    // Establishing connection
    connection = xmmsc_init ("zoggs_fresh_xmms");

    if (!connection)
    {
        qDebug() << "Unable to initialize XMMS";
        return false;
    }
    if (!xmmsc_connect (connection, getenv ("XMMS_PATH")))
    {
        qDebug() << "Unable to connect to XMMS, error is" << xmmsc_get_last_error (connection);
        return false;
    }

    // Initiating glib main loop and setting up callbacks
    xmmsc_mainloop_gmain_init (connection);
    xmmsc_result_t* result = xmmsc_playback_status(connection);

    xmmsc_result_notifier_set(result, on_playback_status_changed, this);
    XMMS_CALLBACK_SET( connection, xmmsc_broadcast_playback_status, on_playback_status_changed, this);

    xmmsc_result_notifier_set( result, on_playback_cur_track_changed, this );
    XMMS_CALLBACK_SET( connection, xmmsc_broadcast_playback_current_id, on_playback_cur_track_changed, this);

    return true;
}

SongData Xmms2Connector::extractSongData(int id)
{
    qDebug() << "extracting song data";
    SongData newSongData;

    if (id == 0)
    {
        qDebug() << "Nothing is playing";
        newSongData.isNothingPlaying = true;
        return newSongData;
    }

    // Setting up the media lib info

    const char *val, *err_buf;
    xmmsc_result_t *result2 = xmmsc_medialib_get_info (connection, id);
    xmmsc_result_wait (result2);
    xmmsv_t *return_value = xmmsc_result_get_value (result2);

    if (xmmsv_is_error (return_value) && xmmsv_get_error (return_value, &err_buf))
    {
        qDebug() << "medialib get info error" << err_buf;
        return newSongData;
    }

    // Getting the artist name

    xmmsv_t *infos = xmmsv_propdict_to_dict (return_value, NULL);
    xmmsv_t *dict_entry;

    if (!xmmsv_dict_get (infos, "artist", &dict_entry) || !xmmsv_get_string (dict_entry, &val))
    {
        qDebug() << "No artist";
    }
    newSongData.artist = QString(val);

    // Getting the album name

    if (!xmmsv_dict_get (infos, "album", &dict_entry) || !xmmsv_get_string (dict_entry, &val))
    {
        qDebug() << "No artist";
    }
    newSongData.album = QString(val);

    // Getting the title

    if (!xmmsv_dict_get (infos, "title", &dict_entry) || !xmmsv_get_string (dict_entry, &val))
    {
        qDebug() << "No title";
    }
    newSongData.title = QString(val);

    xmmsv_unref (infos);
    xmmsc_result_unref (result2);

    return newSongData;
}

SongData Xmms2Connector::currentSong()
{
    return _currentSongData;
}

void Xmms2Connector::refresh()
{
    xmmsc_result_t *result = xmmsc_playback_current_id (connection);
    xmmsc_result_wait (result);
    xmmsv_t *value = xmmsc_result_get_value(result);
    int id;
    xmmsv_get_int (value, &id);
    xmmsc_result_unref (result);
    _currentSongData = extractSongData(id);
    emit currentSongChanged(_currentSongData);
    qDebug() << _currentSongData.title;
}

void Xmms2Connector::startPlayback()
{
    xmmsc_result_t *result = xmmsc_playback_start (connection);
    //xmmsc_result_wait (result);
    xmmsc_result_unref (result);
}

void Xmms2Connector::pausePlayback()
{
    xmmsc_result_t *result = xmmsc_playback_pause(connection);
    //xmmsc_result_wait (result);
    xmmsc_result_unref (result);
}

void Xmms2Connector::stopPlayback()
{
    xmmsc_result_t *result = xmmsc_playback_stop (connection);
    //xmmsc_result_wait (result);
    xmmsc_result_unref (result);
}

void Xmms2Connector::jumpToNextSong()
{
    xmmsc_result_t *result = xmmsc_playlist_set_next_rel(connection, 1);
    //xmmsc_result_wait (result);
    result = xmmsc_playback_tickle(connection);
    //xmmsc_result_wait (result);
    xmmsc_result_unref (result);
}

void Xmms2Connector::jumpToPreviousSong()
{
    xmmsc_result_t *result = xmmsc_playlist_set_next_rel(connection, -1);
    //xmmsc_result_wait (result);
    result = xmmsc_playback_tickle(connection);
    //xmmsc_result_wait (result);
    xmmsc_result_unref (result);
}
