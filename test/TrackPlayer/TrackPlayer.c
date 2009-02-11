////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIT X11 license, Copyright (c) 2005-2006 by:                               //
//                                                                            //
// Authors:                                                                   //
//      Michael Dominic K. <michaldominik@gmail.com>                          //
//                                                                            //
// Permission is hereby granted, free of charge, to any person obtaining a    //
// copy of this software and associated documentation files (the "Software"), //
// to deal in the Software without restriction, including without limitation  //
// the rights to use, copy, modify, merge, publish, distribute, sublicense,   //
// and/or sell copies of the Software, and to permit persons to whom the      //
// Software is furnished to do so, subject to the following conditions:       //
//                                                                            //
// The above copyright notice and this permission notice shall be included    //
// in all copies or substantial portions of the Software.                     //
//                                                                            //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS    //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                 //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN  //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR      //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE  //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                     //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////

#include <gdv.h>
#include "TrackPlayer.h"
#include <gtk/gtk.h>

#define                         MEDIA_1 "../media/Sailing Boat.dv"

#define                         MEDIA_2 "../media/Rock.ogg"

#define                         MEDIA_4 "../media/A Lecture.dv"

#define                         MEDIA_3 "../media/La citte.wav"

#define                         MEDIA_5 "../media/Mouth.ogg"

#define                         MEDIA_6 "../media/Julia i marysia.jpg"

void                            show_banner (void)
{
        printf ("TrackPlayer uses the sample media repository to construct a simple track.\n"
                "The track consists of various test-media aligned one after another.\n"
                "It uses the standard output sinks.\n\n");
}

void                            set_inspector (gchar *uri)
{
        printf ("+ Building and starting inspector for %s\n", uri);
        GdvInspector *inspector = gdv_inspector_new (uri);
        g_return_if_fail (inspector != NULL);
        
        gdv_inspector_set_done_func (inspector, inspector_done_func);
        gdv_inspector_set_error_func (inspector, inspector_error_func);

        InspectionTotal++;
        gdv_inspector_start (inspector);
}

int                             main (int argc, gchar **argv)
{
        // Initialize
        show_banner ();
        gtk_init (&argc, &argv);
        gst_init (&argc, &argv);

        // Build pipeline
        build_pipeline ();
        link_pipeline ();

        // Build the inspectors
        set_inspector (MEDIA_1);
        set_inspector (MEDIA_2);
        set_inspector (MEDIA_3);
        set_inspector (MEDIA_4);
        //set_inspector (MEDIA_5);        
        set_inspector (MEDIA_6);        
 
        printf ("+ Entering main loop\n");
        gtk_main ();

        return 0;
}

gboolean                        inspector_done_func (GdvInspector *inspector)
{
        printf ("+ Inspecting done.\n");

        printf ("+ Generating media item\n");
        GdvMediaItem *item = gdv_itemgenerator_from_inspector (ProjectFormat, inspector);
        g_return_val_if_fail (item != NULL, FALSE);

        printf ("+ Creating the clip\n");
        GdvClip *clip = gdv_clip_new (item);
        g_return_val_if_fail (clip != NULL, FALSE);

        GdvTime start = 0;
        GdvTime stop = GDV_SECOND * 5;
        GdvTime position = 0;

        // Here depending on what's the clip
        gchar *url;
        g_object_get (G_OBJECT (inspector), "url", &url, NULL);

        
        if (g_ascii_strncasecmp (url, MEDIA_1, strlen (url)) == 0) {
                // MEDIA_1
                start = GDV_SECOND * 0;
                stop = GDV_SECOND * 5;
                position = GDV_SECOND * 5;
        } else if (g_ascii_strncasecmp (url, MEDIA_3, strlen (url)) == 0) {
                // MEDIA_3
                start = GDV_SECOND * 1;
                stop = GDV_SECOND * 6;
                position = GDV_SECOND * 10;
        } else if (g_ascii_strncasecmp (url, MEDIA_2, strlen (url)) == 0) {
                // MEDIA_3
                start = GDV_SECOND * 1;
                stop = GDV_SECOND * 6;
                position = GDV_SECOND * 15;
        } else if (g_ascii_strncasecmp (url, MEDIA_4, strlen (url)) == 0) {
                // MEDIA_4
                start = GDV_SECOND * 1;
                stop = GDV_SECOND * 6;
                position = GDV_SECOND * 20;
        } else if (g_ascii_strncasecmp (url, MEDIA_5, strlen (url)) == 0) {
                // MEDIA_5
                start = GDV_SECOND * 0;
                stop = GDV_SECOND * 2.5;
                position = GDV_SECOND * 25;
        } else if (g_ascii_strncasecmp (url, MEDIA_6, strlen (url)) == 0) {
                // MEDIA_6
                start = GDV_SECOND * 0;
                stop = GDV_SECOND * 5;
                position = GDV_SECOND * 0;
        } else
                g_return_val_if_reached (FALSE);
        
        printf ("+ Adjusting the clip to: SOURCE: %s-%s TIMELINE: %s-%s\n",
                gdv_time_to_string (start), gdv_time_to_string (stop),
                gdv_time_to_string (position), gdv_time_to_string (position + stop - start));
        
        g_return_val_if_fail (gdv_clip_left_adjust_to (clip, start), FALSE);
        g_return_val_if_fail (gdv_clip_right_adjust_to (clip, stop), FALSE);
        g_return_val_if_fail (gdv_clip_move_to (clip, position), FALSE);

        printf ("+ Adding the clip to the track\n");
        g_return_val_if_fail (gdv_track_add_clip (Track, clip), FALSE);

        InspectedFiles++;

        if (InspectedFiles == InspectionTotal) {
                printf ("+ Setting pipeline to PLAYING\n");
                gst_element_set_state (GST_ELEMENT (Pipeline), GST_STATE_PAUSED);
        }
        
        return FALSE;
}

gboolean                        inspector_error_func (void)
{
        printf ("+ ERROR in inspecting. \n");
        exit (128);
        return FALSE;
}

gboolean                        build_pipeline (void)
{
        printf ("+ Building pipeline elements\n");
        
        Pipeline = (GstBin *) gst_pipeline_new (NULL);
        g_return_val_if_fail (Pipeline != NULL, FALSE);
        gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (Pipeline)),
                           (gpointer) bus_callback, NULL);

        ProjectFormat = gdv_projectformat_new ();

        Track = gdv_track_new (ProjectFormat, 1);
                
        AudioSink = gst_element_factory_make ("alsasink", NULL);
        g_return_val_if_fail (AudioSink != NULL, FALSE);
        
        VideoSink = gst_element_factory_make ("xvimagesink", NULL);
        g_return_val_if_fail (VideoSink != NULL, FALSE);
        
        VideoQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (VideoQueue != NULL, FALSE);

        gst_pad_add_event_probe (gst_element_get_pad (VideoQueue, "sink"), (GCallback) pad_event_probe, NULL);
        gst_pad_add_buffer_probe (gst_element_get_pad (VideoQueue, "src"), (GCallback) pad_buffer_probe, NULL);

        AudioQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (AudioQueue != NULL, FALSE);

        VideoCompSink = gdv_videocompsink_new ();
        g_return_val_if_fail (VideoCompSink != NULL, FALSE);
        gdv_videocompsink_set_format (VideoCompSink, ProjectFormat);

        AudioCompSink = gdv_audiocompsink_new ();
        g_return_val_if_fail (AudioCompSink != NULL, FALSE);
        gdv_audiocompsink_set_format (AudioCompSink, ProjectFormat);

        gst_bin_add_many (Pipeline,
                          GST_ELEMENT (Track),
                          AudioSink,
                          VideoSink,
                          VideoQueue,
                          AudioQueue,
                          VideoCompSink,
                          AudioCompSink,
                          NULL);
        
        // All fine
        return TRUE;
}

gboolean                        pad_buffer_probe (GstPad *pad, GstBuffer *buffer)
{
        CurrentTime = GST_BUFFER_TIMESTAMP (buffer);

        return TRUE;
}

gboolean                        space_pressed (void)
{
        GstState currentstate;
        GstState pendingstate;

        gst_element_get_state (GST_ELEMENT (Pipeline), &currentstate, &pendingstate, GST_CLOCK_TIME_NONE);

        if (pendingstate != GST_STATE_VOID_PENDING)
                currentstate = pendingstate;

        GstState newstate = (currentstate == GST_STATE_PLAYING) ? GST_STATE_PAUSED : GST_STATE_PLAYING;
        if (newstate == GST_STATE_PLAYING)
                printf ("+ PLAYING pipeline\n");
        else
                printf ("+ PAUSING pipeline\n");
        
        gst_element_set_state (GST_ELEMENT (Pipeline), newstate);

        return FALSE;
}

gboolean                        pad_event_probe (GstPad *pad, GstEvent *event)
{
        if (GST_EVENT_TYPE (event) == GST_EVENT_NAVIGATION) {
                const GstStructure *struc = gst_event_get_structure (event);
                
                const gchar *event = gst_structure_get_string (struc, "event");
                const gchar *key = gst_structure_get_string (struc, "key");

                if (strcmp (event, "key-press") == 0 && strcmp (key, "Left") == 0) {
                        
                        GstClockTime seektime;
                        if (CurrentTime >= GST_SECOND)
                                seektime = CurrentTime - GST_SECOND;
                        else
                                seektime = 0;

                        printf ("+ Seeking to %.2f\n", seektime / (float) GST_SECOND);
                        gst_element_seek (GST_ELEMENT (Pipeline), 1.0, GST_FORMAT_TIME,
                                  GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                                  GST_SEEK_TYPE_SET, (gint64) seektime, 
                                  GST_SEEK_TYPE_SET, -1);
//                        gst_element_seek (GST_ELEMENT (Track), 1.0, GST_FORMAT_TIME,
//                                 GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
//                                  GST_SEEK_TYPE_SET, (gint64) seektime, 
//                                  GST_SEEK_TYPE_SET, -1);
               } else if (strcmp (event, "key-press") == 0 && strcmp (key, "Right") == 0) {
                        
                        GstClockTime seektime = CurrentTime + GST_SECOND;
                        
                        printf ("+ Seeking to %.2f\n", seektime / (float) GST_SECOND);
                        gst_element_seek (GST_ELEMENT (Pipeline), 1.0, GST_FORMAT_TIME,
                                  GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                                  GST_SEEK_TYPE_SET, (gint64) seektime, 
                                  GST_SEEK_TYPE_SET, -1);
               } else if (strcmp (event, "key-press") == 0 && strcmp (key, "space") == 0) {
                       g_idle_add ((gpointer) space_pressed, NULL);
               }

        }

        return TRUE;
}

gboolean                        link_pipeline (void)
{
        printf ("+ Linking pipeline elements\n");

        g_return_val_if_fail (gst_element_link (GST_ELEMENT (Track), GST_ELEMENT (AudioCompSink)), FALSE);
        g_return_val_if_fail (gst_element_link (GST_ELEMENT (AudioCompSink), AudioQueue), FALSE);
        g_return_val_if_fail (gst_element_link (GST_ELEMENT (AudioQueue), AudioSink), FALSE);        

        g_return_val_if_fail (gst_element_link (GST_ELEMENT (Track), GST_ELEMENT (VideoCompSink)), FALSE);
        g_return_val_if_fail (gst_element_link (GST_ELEMENT (VideoCompSink), VideoQueue), FALSE);        
        g_return_val_if_fail (gst_element_link (VideoQueue, VideoSink), FALSE);
        
        // All fine
        return TRUE;
}

gboolean                        bus_callback (GstBus *bus, GstMessage *message, gpointer ptr)
{
        switch (GST_MESSAGE_TYPE (message)) {
                
                case GST_MESSAGE_ERROR: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_error (message, &err, &debug);
                        
                        g_error ("ERROR: (%s) (%s) %s (%s)",
                                 GST_OBJECT_NAME (GST_MESSAGE_SRC (message)),
                                 GST_OBJECT_NAME (GST_OBJECT_PARENT (GST_MESSAGE_SRC (message))),
                                 err->message, debug);
                        //gtk_main_quit ();
                        
                } break;
                        
                case GST_MESSAGE_WARNING: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_warning (message, &err, &debug);
                        
                        g_warning ("WARNING: %s (%s)", err->message, debug);
                        gtk_main_quit ();
                } break;
                        
                default:
                        break;
        };
                
        return TRUE;
}

