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
#include "ClipPlayer.h"
#include <gtk/gtk.h>

void                            show_banner (void)
{
        printf ("ClipPlayer will play a given timespan from a specified file.\n"
                "It's using GdvInspector and GdvDecodeBin with seek-in-ready block-mode thing..\n"
                "Use it to check files.\n\n");
}

void                            show_usage (void)
{
        printf ("Usage:\n"
                "   clipplayer <filename> <start> <stop>\n\n"
                "Start and stop values are expressed in seconds (as double)\n\n"
                "Example:\n"
                "   clipplayer media.dv 1 10\n\n");
}

/* Main */

int                             main (int argc, gchar **argv)
{
        // Initialize
        show_banner ();
        gtk_init (&argc, &argv);
        gst_init (&argc, &argv);

        if (argc <= 3 || argv == NULL || argv [1] == NULL || argv [2] == NULL || argv [3] == NULL) {
                show_usage ();
                return 128;
        }

        gchar *filename = argv [1];
        Start = g_ascii_strtod (argv [2], NULL) * GST_SECOND;
        Stop = g_ascii_strtod (argv [3], NULL) * GST_SECOND;

        if (Start >= Stop) {
                printf ("+ ERROR: Start >= Stop!\n");
                return 128;
        }

        printf ("+ Start: %s Stop: %s\n", gdv_time_to_string (Start), gdv_time_to_string (Stop));

        // Build the inspector
        printf ("+ Building and starting inspector for %s\n", filename);
        Inspector = gdv_inspector_new (filename);
        gdv_inspector_set_done_func (Inspector, inspector_done_func);
        gdv_inspector_set_error_func (Inspector, inspector_error_func);
        gdv_inspector_start (Inspector);

        printf ("+ Entering main loop\n");
        gtk_main ();

        return 0;
}

gboolean                        inspector_done_func (void)
{
        printf ("+ Inspecting done.\n");

        GdvTime len;
        g_object_get (G_OBJECT (Inspector), "length", &len, NULL);
        trim_values (len);

        printf ("+ Generating media item\n");
        GdvProjectFormat *format = gdv_projectformat_new ();
        g_return_val_if_fail (format != NULL, FALSE);
        
        MediaItem = gdv_itemgenerator_from_inspector (format, Inspector);
        g_return_val_if_fail (MediaItem != NULL, FALSE);

        printf ("+ Creating the clip\n");
        Clip = gdv_clip_new (MediaItem);
        g_return_val_if_fail (Clip != NULL, FALSE);

        printf ("+ Adjusting the clip\n");
        g_return_val_if_fail (gdv_clip_left_adjust_to (Clip, Start), FALSE);
        g_return_val_if_fail (gdv_clip_right_adjust_to (Clip, Stop), FALSE);
        g_return_val_if_fail (gdv_clip_move_to (Clip, GDV_ZEROTIME), FALSE);

        GdvTimeSpan *timelinespan = NULL;
        GdvTimeSpan *sourcespan = NULL;
        g_object_get (G_OBJECT (Clip), "timelinespan", &timelinespan, "sourcespan", &sourcespan, NULL);
        g_return_val_if_fail (timelinespan != NULL && sourcespan != NULL, FALSE);
        
        printf ("+ Timeline span: %s Source span: %s\n",
                gdv_timespan_to_string (timelinespan), gdv_timespan_to_string (sourcespan));
                        
        g_return_val_if_fail (build_pipeline (), FALSE);
        g_return_val_if_fail (link_pipeline (), FALSE);
        
        printf ("+ Setting pipeline to PLAYING\n");
        gst_element_set_state (GST_ELEMENT (Pipeline), GST_STATE_PLAYING);
        
        return FALSE;
}

void                            trim_values (GdvTime len)
{
        if (Start > len) {
                printf ("+ WARNING: Start value >= length. Resetting to 0\n");
                Start = 0;
        }

        if (Stop > len) {
                printf ("+ WARNING: Stop value >= length. Resetting to length\n");
                Stop = len;
        }
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

        g_object_get (G_OBJECT (Clip),
                      "hasvideo", &HasVideo,
                      "hasaudio", &HasAudio, NULL);
        
        gst_bin_add_many (Pipeline,
                          GST_ELEMENT (Clip),
                          NULL);

        if (HasAudio) {
                
                AudioConvert = gst_element_factory_make ("audioconvert", NULL);
                g_return_val_if_fail (AudioConvert != NULL, FALSE);

                AudioResample = gst_element_factory_make ("audioresample", NULL);
                g_return_val_if_fail (AudioResample != NULL, FALSE);

                AudioSink = gst_element_factory_make ("alsasink", NULL);
                g_return_val_if_fail (AudioSink != NULL, FALSE);

                AudioQueue = gst_element_factory_make ("queue", NULL);
                g_return_val_if_fail (AudioQueue != NULL, FALSE);
                
                gst_bin_add_many (Pipeline,
                                  AudioConvert,
                                  AudioResample,
                                  AudioSink,
                                  AudioQueue,
                                  NULL);
        }

        if (HasVideo) {

                VideoSink = gst_element_factory_make ("xvimagesink", NULL);
                g_return_val_if_fail (VideoSink != NULL, FALSE);

                VideoQueue = gst_element_factory_make ("queue", NULL);
                g_return_val_if_fail (VideoQueue != NULL, FALSE);

                gst_bin_add_many (Pipeline,
                                  VideoSink,
                                  VideoQueue, 
                                  NULL);

        }
        
        // All fine
        return TRUE;
}

gboolean                        link_pipeline (void)
{
        printf ("+ Linking pipeline elements\n");

        if (HasAudio) {
                g_return_val_if_fail (gst_element_link (GST_ELEMENT (Clip), AudioConvert), FALSE);
                g_return_val_if_fail (gst_element_link (AudioConvert, AudioResample), FALSE);
                g_return_val_if_fail (gst_element_link (AudioResample, AudioQueue), FALSE);
                g_return_val_if_fail (gst_element_link (AudioQueue, AudioSink), FALSE);
        }

        if (HasVideo) {
                g_return_val_if_fail (gst_element_link (GST_ELEMENT (Clip), VideoQueue), FALSE);
                g_return_val_if_fail (gst_element_link (VideoQueue, VideoSink), FALSE);
        }
        
        // All fine
        return TRUE;
}

gboolean                        bus_callback (GstBus *bus, GstMessage *message, gpointer ptr)
{
        //printf ("MSG: %s\n", GST_MESSAGE_TYPE_NAME (message));
        
        switch (GST_MESSAGE_TYPE (message)) {
                
                case GST_MESSAGE_ERROR: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_error (message, &err, &debug);
                        
                        printf ("ERROR: %s (%s)", err->message, debug);
                        gtk_main_quit ();
                        
                } break;
                        
                case GST_MESSAGE_WARNING: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_warning (message, &err, &debug);
                        
                        printf ("WARNING: %s (%s)", err->message, debug);
                        gtk_main_quit ();
                } break;
                        
                default:
                        break;
        };
                
        return TRUE;
}

