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

#include <gst/gst.h>
#include "SeekPlayer.h"
#include <gtk/gtk.h>

/* Main */

void                            show_banner (void)
{
        printf ("SeekPlayer will play 1s - 3s segment from a given media file.\n"
                "It uses a block-seek technique to do the seek before playback starts.\n"
                "Use it to check files.\n\n");
}

void                            show_usage (void)
{
        printf ("Usage:\n"
                "   seekplayer <filename>\n\n");
}

gboolean                        build_pipeline (gchar *file)
{
        printf ("+ Building pipeline elements for %s\n", file);
        
        Pipeline = (GstBin *) gst_pipeline_new (NULL);
        g_return_val_if_fail (Pipeline != NULL, FALSE);
        gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (Pipeline)),
                           (gpointer) bus_callback, NULL);

        FileSrc = gst_element_factory_make ("filesrc", NULL);
        g_return_val_if_fail (FileSrc != NULL, FALSE);
        g_object_set (G_OBJECT (FileSrc), "location", file, NULL);

        DecodeBin = gst_element_factory_make ("decodebin", NULL);
        g_return_val_if_fail (DecodeBin != NULL, FALSE);
        g_signal_connect (G_OBJECT (DecodeBin), "pad-added",
                          G_CALLBACK (new_pad), DecodeBin);
        g_signal_connect (G_OBJECT (DecodeBin), "no-more-pads",
                          G_CALLBACK (no_more_pads), DecodeBin);

        AudioConvert = gst_element_factory_make ("audioconvert", NULL);
        g_return_val_if_fail (AudioConvert != NULL, FALSE);

        AudioResample = gst_element_factory_make ("audioresample", NULL);
        g_return_val_if_fail (AudioResample != NULL, FALSE);

        AudioSink = gst_element_factory_make ("alsasink", NULL);
        g_return_val_if_fail (AudioSink != NULL, FALSE);

        VideoSink = gst_element_factory_make ("xvimagesink", NULL);
        g_return_val_if_fail (VideoSink != NULL, FALSE);

        AudioQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (AudioQueue != NULL, FALSE);

        VideoQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (VideoQueue != NULL, FALSE);

        VIdentity = gst_element_factory_make ("identity", NULL);
        g_return_val_if_fail (VIdentity != NULL, FALSE);
        g_signal_connect (G_OBJECT (VIdentity), "handoff",
                          G_CALLBACK (handoff), NULL);

        AIdentity = gst_element_factory_make ("identity", NULL);
        g_return_val_if_fail (AIdentity != NULL, FALSE);
        g_signal_connect (G_OBJECT (AIdentity), "handoff",
                          G_CALLBACK (handoff), NULL);

        gst_bin_add_many (Pipeline,
                          FileSrc, DecodeBin,
                          AudioConvert, AudioResample,
                          AudioSink, VideoSink,
                          AudioQueue, VideoQueue,
                          AIdentity, VIdentity,
                          NULL);

        
        
        // All fine
        return TRUE;
}

gboolean                        link_pipeline (void)
{
        printf ("+ Linking initial pipeline elements\n");

        g_return_val_if_fail (gst_element_link (FileSrc, DecodeBin), FALSE);
        g_return_val_if_fail (gst_element_link (AudioConvert, AudioResample), FALSE);
        g_return_val_if_fail (gst_element_link (AudioResample, AudioQueue), FALSE);
        g_return_val_if_fail (gst_element_link (AudioQueue, AIdentity), FALSE);
        g_return_val_if_fail (gst_element_link (AIdentity, AudioSink), FALSE);
        g_return_val_if_fail (gst_element_link (VideoQueue, VIdentity), FALSE);
        g_return_val_if_fail (gst_element_link (VIdentity, VideoSink), FALSE);
                
        // All fine
        return TRUE;
}

int                             main (int argc, gchar **argv)
{
        // Initialize
        show_banner ();
        gtk_init (&argc, &argv);
        gst_init (&argc, &argv);

        if (argc <= 1 || argv == NULL || argv [1] == NULL) {
                show_usage ();
                return 128;
        }

        gchar *filename = argv [1];

        HasVideo = FALSE;
        HasAudio = FALSE;
        MinVBuffer = G_MAXUINT64;
        MinABuffer = G_MAXUINT64;
        MaxVBuffer = 0;
        MaxABuffer = 0;

        // Start the gst fun
        g_return_val_if_fail (build_pipeline (filename) != FALSE, 128);
        g_return_val_if_fail (link_pipeline () != FALSE, 128);
        g_return_val_if_fail (block_initial () != FALSE, 128);

        // Playback start
        printf ("+ Setting pipeline to PLAYING\n");
        gst_element_set_state (GST_ELEMENT (Pipeline), GST_STATE_PLAYING);

        printf ("+ Entering main loop\n");
        g_timeout_add (3000, timeout, NULL);
        gtk_main ();

        printf ("+ Left main loop, setting pipeline to NULL\n");
        gst_element_set_state (GST_ELEMENT (Pipeline), GST_STATE_NULL);
        gst_element_get_state (GST_ELEMENT (Pipeline), NULL, NULL, GST_CLOCK_TIME_NONE);

        printf ("\n");
        show_buffer_results ();
        printf ("\n");
        
        return 0;
}

void                            new_pad (GstElement *element, GstPad *pad, gpointer ptr)
{
        printf ("+ new-pad %s\n", GST_PAD_NAME (pad));

        GstCaps *caps;
        GstStructure *str;

        caps = gst_pad_get_caps (pad);
        g_assert (caps != NULL);
        
        str = gst_caps_get_structure (caps, 0);
        g_assert (str != NULL);

        if (g_strrstr (gst_structure_get_name (str), "video")) {
                HasVideo = TRUE;
                printf ("+ Linking %s to video queue\n", GST_PAD_NAME (pad));
                gst_pad_link (pad, gst_element_get_pad (VideoQueue, "sink"));
        }

        if (g_strrstr (gst_structure_get_name (str), "audio")) {
                HasAudio = TRUE;
                printf ("+ Linking %s to audio convert\n", GST_PAD_NAME (pad));
                gst_pad_link (pad, gst_element_get_pad (AudioConvert, "sink"));
        }
        
        gst_caps_unref (caps);
}

gboolean                        block_initial (void)
{
        printf ("+ Initially blocking pads\n");

        GstPad *vpad = gst_element_get_pad (VideoQueue, "src");
        GstPad *apad = gst_element_get_pad (AudioConvert, "src");

        g_return_val_if_fail (vpad != NULL && apad != NULL, FALSE);
        
        g_return_val_if_fail (gst_pad_set_blocked_async (vpad, TRUE,
                                                         (GstPadBlockCallback) block, new_action (vpad, VideoQueue)),
                              FALSE);

        g_return_val_if_fail (gst_pad_set_blocked_async (apad, TRUE,
                                                         (GstPadBlockCallback) block, new_action (apad, AudioConvert)),
                              FALSE);

        return TRUE;
}

void                            block (GstPad *pad, gboolean blocked, UnblockAction *action)
{
        ACTION_LOCK (action);

        if (blocked == TRUE && action->KillSwitch == FALSE) {
                printf ("+ %s:%s pad BLOCKED\n", GST_PAD_NAME (action->Pad), GST_ELEMENT_NAME (action->Element));
                g_idle_add ((GSourceFunc) idle_unblock_action, action);
                action->KillSwitch = TRUE;
        } else if (blocked == TRUE && action->KillSwitch == TRUE)
                printf ("+ %s:%s pad BLOCKED [ignoring]\n", GST_PAD_NAME (action->Pad), GST_ELEMENT_NAME (action->Element));
        else if (blocked == FALSE)
                printf ("+ %s:%s pad UNBLOCKED\n", GST_PAD_NAME (action->Pad), GST_ELEMENT_NAME (action->Element));
        else
                g_assert_not_reached ();

        ACTION_UNLOCK (action);
}

gboolean                        idle_unblock_action (UnblockAction *action)
{
        ACTION_LOCK (action);
        printf ("+ Idle action for %s:%s\n", GST_PAD_NAME (action->Pad), GST_ELEMENT_NAME (action->Element));

        printf ("+ Seeking %s:%s\n", GST_PAD_NAME (action->Pad), GST_ELEMENT_NAME (action->Element));

        GstEvent *seek = gst_event_new_seek (1.0, GST_FORMAT_TIME,
                                             GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                                             GST_SEEK_TYPE_SET, GST_SECOND * 1,
                                             GST_SEEK_TYPE_SET, GST_SECOND * 3);
        
        g_return_val_if_fail (seek != NULL, FALSE);

        gst_pad_send_event (action->Pad, seek);

        printf ("+ Unblocking %s:%s\n", GST_PAD_NAME (action->Pad), GST_ELEMENT_NAME (action->Element));
        
        g_return_val_if_fail (gst_pad_set_blocked_async (action->Pad, FALSE, (GstPadBlockCallback) block, action),
                              FALSE);

        ACTION_UNLOCK (action);
        
        return FALSE;
}

UnblockAction*                  new_action (GstPad *pad, GstElement *element)
{
        UnblockAction *new = g_new (UnblockAction, 1);
        new->Mutex = g_mutex_new ();
        new->Element = element;
        new->Pad = pad;
        new->KillSwitch = FALSE;

        return new;
}

void                            no_more_pads (GstElement *element, gpointer ptr)
{
        printf ("+ No more pads\n");

        if (HasVideo == FALSE) {
                printf ("+ No video, adding stock videotestsrc\n");
                GstElement *element = gst_element_factory_make ("videotestsrc", NULL);
                g_return_if_fail (element != NULL);

                gst_element_set_locked_state (element, TRUE);
                gst_bin_add (Pipeline, element);
                
                g_return_if_fail (gst_element_link (element, VideoQueue));
                
                gst_element_set_locked_state (element, FALSE);
                gst_element_set_state (element, GST_STATE_PLAYING);

                HasVideo = TRUE;
        }

        if (HasAudio == FALSE) {
                printf ("+ No audio, adding stock audiotestsrc\n");
                GstElement *element = gst_element_factory_make ("audiotestsrc", NULL);
                g_return_if_fail (element != NULL);

                gst_element_set_locked_state (element, TRUE);
                gst_bin_add (Pipeline, element);
                
                g_return_if_fail (gst_element_link (element, AudioConvert));
                
                gst_element_set_locked_state (element, FALSE);
                gst_element_set_state (element, GST_STATE_PLAYING);

                HasAudio = TRUE;
        }
}

gboolean                        bus_callback (GstBus *bus, GstMessage *message, gpointer ptr)
{
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

gboolean                        timeout (gpointer ptr)
{
        printf ("+ 3 secs passed, quitting\n");
        gtk_main_quit ();
        
        return FALSE;
}

void                            handoff (GstElement *element, GstBuffer *buffer, gpointer ptr)
{
        if (element == VIdentity) {
                MinVBuffer = MIN (MinVBuffer, GST_BUFFER_TIMESTAMP (buffer));
                MaxVBuffer = MAX (MaxVBuffer, GST_BUFFER_TIMESTAMP (buffer) + GST_BUFFER_DURATION (buffer));
        } else {
                MinABuffer = MIN (MinABuffer, GST_BUFFER_TIMESTAMP (buffer));
                MaxABuffer = MAX (MaxABuffer, GST_BUFFER_TIMESTAMP (buffer) + GST_BUFFER_DURATION (buffer));
        }
}

void                            show_buffer_results ()
{
        printf ("Min audio buffer received: ");
        if (MinABuffer == G_MAXUINT64)
                printf ("N/A\n");
        else
                printf ("%.2f (%lld ns)\n", (float) MinABuffer / (float) GST_SECOND,
                        MinABuffer);

        printf ("Max audio buffer received: ");
        if (MaxABuffer == 0)
                printf ("N/A\n");
        else
                printf ("%.2f (%lld ns)\n", (float) MaxABuffer / (float) GST_SECOND,
                        MaxABuffer);

        printf ("Min video buffer received: ");
        if (MinVBuffer == G_MAXUINT64)
                printf ("N/A\n");
        else
                printf ("%.2f (%lld ns)\n", (float) MinVBuffer / (float) GST_SECOND,
                        MinVBuffer);

        printf ("Max video buffer received: ");
        if (MaxVBuffer == 0)
                printf ("N/A\n");
        else
                printf ("%.2f (%lld ns)\n", (float) MaxVBuffer / (float) GST_SECOND,
                        MaxVBuffer);
}


        
