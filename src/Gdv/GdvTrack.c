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

#include "GdvTrack.h"
#include "GdvTrackPrivate.h"

static GstElementClass*         ParentClass = NULL;

/* GObject Type */
GType                           gdv_track_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvTrackClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_track_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvTrack),
                        4,
                        (GInstanceInitFunc) gdv_track_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvTrack", &objectinfo, 0);
        }

        return objecttype;
}

/* GObject init */
void                            gdv_track_class_init (GdvTrackClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) klass;
        
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
        gobjectclass->get_property = (gpointer) (gdv_track_get_property);
        gobjectclass->set_property = (gpointer) (gdv_track_set_property);

        g_object_class_install_property (gobjectclass, ARG_CLIPSTORE,
                                         g_param_spec_object
                                         ("clipstore", "ClipStore",
                                          "ClipStore used by this track",
                                          GDV_TYPE_CLIPSTORE,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_PROJECTFORMAT,
                                         g_param_spec_object
                                         ("projectformat", "ProjectFormat",
                                          "Out project format",
                                          GDV_TYPE_PROJECTFORMAT,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_CLIPS,
                                         g_param_spec_pointer
                                         ("clips", "Clips", "The list of all the clips",
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_LAYER,
                                         g_param_spec_int
                                         ("layer", "Layer", "The layer of this track", 0, 100, 0, 
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_OPACITY,
                                         g_param_spec_double
                                         ("opacity", "Opacity", "The opacity of this track", -1, 1, 1, 
                                          G_PARAM_READWRITE));
        
        gobjectclass->dispose = (gpointer) gdv_track_dispose;
        gobjectclass->finalize = (gpointer) gdv_track_finalize;
        
        elementclass->send_event = (gpointer) send_event;
        elementclass->change_state = (gpointer) change_state;
        
        // Signals
        klass->ReseekNeededSignalId =
                g_signal_newv ("reseek-needed",
                               GDV_TYPE_TRACK,
                               G_SIGNAL_RUN_LAST,
                               NULL,
                               NULL,
                               NULL,
                               g_cclosure_marshal_VOID__VOID,
                               G_TYPE_NONE,
                               0,
                               NULL);
}

/* GObject init */
static void                     gdv_track_init (GdvTrack *this)
{
        GDV_DEBUG_ELEMENT ("Initializing fresh GdvTrack object", NULL);
                        
        g_return_if_fail (GDV_IS_TRACK (this));
        this->Private = NULL;

        GdvTrackPrivate *priv = g_new0 (GdvTrackPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
                this->Private = NULL;
        }
        
}

/* Initialize the private struct. Here we do some hard work */
static gboolean                 private_init (GdvTrack *this, GdvTrackPrivate *private)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);
                
        // The Bus handlig stuff
        GDV_DEBUG_ELEMENT ("Setting the bus sync handler", NULL);
        GstBus *bus = GST_BIN (this)->child_bus;
        g_return_val_if_fail (this != NULL, FALSE);
        
        GST_OBJECT_LOCK (bus);
        
        private->ParentSyncHandler = bus->sync_handler;
        private->ParentSyncHandlerData = bus->sync_handler_data;
        
        bus->sync_handler = (GstBusSyncHandler) sync_handler;
        bus->sync_handler_data = (gpointer) this;

        GST_OBJECT_UNLOCK (bus);
        
        // The clips list
        GDV_DEBUG_ELEMENT ("Preparing the clips list", NULL);
        private->ClipStore = gdv_clipstore_new ();
        g_return_val_if_fail (private->ClipStore != NULL, FALSE);

        // VCorrector
        private->VCorrector = (GstElement *) gdv_corrector_new ();
        g_return_val_if_fail (private->VCorrector != NULL, FALSE);
        gst_bin_add (GST_BIN (this), private->VCorrector);

        // ACorrector
        private->ACorrector = (GstElement *) gdv_corrector_new (); 
        g_return_val_if_fail (private->ACorrector != NULL, FALSE);
        gst_bin_add (GST_BIN (this), private->ACorrector);
        
        // Audio elements
        private->AudioConvert = gst_element_factory_make ("audioconvert", NULL);
        g_return_val_if_fail (private->AudioConvert != NULL, FALSE);
        gst_bin_add (GST_BIN (this), private->AudioConvert);
        
        private->AudioResample = gst_element_factory_make ("audioresample", NULL);
        g_return_val_if_fail (private->AudioResample != NULL, FALSE);
        gst_bin_add (GST_BIN (this), private->AudioResample);

        // Audio queue
        private->AQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (private->AQueue != NULL, FALSE);
        gst_bin_add (GST_BIN (this), private->AQueue);
 
        g_return_val_if_fail (gst_element_link (GST_ELEMENT (private->ACorrector), private->AudioConvert), FALSE);
        g_return_val_if_fail (gst_element_link (private->AudioConvert, private->AudioResample), FALSE);
        g_return_val_if_fail (gst_element_link (private->AudioResample, private->AQueue), FALSE);
        
        // The pads
        GDV_DEBUG_ELEMENT ("Preparing the ghost pads", NULL);
        GstPad *vpad = gst_element_get_pad (GST_ELEMENT (private->VCorrector), 
                                            "src");
        GstPad *apad = gst_element_get_pad (GST_ELEMENT (private->AQueue), 
                                            "src");
        g_return_val_if_fail (apad != NULL && vpad != NULL, FALSE);

        private->VSrc = gst_ghost_pad_new ("vsrc", vpad);
        private->ASrc = gst_ghost_pad_new ("asrc", apad);

        gst_object_unref (apad);
        gst_object_unref (vpad);
        
        g_return_val_if_fail (private->ASrc != NULL, FALSE);
        g_return_val_if_fail (private->VSrc != NULL, FALSE);
        gst_element_add_pad (GST_ELEMENT (this), (GstPad *) private->VSrc);
        gst_element_add_pad (GST_ELEMENT (this), (GstPad *) private->ASrc);

        // Add the probes
        // FIXME: Actually, the probes should be on the sink of the first 
        // input element here. 
        gst_pad_add_event_probe (vpad, (GCallback) src_event_probe, this);
        gst_pad_add_event_probe (apad, (GCallback) src_event_probe, this);

        // Some more...
        private->CurrentVElement = NULL;
        private->CurrentAElement = NULL;
        private->FirstDone = FALSE;

        private->ProjectFormat = NULL;
        private->Layer = 0;
        private->NextCutAt = GDV_INFINITY;
        private->SavedSeek = GDV_ZEROTIME;

        private->Tower = gdv_idletower_new ();

        private->Clips = NULL;
        
        // Mutex
        private->SegmentMutex = g_mutex_new ();
        g_return_val_if_fail (private->SegmentMutex != NULL, FALSE);

        return TRUE;
}

static void                     spawn_sheeps (GdvTrack *this, GdvProjectFormat *format)
{
        // Our blackness element
        GDV_DEBUG_ELEMENT ("Preparing the Blackness element", NULL);
        this->Private->Blackness = GST_ELEMENT (gdv_blacksheep_new (format));
        g_return_if_fail (this->Private->Blackness != NULL);
        gst_bin_add (GST_BIN (this), this->Private->Blackness);
        GDV_DEBUG_ELEMENT ("Linking Blackness initially", NULL);
        g_return_if_fail (gst_element_link_pads (this->Private->Blackness, "src",
                                                     GST_ELEMENT (this->Private->VCorrector), "sink"));
        
        // Our silence element
        GDV_DEBUG_ELEMENT ("Preparing the Silence element", NULL);
        this->Private->Silence = GST_ELEMENT (gdv_silentsheep_new (format));
        g_return_if_fail (this->Private->Silence != NULL);
        gst_bin_add (GST_BIN (this), this->Private->Silence);
        GDV_DEBUG_ELEMENT ("Linking Silence initially", NULL);
        g_return_if_fail (gst_element_link_pads (this->Private->Silence, "src",
                                                     GST_ELEMENT (this->Private->ACorrector), "sink"));

        this->Private->CurrentVElement = this->Private->Blackness;
        this->Private->CurrentAElement = this->Private->Silence;
}

/* Dispose the private struct */
static void                     private_dispose (GdvTrack *this, GdvTrackPrivate *private)
{
        g_return_if_fail (private != NULL);

        if (private->ClipStore != NULL) {
                g_object_unref (G_OBJECT (private->ClipStore));
                private->ClipStore = NULL;
        }

        // FIXME: Implement
}

/* GObject get property */
static void                     gdv_track_get_property (GdvTrack *this, guint propid,
                                                        GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_TRACK (this));
        g_return_if_fail (this->Private != NULL);
        
        switch (propid) {

                case ARG_CLIPSTORE:
                        g_value_set_object (value, this->Private->ClipStore);
                        break;

                case ARG_PROJECTFORMAT:
                        g_value_set_object (value, this->Private->ProjectFormat);
                        break;
                        
                case ARG_LAYER:
                        g_value_set_int (value, this->Private->Layer);
                        break;

                case ARG_OPACITY:
                        g_value_set_double (value, 1.0);
                        break;

                case ARG_CLIPS: {
                        GList *list = NULL;

                        // Let's copy all the elements to the new list
                        GSList *iterator;

                        for (iterator = this->Private->Clips; iterator; iterator = g_slist_next (iterator)) {
                                ClipData *data = (ClipData *) iterator->data;
                                g_assert (data != NULL);
                                g_assert (data->Clip != NULL);
                                
                                list = g_list_append (list, data->Clip);
                        }
                        
                        g_value_set_pointer (value, list);
                        
                } break;
                        
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* Bus sync handler that handles pipeline updates etc. */
static GstBusSyncReply          sync_handler  (GstBus *bus, GstMessage *message, 
                                               GdvTrack *this)
{
        GDV_DEBUG_ELEMENT ("Entering the sync_handler bus:%s message:%s", 
                           GST_OBJECT_NAME (this), gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
          
        GstBusSyncReply reply = GST_BUS_DROP;

        switch (GST_MESSAGE_TYPE (message)) {

                case GST_MESSAGE_SEGMENT_DONE: {

                        GstFormat format;
                        gint64 time;
                        gst_message_parse_segment_done (message, &format, &time);
                        
                        if (format != GST_FORMAT_TIME) {
                                // The message is in a non-time format. Let's try converting it.
                                GDV_DEBUG_ELEMENT ("Trying to convert a message to time", NULL);
                                GstFormat destformat = GST_FORMAT_TIME;
                                gint64 destval;
                                gst_element_query_convert (GST_ELEMENT (GST_MESSAGE_SRC (message)),
                                                           format, time,
                                                           &destformat, &destval);
                                if (destformat != GST_FORMAT_TIME) {
                                        GDV_WARNING_ELEMENT ("Can't convert message to time", NULL);
                                        return GST_BUS_DROP;
                                } else
                                        time = destval;
                        }
                        
                        g_mutex_lock (this->Private->SegmentMutex);
                        
                        GDV_DEBUG_ELEMENT ("Got SEGMENT_DONE at %s",
                                           gdv_time_to_string ((guint64) time));
                        
                        if (this->Private->CurrentAElement == this->Private->CurrentVElement ||
                            this->Private->FirstDone) {

                                if (gdv_idletower_has_gid (this->Private->Tower, SEEK_ACTION_GID))
                                        GDV_DEBUG_ELEMENT ("Seek action gided, ignoring\n", NULL);
                                else {
                                        GDV_DEBUG_ELEMENT ("Segment done two, rebuilding", NULL);
                                        g_assert (this->Private->NextCutAt != GDV_INFINITY);

                                        SegmentDoneAction *action = g_new (SegmentDoneAction, 1);
                                        action->Track = this;
                                        action->Time = this->Private->NextCutAt;

                                        gdv_idletower_add_item (this->Private->Tower, SEGMENT_DONE_ACTION_GID, 
                                                                (gpointer) segment_done_action,
                                                                (gpointer) segment_done_action_dispose, 
                                                                NULL, action);
                                }
                                this->Private->FirstDone = FALSE;
                        } else {
                                GDV_DEBUG_ELEMENT ("Segment done one", NULL);
                                this->Private->FirstDone = TRUE;
                        }
                        g_mutex_unlock (this->Private->SegmentMutex);
                } break;
                        
                default: {
                        if (this->Private->ParentSyncHandler != NULL) 
                                reply = this->Private->ParentSyncHandler (bus, 
                                                                          message, 
                                                                          this->Private->ParentSyncHandlerData);
                } break;
        }
 
        // Finish
        return reply;
}

/* Add a clip to this track */
gboolean                        gdv_track_add_clip (GdvTrack *this, GdvClip *clip)
{
        // FIXME: Fail if we're playing
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                
        GDV_DEBUG_ELEMENT ("Adding clip '%s' to the track", GST_OBJECT_NAME (clip));
        
        // Check if clip area if okay with the current clips
        GdvTimeSpan *timespan = NULL;
        g_object_get (G_OBJECT (clip), "timelinespan", &timespan, NULL);
        if (timespan == NULL) {
                GDV_WARNING_ELEMENT ("Failed to obtain timespan of clip", NULL);
                goto error;
        }
        
        if (! gdv_clipstore_is_span_empty (this->Private->ClipStore, timespan)) {
                GDV_WARNING_ELEMENT ("Track area %s not empty", 
                                     gdv_timespan_to_string (timespan));
                goto error;
        }
        
        gdv_timespan_free (timespan);
        if (! gdv_clipstore_add_clip (this->Private->ClipStore, clip))
                goto error;

        // Make sure we're in null
        gst_element_set_locked_state (GST_ELEMENT (clip), TRUE);
        gst_element_set_state (GST_ELEMENT (clip), GST_STATE_NULL);
        gst_element_get_state (GST_ELEMENT (clip), NULL, NULL, GST_CLOCK_TIME_NONE);

        gst_bin_add (GST_BIN (this), GST_ELEMENT (clip));
        gst_object_ref (clip);

        // Clip data
        ClipData *data = g_new (ClipData, 1);
        data->Clip = clip;
        data->ChangedSignal = g_signal_connect (G_OBJECT (clip), "changed",
                                                (GCallback) on_clip_changed,
                                                this);
        
        this->Private->Clips = g_slist_append (this->Private->Clips, data);
        
        // Force the clip-change emission
        // This also immidiatelly forces the reseek-needed signal
        g_debug ("Forcing changed signal");
        gdv_clip_force_changed_signal (clip);

        return TRUE;
        
        // ERROR
        error:
        g_return_val_if_reached (FALSE);
}

/* Add a clip to this track */
gboolean                        gdv_track_remove_clip (GdvTrack *this, GdvClip *clip)
{
        // FIXME: Fail if we're playing
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
        
        GDV_DEBUG_ELEMENT ("Removing clip '%s' from the track", GST_OBJECT_NAME (clip));

        g_mutex_lock (this->Private->SegmentMutex);
        
        if (! gdv_clipstore_remove_clip (this->Private->ClipStore, clip)) {
                g_mutex_unlock (this->Private->SegmentMutex);
                goto error;
        }

        GSList *element = g_slist_find_custom (this->Private->Clips, clip, (GCompareFunc) clip_find_func);
        g_assert (element != NULL);
        
        ClipData *data = element->data;

        if (data->ChangedSignal != 0)
                g_signal_handler_disconnect (data->Clip, data->ChangedSignal);

        this->Private->Clips = g_slist_remove (this->Private->Clips, data);
 
        // If we're not playing that clip, we can safely remove it here
        if (this->Private->CurrentVElement != GST_ELEMENT (clip) &&
            this->Private->CurrentAElement != GST_ELEMENT (clip)) {
                gst_element_set_locked_state (GST_ELEMENT (clip), TRUE);
                gst_element_set_state (GST_ELEMENT (clip), GST_STATE_NULL);
                gst_element_get_state (GST_ELEMENT (clip), NULL, NULL, GST_CLOCK_TIME_NONE);

                gst_bin_remove (GST_BIN (this), GST_ELEMENT (clip));
        } else {
                g_signal_emit (this, GDV_TRACK_GET_CLASS (this)->ReseekNeededSignalId,
                               0, NULL);

                // At this point we've got a seek schedlued...
                // We can add an ops to remove it actually from the shit
                PostRemoveAction *action = g_new (PostRemoveAction, 1);
                action->Track = this;
                action->Clip = clip;
                gdv_idletower_add_item (this->Private->Tower, POST_REMOVE_ACTION_GID, 
                                        (gpointer) post_remove_action,
                                        (gpointer) post_remove_action_dispose, 
                                        NULL, action);
        }
      
        g_mutex_unlock (this->Private->SegmentMutex);
        
        // Force the clip-change emission
        gdv_clip_force_changed_signal (clip);

        return TRUE;
        
        // ERROR
        error:
        g_return_val_if_reached (FALSE);
}

/* Create a new instance */
GdvTrack*                       gdv_track_new (GdvProjectFormat *format, gint32 layer)
{
        GdvTrack *this = g_object_new (GDV_TYPE_TRACK, NULL);
        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                gst_object_unref (this);
                return NULL;
        }
        
        spawn_sheeps (this, format);

        this->Private->ProjectFormat = format;
        g_object_ref (format);
        this->Private->Layer = layer;

        // FIXME: Should go into GObject property
        // gdv_bumperstamper_set_layer (this->Private->VBumper, layer);
        
        return this;
}

/* A GStreamer send_event. We silently ignore all events expect for seek */
gboolean                        send_event (GdvTrack *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (event != NULL, FALSE);
        
        gboolean ret = TRUE;

        switch (GST_EVENT_TYPE (event)) {

                case GST_EVENT_SEEK: {
                        // Parse the event
                        gdouble rate;
                        GstSeekFlags flags;
                        GstFormat format;
                        GstSeekType curtype, stoptype;
                        gint64 cur, stop;
                        gst_event_parse_seek (event, &rate, &format, &flags,
                                              &curtype, &cur, &stoptype, &stop);
                        if (curtype != GST_SEEK_TYPE_SET) {
                                GDV_WARNING_ELEMENT ("Can't handle non-set seek type", NULL);
                                ret = FALSE;
                                goto done;
                        }

                        if (format != GST_FORMAT_TIME) {
                                GDV_WARNING_ELEMENT ("Can't handle non-time seek type", NULL);
                                ret = FALSE;
                                goto done;
                        }
                        
                        GDV_DEBUG_ELEMENT ("Seeking to %s", gdv_time_to_string (cur));

                        GstState state;
                        gst_element_get_state (GST_ELEMENT (this), 
                                               &state, NULL, GST_CLOCK_TIME_NONE);
                        
                        if (state >= GST_STATE_PAUSED) {
                                SeekAction *action = g_new (SeekAction, 1);
                                action->Track = this;
                                action->Time = (guint64) cur;

                                // Abort all pending actions
                                gdv_idletower_abort_by_gid (this->Private->Tower, SEEK_ACTION_GID);
                                gdv_idletower_abort_by_gid (this->Private->Tower, SEGMENT_DONE_ACTION_GID);
                                
                                gdv_idletower_add_item (this->Private->Tower, SEEK_ACTION_GID, 
                                                        (gpointer) seek_action,
                                                        (gpointer) seek_action_dispose, 
                                                        NULL, action);
                        } else
                                this->Private->SavedSeek = cur;
                } break;

                default: {
                        GDV_WARNING_ELEMENT ("Unhandled event type, ignoring", NULL);
                        ret = FALSE;
                        goto done;
                }

        }

        done:
        gst_event_unref (event);
        return ret;
}

/* Relink according to mode and current settings. Alters the 
 * object params */
static gboolean                 do_relink (GdvTrack *this, GdvTrackPlayback mode, GdvClip *clip,
                                           gboolean *vrelinked, gboolean *arelinked)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (vrelinked != NULL && arelinked != NULL, FALSE);
        
        gboolean relinkblackness = FALSE;
        gboolean relinksilence = FALSE;
        gboolean relinkvclip = FALSE;
        gboolean relinkaclip = FALSE;
        
        switch (mode) {
                
                case GDV_TRACK_PLAYBACK_BLACK_SILENCE: {
                        
                        if (this->Private->CurrentVElement != this->Private->Blackness)
                                relinkblackness = TRUE;
                        
                        if (this->Private->CurrentAElement != this->Private->Silence)
                                relinksilence = TRUE;
                        
                } break;
                
                case GDV_TRACK_PLAYBACK_BLACK_CLIP: {
                        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                        
                        if (this->Private->CurrentVElement != this->Private->Blackness)
                                relinkblackness = TRUE;
                        
                        if (this->Private->CurrentAElement != (GstElement *) clip)
                                relinkaclip = TRUE;
                        
                } break;
                
                case GDV_TRACK_PLAYBACK_CLIP_SILENCE: {
                        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                        
                        if (this->Private->CurrentAElement != this->Private->Silence)
                                relinksilence = TRUE;
                        
                        if (this->Private->CurrentVElement != (GstElement *) clip)
                                relinkvclip = TRUE;
                        
                } break;
                
                case GDV_TRACK_PLAYBACK_CLIP_CLIP: {
                        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                        
                        if (this->Private->CurrentVElement != (GstElement *) clip)
                                relinkvclip = TRUE;
                        
                        if (this->Private->CurrentAElement != (GstElement *) clip)
                                relinkaclip = TRUE;
                        
                } break;
                
                default:
                g_return_val_if_reached (FALSE);
                break;
        }
        
        // Unlink current video element
        if (relinkblackness || relinkvclip) {
                GDV_DEBUG_ELEMENT ("Unlinking old video element '%s'", 
                                   GST_OBJECT_NAME (this->Private->CurrentVElement));
                
                gst_element_unlink (GST_ELEMENT (this->Private->CurrentVElement), 
                                    GST_ELEMENT (this->Private->VCorrector));
        }
        
        // Unlink current audio element
        if (relinksilence || relinkaclip) {
                GDV_DEBUG_ELEMENT ("Unlinking old audio element '%s'", 
                                   GST_OBJECT_NAME (this->Private->CurrentAElement));
                
                gst_element_unlink (GST_ELEMENT (this->Private->CurrentAElement), 
                                    GST_ELEMENT (this->Private->ACorrector));
        }

        // Possibly relink blackness
        if (relinkblackness == TRUE) {
                GDV_DEBUG_ELEMENT ("Relinking to Blackness", NULL);
                gboolean res1 = gst_element_link_pads (GST_ELEMENT (this->Private->Blackness),
                                                       "src",
                                                       GST_ELEMENT (this->Private->VCorrector),
                                                       "sink");
                GDV_DEBUG_ELEMENT ("(rebuil) Relink blackness %d", res1);
                //g_return_val_if_fail (res1 != TRUE, FALSE);
                *vrelinked = TRUE;
                this->Private->CurrentVElement = this->Private->Blackness;
        }
        
        // Possibly relink silence
        if (relinksilence == TRUE) {
                GDV_DEBUG_ELEMENT ("Relinking to Silence", NULL);
                // Relink the video as it is different
                gst_element_link_pads (GST_ELEMENT (this->Private->Silence),
                                                    "src", 
                                                    GST_ELEMENT (this->Private->ACorrector),
                                                    "sink");
                //g_return_val_if_fail (res2 != TRUE, FALSE);
                *arelinked = TRUE;
                this->Private->CurrentAElement = this->Private->Silence;
        }
        
        // Possibly relink clip v
        if (relinkvclip == TRUE && 
            this->Private->CurrentVElement != (GstElement *) clip) {
                GDV_DEBUG_ELEMENT ("Relinking to clip video", NULL);
                gboolean res3 = gst_element_link_pads (GST_ELEMENT (clip),
                                                       "vsrc", 
                                                       GST_ELEMENT (this->Private->VCorrector),
                                                       "sink");
                GDV_DEBUG_ELEMENT ("(rebuil) Relink clip %d", res3);
                //g_return_val_if_fail (res3 != TRUE, FALSE);
                *vrelinked = TRUE;
                this->Private->CurrentVElement = (GstElement *) clip;
        }
        
        // Possibly relink clip a
        if (relinkaclip == TRUE && 
            this->Private->CurrentAElement != (GstElement *) clip) {
                GDV_DEBUG_ELEMENT ("Relinking to clip audio", NULL);
                gst_element_link_pads (GST_ELEMENT (clip),
                                                    "asrc", 
                                                    GST_ELEMENT (this->Private->ACorrector),
                                                    "sink");

                //g_return_val_if_fail (res4 != TRUE, FALSE);
                *arelinked = TRUE;
                this->Private->CurrentAElement = (GstElement *) clip;
        }
        
        GDV_DEBUG_ELEMENT ("Relinked video: %d Relinked audio: %d", 
                           *vrelinked, *arelinked);
                
        return TRUE;
}

static gboolean                 check_relink (GdvTrack *this, GdvTrackPlayback mode, GdvClip *clip,
                                              gboolean *vrelinked, gboolean *arelinked)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (vrelinked != NULL && arelinked != NULL, FALSE);
        
        gboolean relinkblackness = FALSE;
        gboolean relinksilence = FALSE;
        gboolean relinkvclip = FALSE;
        gboolean relinkaclip = FALSE;
        
        switch (mode) {
                
                case GDV_TRACK_PLAYBACK_BLACK_SILENCE: {
                        
                        if (this->Private->CurrentVElement != this->Private->Blackness)
                                relinkblackness = TRUE;
                        
                        if (this->Private->CurrentAElement != this->Private->Silence)
                                relinksilence = TRUE;
                        
                } break;
                
                case GDV_TRACK_PLAYBACK_BLACK_CLIP: {
                        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                        
                        if (this->Private->CurrentVElement != this->Private->Blackness)
                                relinkblackness = TRUE;
                        
                        if (this->Private->CurrentAElement != (GstElement *) clip)
                                relinkaclip = TRUE;
                        
                } break;
                
                case GDV_TRACK_PLAYBACK_CLIP_SILENCE: {
                        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                        
                        if (this->Private->CurrentAElement != this->Private->Silence)
                                relinksilence = TRUE;
                        
                        if (this->Private->CurrentVElement != (GstElement *) clip)
                                relinkvclip = TRUE;
                        
                } break;
                
                case GDV_TRACK_PLAYBACK_CLIP_CLIP: {
                        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
                        
                        if (this->Private->CurrentVElement != (GstElement *) clip)
                                relinkvclip = TRUE;
                        
                        if (this->Private->CurrentAElement != (GstElement *) clip)
                                relinkaclip = TRUE;
                        
                } break;
                
                default:
                g_return_val_if_reached (FALSE);
                break;
        }

        // Possibly relink blackness
        if (relinkblackness == TRUE) {
                *vrelinked = TRUE;
        }
        
        // Possibly relink silence
        if (relinksilence == TRUE) {
                *arelinked = TRUE;
        }
        
        // Possibly relink clip v
        if (relinkvclip == TRUE && 
            this->Private->CurrentVElement != (GstElement *) clip) {

                *vrelinked = TRUE;
        }
        
        // Possibly relink clip a
        if (relinkaclip == TRUE && 
            this->Private->CurrentAElement != (GstElement *) clip) {

                *arelinked = TRUE;
        }

        return TRUE;
}

/* Get the playback mode and the current clip */
static GdvTrackPlayback         get_mode (GdvTrack *this, GdvClip **clip, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), GDV_TRACK_PLAYBACK_UNKNOWN);
        
        *clip = gdv_clipstore_get_clip_at_time (this->Private->ClipStore,
                                                time);
        if (*clip == NULL)
                return GDV_TRACK_PLAYBACK_BLACK_SILENCE;
        
        gboolean hasaudio, hasvideo;
        g_object_get (G_OBJECT (*clip), 
                      "hasaudio", &hasaudio,
                      "hasvideo", &hasvideo,
                      NULL);
        
        if (hasaudio && hasvideo)
                return GDV_TRACK_PLAYBACK_CLIP_CLIP;
        else if (hasvideo)
                return GDV_TRACK_PLAYBACK_CLIP_SILENCE;
        else if (hasaudio)
                return GDV_TRACK_PLAYBACK_BLACK_CLIP;
                
        g_return_val_if_reached (GDV_TRACK_PLAYBACK_UNKNOWN);
}

/* Find the boundries for the seek depending on the mode etc. Note that still 
 * the handling of the values (where exactly they need to be sent) is still 
 * dependent on the mode etc. */
static gboolean                 get_seek_span (GdvTrack *this, GdvTrackPlayback mode, GdvClip *curclip,
                                               gint64 *start, gint64 *stop, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);
        g_return_val_if_fail (start != NULL && stop != NULL, FALSE);
        
        switch (mode) {
                
                case GDV_TRACK_PLAYBACK_BLACK_SILENCE: {
                        // We'playing using silence element and blackness
                        // element. We need to see what's after present time
                        GdvClip *clip = gdv_clipstore_get_clip_after_time (this->Private->ClipStore, 
                                                                           time);
                        if (clip == NULL) {
                                *start = time;
                                *stop = -1;
                                return TRUE;
                        } else {
                                // Let's get the timespan of the clip
                                GdvTimeSpan *span = NULL;
                                g_object_get (G_OBJECT (clip), "timelinespan", &span, NULL);
                                g_return_val_if_fail (span != NULL, FALSE);
                                
                                *start = time;
                                *stop = span->Start;
                                gdv_timespan_free (span);
                                
                                return TRUE;
                        }
                } break;
                
                case GDV_TRACK_PLAYBACK_CLIP_CLIP:
                case GDV_TRACK_PLAYBACK_CLIP_SILENCE:
                case GDV_TRACK_PLAYBACK_BLACK_CLIP: {
                        // We're playing black video and some sound from the
                        // clip
                        g_return_val_if_fail (curclip != NULL, FALSE);
                        GdvTimeSpan *span = NULL;
                        g_object_get (G_OBJECT (curclip), "timelinespan", &span, NULL);
                        g_return_val_if_fail (span != NULL, FALSE);
                        
                        *start = time;
                        *stop = span->End;
                        gdv_timespan_free (span);
                        
                        return TRUE;  
                } break;
                
                default:
                g_return_val_if_reached (FALSE);
                break;
        }
}

/* This is for debug routines only. Translate the mode to a string */
static gchar*                   playback_mode_to_string (GdvTrackPlayback mode)
{
        switch (mode) {
                
                case GDV_TRACK_PLAYBACK_BLACK_SILENCE:
                return "no video & no audio";
                break;

                case GDV_TRACK_PLAYBACK_BLACK_CLIP:
                return "no video & some audio";
                break;
                
                case GDV_TRACK_PLAYBACK_CLIP_SILENCE:
                return "some video & no audio";
                break;
                
                case GDV_TRACK_PLAYBACK_CLIP_CLIP:
                return "some video & some audio";
                break;
                
                default:
                return "Unknown";
                break;
                
        }
}

/* A signal emitted by the clip when it's position/params where changed. We ask for a rebuild */
static void                     on_clip_changed (gpointer a, GdvTrack *this)
{
        g_return_if_fail (GDV_IS_TRACK (this));

        GDV_DEBUG_ELEMENT ("Clip changed, asking for a rebuild", NULL);

        // FIXME: We need to be 100% sure that the ClipStore actually updated the
        // clip in the first place! (received signal first)
        g_signal_emit (this, GDV_TRACK_GET_CLASS (this)->ReseekNeededSignalId,
                       0, NULL);
}

/* GstElement state change */
static GstStateChangeReturn     change_state (GdvTrack *this, GstStateChange transition)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), GST_STATE_CHANGE_FAILURE);
        GDV_DEBUG_ELEMENT ("Changing state", NULL);
        
        GstStateChangeReturn ret;
        
        switch (transition) {
                
                case GST_STATE_CHANGE_READY_TO_PAUSED: {
                        do_initial_setup (this);                        
                } break;

                case GST_STATE_CHANGE_PAUSED_TO_READY: {
                        this->Private->SavedSeek = GDV_ZEROTIME;
                } break;
                        
                default: 
                break;
        }
        
        ret = GST_ELEMENT_CLASS (ParentClass)->change_state (GST_ELEMENT (this), 
                                                             transition);
        return ret;
}

static int                      clip_find_func (ClipData *a, GdvClip *clip)
{
        g_return_val_if_fail (a != NULL, -1);
        g_return_val_if_fail (clip != NULL, -1);

        if (a->Clip == clip)
                return 0;
        else
                return -1;
}

static void                     gdv_track_set_property (GdvTrack *this, guint propid, 
                                                        GValue *value, GParamSpec *pspec)
{
        /*
        g_assert (GDV_IS_TRACK (this));

        switch (propid) {

                case ARG_OPACITY:
                        gdv_bumperstamper_set_opacity (this->Private->VBumper, g_value_get_double (value));
                        g_signal_emit (this, GDV_TRACK_GET_CLASS (this)->ReseekNeededSignalId,
                                       0, NULL);
                        break;
                                        
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (this, propid, pspec);
                        break;
                        }*/
}

/* GObject dispose */
static void                     gdv_track_dispose (GdvTrack *this)
{
        g_return_if_fail (GDV_IS_TRACK (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this, this->Private);

        // FIXME: Wrong dispose
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_track_finalize (GdvTrack *this)
{
        g_return_if_fail (GDV_IS_TRACK (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }

        // FIXME: Wrong finalize
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

static gboolean                 do_initial_setup (GdvTrack *this)
{
        GDV_DEBUG_ELEMENT ("Doing an initial rebuild at %s", 
                           gdv_time_to_string (this->Private->SavedSeek));
        
        GdvTrackPlayback mode = GDV_TRACK_PLAYBACK_UNKNOWN; // Our mode
        GdvClip *currentclip = NULL;                        // Clip we're supposed to play now
        gboolean vrelinked = FALSE;                         // Did we relinked video
        gboolean arelinked = FALSE;                         // Did we relinked audio
        GstElement *oldvelement = 
                this->Private->CurrentVElement;             // Current/old video element
        GstElement *oldaelement = 
                this->Private->CurrentAElement;             // Current/old audio element
        
        // Get the mode
        mode = get_mode (this, &currentclip, this->Private->SavedSeek);
        g_return_val_if_fail (mode != GDV_TRACK_PLAYBACK_UNKNOWN, FALSE);
        
        GDV_DEBUG_ELEMENT ("Current clip: %s Mode: %s", 
                           (currentclip != NULL) ? GST_OBJECT_NAME (currentclip) : "(NULL)",
                           playback_mode_to_string (mode));

        g_return_val_if_fail (do_relink (this, mode, currentclip, &vrelinked, &arelinked), FALSE);

        gint64 start, stop;
        get_seek_span (this, mode, currentclip, &start, &stop, this->Private->SavedSeek);

        GDV_DEBUG_ELEMENT ("Seeking %s %s-%s", GST_OBJECT_NAME (this->Private->CurrentAElement),
                           gdv_time_to_string (start), gdv_time_to_string (stop));
                                                                        
        gst_element_seek (this->Private->CurrentAElement, 1.0, GST_FORMAT_TIME,
                          GST_SEEK_FLAG_SEGMENT,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, stop);

        GDV_DEBUG_ELEMENT ("Seeking %s %s-%s", GST_OBJECT_NAME (this->Private->CurrentVElement),
                           gdv_time_to_string (start), gdv_time_to_string (stop));
                                                                        
        gst_element_seek (this->Private->CurrentVElement, 1.0, GST_FORMAT_TIME,
                          GST_SEEK_FLAG_SEGMENT,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, stop);

        this->Private->NextCutAt = stop;

        // Unlock 
        if (vrelinked) {
                GDV_DEBUG_ELEMENT ("Locking %s, Unlocking %s",
                                   GST_OBJECT_NAME (oldvelement), GST_OBJECT_NAME (this->Private->CurrentVElement));
                gst_element_set_locked_state (oldvelement, TRUE);
                gst_element_set_locked_state (this->Private->CurrentVElement, FALSE);
        }

        if (arelinked) {
                GDV_DEBUG_ELEMENT ("Locking %s, Unlocking %s",
                                   GST_OBJECT_NAME (oldaelement), GST_OBJECT_NAME (this->Private->CurrentAElement));
                gst_element_set_locked_state (oldaelement, TRUE);
                gst_element_set_locked_state (this->Private->CurrentAElement, FALSE);
        }
        
        this->Private->FirstDone = FALSE;
        
        return TRUE;
}

static void                     seek_action (SeekAction *action)
{
        g_return_if_fail (action != NULL);
        
        GdvTrack *this = action->Track;
        g_return_if_fail (GDV_IS_TRACK (this));

        g_mutex_lock (this->Private->SegmentMutex);
        GDV_DEBUG_ELEMENT ("Doing a seek action rebuild at %s", gdv_time_to_string (action->Time));
        g_debug ("Seeking at %s", gdv_time_to_string (action->Time));

        GdvTrackPlayback mode = GDV_TRACK_PLAYBACK_UNKNOWN; // Our mode
        GdvClip *currentclip = NULL;                        // Clip we're supposed to play now
        gboolean vrelinked = FALSE;                         // Did we relinked video
        gboolean arelinked = FALSE;                         // Did we relinked audio
        GstElement *oldvelement = 
                this->Private->CurrentVElement;             // Current/old video element
        GstElement *oldaelement = 
                this->Private->CurrentAElement;             // Current/old audio element
        GstState parentstate;                               // The current track state
        gboolean vflush = FALSE;
        gboolean aflush = FALSE;

        // Get the state
        GDV_DEBUG_ELEMENT ("Getting parent state", NULL);
        gst_element_get_state (GST_ELEMENT (this), 
                               &parentstate, NULL, GST_CLOCK_TIME_NONE);
        parentstate = GST_STATE_PAUSED;

        // Get the mode
        mode = get_mode (this, &currentclip, action->Time);
        g_return_if_fail (mode != GDV_TRACK_PLAYBACK_UNKNOWN);
        
        GDV_DEBUG_ELEMENT ("Current clip: %s Mode: %s", 
                           (currentclip != NULL) ? GST_OBJECT_NAME (currentclip) : "(NULL)",
                           playback_mode_to_string (mode));

        g_return_if_fail (check_relink (this, mode, currentclip, &vflush, &aflush));

        g_debug ("Start flushes!");
        
        if (vflush) {
                GDV_DEBUG_ELEMENT ("Sending a vsrc flush-start", NULL);
                GstEvent *event1 = gst_event_new_flush_start ();
                gst_pad_send_event (GST_PAD_PEER (gst_element_get_pad (GST_ELEMENT (this), "vsrc")), event1);
        }
 
        if (aflush) {
                GDV_DEBUG_ELEMENT ("Sending an asrc flush-start", NULL);
                GstEvent *event2 = gst_event_new_flush_start ();
                gst_pad_send_event (GST_PAD_PEER (gst_element_get_pad (GST_ELEMENT (this), "asrc")), event2);
        }

        g_debug ("Stop flushes");
        
        if (aflush) {
                GDV_DEBUG_ELEMENT ("Sending an asrc flush-stop", NULL);
                GstEvent *event1 = gst_event_new_flush_stop ();
                gst_pad_send_event (GST_PAD_PEER (gst_element_get_pad (GST_ELEMENT (this), "asrc")), event1);
        }

        if (vflush) {
                GDV_DEBUG_ELEMENT ("Sending a vsrc flush-stop", NULL);
                GstEvent *event2 = gst_event_new_flush_stop ();
                gst_pad_send_event (GST_PAD_PEER (gst_element_get_pad (GST_ELEMENT (this), "vsrc")), event2);
        }

        g_debug ("Reseetting the corrector");
        g_return_if_fail (do_relink (this, mode, currentclip, &vrelinked, &arelinked));
        gdv_corrector_reset (GDV_CORRECTOR (this->Private->ACorrector));
        gdv_corrector_reset (GDV_CORRECTOR (this->Private->VCorrector));

        gint64 start, stop;
        get_seek_span (this, mode, currentclip, &start, &stop, action->Time);

        g_debug ("Seeking");
        gst_element_seek (this->Private->CurrentAElement, 1.0, GST_FORMAT_TIME,
                          (aflush == TRUE) ? GST_SEEK_FLAG_SEGMENT : GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, stop);

        gst_element_seek (this->Private->CurrentVElement, 1.0, GST_FORMAT_TIME,
                          (vflush == TRUE) ? GST_SEEK_FLAG_SEGMENT : GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, stop);

        g_debug ("Post routine");
        if (vrelinked == TRUE) {
                GDV_DEBUG_ELEMENT ("Locking %s and setting to READY", GST_OBJECT_NAME (oldvelement));
                gst_element_set_locked_state (oldvelement, TRUE);

                if (oldvelement == this->Private->Blackness) 
                        gst_element_set_state (oldvelement, GST_STATE_READY);
                else
                        gst_element_set_state (oldvelement, GST_STATE_NULL);

                gst_element_get_state (oldvelement, NULL, NULL, GST_CLOCK_TIME_NONE);
 
                GDV_DEBUG_ELEMENT ("Unlocking %s and setting to parent state", GST_OBJECT_NAME (this->Private->CurrentVElement));
                gst_element_set_locked_state (this->Private->CurrentVElement, FALSE);
                gst_element_set_state (this->Private->CurrentVElement, parentstate);
        }

        if (arelinked == TRUE) {
                GDV_DEBUG_ELEMENT ("Locking %s and setting to READY", GST_OBJECT_NAME (oldaelement));
                gst_element_set_locked_state (oldaelement, TRUE);
                
                if (oldaelement == this->Private->Silence) 
                        gst_element_set_state (oldaelement, GST_STATE_READY);
                else
                        gst_element_set_state (oldaelement, GST_STATE_NULL);
                
                gst_element_get_state (oldaelement, NULL, NULL, GST_CLOCK_TIME_NONE);

                GDV_DEBUG_ELEMENT ("Unlocking %s and setting to parent state", GST_OBJECT_NAME (this->Private->CurrentAElement));
                gst_element_set_locked_state (this->Private->CurrentAElement, FALSE);
                gst_element_set_state (this->Private->CurrentAElement, parentstate);
        }

        g_debug ("Done! New velement: %s", GST_OBJECT_NAME (this->Private->CurrentVElement));
        this->Private->FirstDone = FALSE;
        this->Private->NextCutAt = stop;
        g_mutex_unlock (this->Private->SegmentMutex);

        g_debug ("Leaving");
}

static void                     post_remove_action (PostRemoveAction *action)
{
        g_return_if_fail (action != NULL);
        
        GdvTrack *this = action->Track;
        g_return_if_fail (GDV_IS_TRACK (this));

        GDV_DEBUG_ELEMENT ("Doing a post remove action. Nulling a removing %s", GST_OBJECT_NAME (action->Clip));

        gst_element_set_state (GST_ELEMENT (action->Clip), GST_STATE_NULL);
        gst_element_get_state (GST_ELEMENT (action->Clip), NULL, NULL, GST_CLOCK_TIME_NONE);
        gst_bin_remove (GST_BIN (this), GST_ELEMENT (action->Clip));
}

static void                     segment_done_action (SegmentDoneAction *action)
{
        g_debug ("Segment done action");
        g_return_if_fail (action != NULL);
        
        GdvTrack *this = action->Track;
        g_return_if_fail (GDV_IS_TRACK (this));

        g_mutex_lock (this->Private->SegmentMutex);
        GDV_DEBUG_ELEMENT ("Doing a segment-done action rebuild at %s", gdv_time_to_string (action->Time));

        GdvTrackPlayback mode = GDV_TRACK_PLAYBACK_UNKNOWN; // Our mode
        GdvClip *currentclip = NULL;                        // Clip we're supposed to play now
        gboolean vrelinked = FALSE;                         // Did we relinked video
        gboolean arelinked = FALSE;                         // Did we relinked audio
        GstElement *oldvelement = 
                this->Private->CurrentVElement;             // Current/old video element
        GstElement *oldaelement = 
                this->Private->CurrentAElement;             // Current/old audio element
        GstState parentstate;                               // The current track state

        // Get the state
        GDV_DEBUG_ELEMENT ("Getting parent state", NULL);
        gst_element_get_state (GST_ELEMENT (this), 
                               &parentstate, NULL, GST_CLOCK_TIME_NONE);
        
        // Get the mode
        mode = get_mode (this, &currentclip, action->Time);
        g_return_if_fail (mode != GDV_TRACK_PLAYBACK_UNKNOWN);
        
        GDV_DEBUG_ELEMENT ("Current clip: %s Mode: %s", 
                           (currentclip != NULL) ? GST_OBJECT_NAME (currentclip) : "(NULL)",
                           playback_mode_to_string (mode));

        gst_element_set_locked_state (oldvelement, TRUE);
        gst_element_set_locked_state (oldaelement, TRUE);
 
        if (oldvelement == this->Private->Blackness) 
                gst_element_set_state (oldvelement, GST_STATE_READY);
        else
                gst_element_set_state (oldvelement, GST_STATE_NULL);

        if (oldaelement == this->Private->Silence) 
                gst_element_set_state (oldaelement, GST_STATE_READY);
        else
                gst_element_set_state (oldaelement, GST_STATE_NULL);

        //gst_element_get_state (oldvelement, NULL, NULL, GST_CLOCK_TIME_NONE);
        //gst_element_get_state (oldaelement, NULL, NULL, GST_CLOCK_TIME_NONE);

        g_return_if_fail (do_relink (this, mode, currentclip, &vrelinked, &arelinked));

        gint64 start, stop;
        get_seek_span (this, mode, currentclip, &start, &stop, action->Time);

        GDV_DEBUG_ELEMENT ("Seeking %s %s-%s", GST_OBJECT_NAME (this->Private->CurrentAElement),
                           gdv_time_to_string (start), gdv_time_to_string (stop));
                                                                        
        gst_element_seek (this->Private->CurrentAElement, 1.0, GST_FORMAT_TIME,
                          GST_SEEK_FLAG_SEGMENT,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, stop);

        GDV_DEBUG_ELEMENT ("Seeking %s %s-%s", GST_OBJECT_NAME (this->Private->CurrentVElement),
                           gdv_time_to_string (start), gdv_time_to_string (stop));
                                                                        
        gst_element_seek (this->Private->CurrentVElement, 1.0, GST_FORMAT_TIME,
                          GST_SEEK_FLAG_SEGMENT,
                          GST_SEEK_TYPE_SET, start,
                          GST_SEEK_TYPE_SET, stop);

        gst_element_set_locked_state (this->Private->CurrentVElement, FALSE);
        gst_element_set_locked_state (this->Private->CurrentAElement, FALSE);

        gst_element_set_state (this->Private->CurrentAElement, parentstate);
        gst_element_set_state (this->Private->CurrentVElement, parentstate);
        
        this->Private->NextCutAt = stop;

        g_mutex_unlock (this->Private->SegmentMutex);

        g_debug ("Leaving segment-done");
}

static gboolean                 segment_done_action_dispose (SegmentDoneAction *action)
{
        g_return_val_if_fail (action != NULL, FALSE);
        g_free (action);

        return TRUE;
}

static gboolean                 seek_action_dispose (SeekAction *action)
{
        g_return_val_if_fail (action != NULL, FALSE);
        g_free (action);

        return TRUE;
}

static gboolean                 post_remove_action_dispose (PostRemoveAction *action)
{
        g_return_val_if_fail (action != NULL, FALSE);
        g_free (action);

        return TRUE;
}

static gboolean                 src_event_probe (GstPad *pad, GstEvent *event, GdvTrack *this)
{
        g_return_val_if_fail (GDV_IS_TRACK (this), FALSE);

        GDV_DEBUG_ELEMENT ("Event probe for %s, event %s", GST_PAD_NAME (pad), GST_EVENT_TYPE_NAME (event));

        // We forward everything point-blank
        if (GST_EVENT_TYPE (event) != GST_EVENT_SEEK)
                return TRUE;

        // If it's a seek, we anihilate it and forward the actual event to the
        // pad-send function
        
        GDV_DEBUG_ELEMENT ("Forwarding this seek event to the track send function", NULL);
        // FIXME: This is surely broken in terms of reffing
        gst_element_send_event (GST_ELEMENT (this), gst_event_copy (event)); 

        return FALSE;
}
