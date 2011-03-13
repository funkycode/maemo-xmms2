#include "xmms2con.h"
#include <xmmsclient/xmmsclient-glib.h>
#include <xmmsclient/xmmsclient.h>
#include <glib.h>

static int32_t playback_status = 0;
static uint32_t play_time = 0;
static int32_t cur_track_duration = 0;
static int32_t cur_track_id = 0;


/*void xmms2con::xmmsc_result_notifier_set_and_unref (xmmsc_result_t *result, xmmsc_result_notifier_t func, void *user_data)
{
        xmmsc_result_notifier_set_full (result, func, user_data, NULL);
        xmmsc_result_unref(result);
}
*/


bool xmms2con::connect_xmms()
{
    connection = xmmsc_init ("maexmms");
    if (!connection) {
             fprintf (stderr, "OOM!\n");
             exit (EXIT_FAILURE);
            }
            if (!xmmsc_connect (connection, getenv ("XMMS_PATH"))) {
                            fprintf (stderr, "Connection failed: %s\n",
                                     xmmsc_get_last_error (connection));

                            exit (EXIT_FAILURE);
                        }

     //xmmsc_mainloop_gmain_init (connection);

     //xmms_callbacks();

         }

/*void xmms2con::xmms_callbacks()
{

   xmmsc_result_t* result;

   result = xmmsc_playback_status(connection);

   xmmsc_result_notifier_set_and_unref(result, on_playback_status_changed, NULL);

   XMMS_CALLBACK_SET( connection, xmmsc_broadcast_playback_status, on_playback_status_changed, NULL );


   xmmsc_result_notifier_set_and_unref( result, on_playback_cur_track_changed, NULL );

   XMMS_CALLBACK_SET( connection, xmmsc_broadcast_playback_current_id, on_playback_cur_track_changed, NULL );

}
*/

void xmms2con::start_playback()
{
    xmmsc_result_t *result;
        /*if( playback_status == XMMS_PLAYBACK_STATUS_PLAY )
        {
            result = xmmsc_playback_pause(connection);
            xmmsc_result_notifier_set_and_unref(result, on_playback_started, NULL);
             ui->play_bt->setText("Play");
        }
        else
        {
            result = xmmsc_playback_start(connection);
            xmmsc_result_unref(result);
            ui->play_bt->setText("Pause");
        }*/

    result = xmmsc_playback_start(connection);
    xmmsc_result_wait (result);
   // xmmsc_result_unref(result);


}

void xmms2con::stop_playback()
{

    xmmsc_result_t *result= xmmsc_playback_stop (connection);
    xmmsc_result_wait (result);
    //xmmsc_result_unref (result);
}
void xmms2con::next_song()
{
    xmmsc_result_t *result = xmmsc_playlist_set_next_rel(connection, 1);
    xmmsc_result_wait (result);
    //xmmsc_result_unref (result);
    result = xmmsc_playback_tickle(connection);
    xmmsc_result_wait (result);
    //xmmsc_result_unref (result);
}
void xmms2con::prev_song()
{
    xmmsc_result_t *result = xmmsc_playlist_set_next_rel(connection, -1);
    xmmsc_result_wait (result);
    //xmmsc_result_unref (result);
    result = xmmsc_playback_tickle(connection);
    xmmsc_result_wait (result);
    //xmmsc_result_unref (result);
}

/*void xmms2con::get_song_info()
{
    //int id;
    const char *err_buf;
    const char *val;
    xmmsv_t *dict_entry, *return_value, *infos;
    int id;
    song_info_str song_info;
    xmmsc_result_t *result = xmmsc_playback_current_id (connection);
    xmmsc_result_wait (result);
    return_value = xmmsc_result_get_value (result);
    if (xmmsv_is_error (return_value) &&
                xmmsv_get_error (return_value, &err_buf)) {
                    fprintf (stderr, "playback current id returns error, %s\n",
                             err_buf);
            }

            if (!xmmsv_get_int (return_value, &id)) {
                    fprintf (stderr, "xmmsc_playback_current_id didn't "
                             "return int as expected\n");
                    // Fake id (ids are >= 1) used as an error flag.
                    id = 0;
            }
    xmmsc_result_unref (result);
    result = xmmsc_medialib_get_info (connection, id);
    xmmsc_result_wait (result);
    return_value = xmmsc_result_get_value (result);
    if (xmmsv_is_error (return_value) &&
                xmmsv_get_error (return_value, &err_buf)) {
                    /*
                     * This can return error if the id
                     * is not in the medialib
                    // *
                    fprintf (stderr, "medialib get info returns error, %s\n",
                             err_buf);
                    exit (EXIT_FAILURE);
            }

    //song_info.id=id;
    infos = xmmsv_propdict_to_dict (return_value, NULL);
    if (!xmmsv_dict_get (infos, "artist", &dict_entry) ||
                !xmmsv_get_string (dict_entry, &val)) {
        val = "No Artist";
                }

    song_info.artist=val;
    if (!xmmsv_dict_get (infos, "title", &dict_entry) ||
                !xmmsv_get_string (dict_entry, &val)) {
       val = "No Title";
            }




    song_info.title = val;

    printf ("\n info = %s - %s \n", song_info.artist, song_info.title);



    xmmsv_unref (infos);
    xmmsc_result_unref (result);
   // xmmsc_unref (connection);
   //ui->song_info->setText(song_info.Title);
    //return TRUE;


}
*/
/*
int xmms2con::on_playback_status_changed( xmmsv_t *value, void *user_data )
{
    if ( !xmmsv_get_int(value, &playback_status) )
        {
            playback_status = XMMS_PLAYBACK_STATUS_STOP;
            return TRUE;
        }

    switch( playback_status )
        {
            case XMMS_PLAYBACK_STATUS_PLAY:
            {

            printf("Play");
               break;

            }
            case XMMS_PLAYBACK_STATUS_STOP:
               printf("Stopped");
                break;
            case XMMS_PLAYBACK_STATUS_PAUSE:
              printf("Pause");

                break;
        }



}
*/
/*
int xmms2con::on_playback_started( xmmsv_t* value, void* user_data )
 {
 #if 0
     //xmmsc_result_unref(res);
     // FIXME: this can cause some problems sometimes...
     //res = xmmsc_playlist_current_pos(con, cur_playlist);
     //xmmsc_result_notifier_set_and_unref(res, on_playlist_pos_changed, NULL);
 #endif
     return TRUE;
 }


int xmms2con::on_playback_cur_track_changed( xmmsv_t* value, void* user_data )
 {
     if( xmmsv_get_int(value, &cur_track_id) && cur_track_id != 0)
     {
         xmmsc_result_t *result2;
         result2 = xmmsc_medialib_get_info(connection, cur_track_id);
         xmmsc_result_notifier_set_and_unref(result2, on_playback_track_loaded, NULL);
     }

     return TRUE;
 }

int xmms2::on_playback_track_loaded( xmmsv_t* value, void* user_data )
{

    song_info_str song_info;
    QString label="test";
    const char* err;


        if (xmmsv_get_error (value, &err)) {
            g_warning( "Server error: %s", err );
            return TRUE;
        }


    MainWindow->song_info_label->setText="test";

   return TRUE;

}

*/
