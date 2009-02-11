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
#include "SourcePlayer.h"
#include <gtk/gtk.h>

void                            show_banner (void)
{
        printf ("SourcePlayer will play a given timespan from a specified file.\n"
                "It's using GdvInspector and GdvDecodeBin with seek-in-ready block-mode thing..\n"
                "Use it to check files.\n\n");
}

void                            show_usage (void)
{
        printf ("Usage:\n"
                "   sourceplayer <filename> <start> <stop>\n\n"
                "Start and stop values are expressed in seconds (as double)\n\n"
                "Example:\n"
                "   sourceplayer media.dv 1 10\n\n");
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

        g_object_get (G_OBJECT (Inspector), "hasaudio", &HasAudio, "hasvideo", &HasVideo, NULL);
        printf ("+ Has audio: %d Has video: %d\n", HasAudio, HasVideo);

        GdvVideoFormat *vformat;
        GdvAudioFormat *aformat;
        gchar *url = NULL;
        GdvTime len;

        g_return_val_if_fail (HasVideo == TRUE || HasAudio == TRUE, FALSE);

        printf ("+ Reading formats\n");
        if (HasVideo) {
                g_object_get (G_OBJECT (Inspector), "videoformat", &vformat, NULL);
                g_return_val_if_fail (vformat != NULL, FALSE);
        } else
                vformat = NULL;

        if (HasAudio) {
                g_object_get (G_OBJECT (Inspector), "audioformat", &aformat, NULL);
                g_return_val_if_fail (aformat != NULL, FALSE);
        } else
                aformat = NULL;

        // Get the url
        g_object_get (G_OBJECT (Inspector), "url", &url, "length", &len, NULL);
        g_return_val_if_fail (url != NULL, FALSE);
        g_return_val_if_fail (len > 0, FALSE);

        g_return_val_if_fail (build_pipeline (vformat, aformat, url, len), FALSE);
        g_return_val_if_fail (link_pipeline (HasAudio, HasVideo), FALSE);
        trim_values (len);
        
        printf ("+ Setting pipeline to READY\n");
        gst_element_set_state (GST_ELEMENT (Pipeline), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (Pipeline), NULL, NULL, GST_CLOCK_TIME_NONE);

        printf ("+ Seeking (in READY) the GdvDecodeBin %s-%s\n",
                gdv_time_to_string (Start), gdv_time_to_string (Stop));
        
        g_return_val_if_fail (seek_pipeline (Start, Stop), FALSE);

        printf ("+ Setting pipeline to PLAYING\n");
        gst_element_set_state (GST_ELEMENT (Pipeline), GST_STATE_PLAYING);
        
        return FALSE;
}

gboolean                        seek_pipeline (GdvTime Start, GdvTime Stop)
{
        return gst_element_seek (GST_ELEMENT (DecodeBin), 1.0, GST_FORMAT_TIME,
                                 GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                                 GST_SEEK_TYPE_SET, Start,
                                 GST_SEEK_TYPE_SET, Stop);
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

gboolean                        build_pipeline (GdvVideoFormat *vformat, GdvAudioFormat *aformat, gchar *url, GdvTime len)
{
        printf ("+ Building pipeline elements for %s\n", url);
        
        Pipeline = (GstBin *) gst_pipeline_new (NULL);
        g_return_val_if_fail (Pipeline != NULL, FALSE);
        gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (Pipeline)),
                           (gpointer) bus_callback, NULL);
        
        DecodeBin = gdv_decodebin_new (url, vformat, aformat, len);

        gst_bin_add_many (Pipeline,
                          GST_ELEMENT (DecodeBin),
                          NULL);

        if (aformat != NULL) {

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

        if (vformat != NULL) {

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

gboolean                        link_pipeline (gboolean hasaudio, gboolean hasvideo)
{
        printf ("+ Linking pipeline elements\n");

        if (hasaudio) {
                g_return_val_if_fail (gst_element_link (GST_ELEMENT (DecodeBin), AudioConvert), FALSE);
                g_return_val_if_fail (gst_element_link (AudioConvert, AudioResample), FALSE);
                g_return_val_if_fail (gst_element_link (AudioResample, AudioQueue), FALSE);
                g_return_val_if_fail (gst_element_link (AudioQueue, AudioSink), FALSE);
        }

        if (hasvideo) {
                g_return_val_if_fail (gst_element_link (GST_ELEMENT (DecodeBin), VideoQueue), FALSE);
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

