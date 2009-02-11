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

#include "GdvDecodeBin.h"
#include "GdvDecodeBinPrivate.h"
#include "GdvSourceProtected.h"

static GdvSourceClass*          ParentClass = NULL;

/* GObject get type */
GType                           gdv_decodebin_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvDecodeBinClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_decodebin_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvDecodeBin),
                        8,
                        (GInstanceInitFunc) gdv_decodebin_init,
                };
        
                // FileBased interface
                static const GInterfaceInfo filebasedinfo = {
                        NULL,
                        NULL,
                        NULL
                };

                objecttype = g_type_register_static (GDV_TYPE_SOURCE, 
                                                     "GdvDecodeBin", 
                                                     &objectinfo, 
                                                     0);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_FILE_BASED,
                                             &filebasedinfo);
        }

        return objecttype;
}

/* GObject class init */
static void                     gdv_decodebin_class_init (GdvDecodeBinClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GdvSourceClass *sourceclass = (GdvSourceClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) klass;

        ParentClass = g_type_class_ref (GDV_TYPE_SOURCE);

        gobjectclass->get_property = (gpointer) (gdv_decodebin_get_property);
        
        g_object_class_install_property (gobjectclass, ARG_BASEFILE,
                                         g_param_spec_string
                                         ("basefile", "BaseFile",
                                          "The media file we're based on",
                                          NULL, G_PARAM_READABLE));

        gobjectclass->dispose = (gpointer) (gdv_decodebin_dispose);
        gobjectclass->finalize = (gpointer) (gdv_decodebin_finalize);
        sourceclass->CloneFunc = (GdvSourceCloneFunc) clone;
        elementclass->change_state = (gpointer) change_state;
        elementclass->send_event = (gpointer) send_event;
}

/* GObject init */
static void                     gdv_decodebin_init (GdvDecodeBin *this)
{
        GDV_DEBUG_ELEMENT ("Initializing fresh GdvDecodeBin object", NULL);

        g_return_if_fail (GDV_IS_DECODEBIN (this));
        this->Private = NULL;

        GdvDecodeBinPrivate *priv = g_new0 (GdvDecodeBinPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Create a new avisource for the given file */
static gboolean                 private_build (GdvDecodeBin *this, GdvDecodeBinPrivate *private, const gchar *filename)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), FALSE);
        g_return_val_if_fail (filename != NULL, FALSE);
        g_return_val_if_fail (private != NULL, FALSE);
        
        GDV_DEBUG_ELEMENT ("Builiding the GdvDecodeBin elements for %s", filename);
        this->Private->FileSrc = gst_element_factory_make ("filesrc", NULL);
        g_return_val_if_fail (this->Private->FileSrc != NULL, FALSE);
        
        g_object_set (G_OBJECT (this->Private->FileSrc), "location", filename, NULL);
        
        this->Private->DecodeBin = gst_element_factory_make ("decodebin", NULL);
        g_return_val_if_fail (this->Private->DecodeBin != NULL, FALSE);
        
        g_signal_connect (G_OBJECT (this->Private->DecodeBin), "pad-added",
                          G_CALLBACK (new_pad), this);

        g_signal_connect (G_OBJECT (this->Private->DecodeBin), "no-more-pads",
                          G_CALLBACK (no_more_pads), this);
        
        // Add
        gst_bin_add_many (GST_BIN (this), this->Private->FileSrc, this->Private->DecodeBin, NULL);
        
        // Link everything
        g_return_val_if_fail (gst_element_link (this->Private->FileSrc, this->Private->DecodeBin), FALSE);
                
        // Copy our filename
        this->Private->FileName = g_strdup (filename);
        
        return TRUE;
}

/* GObject dispose */
static void                     gdv_decodebin_dispose (GdvDecodeBin *this)
{
        g_return_if_fail (GDV_IS_DECODEBIN (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

static void                     gdv_decodebin_finalize (GdvDecodeBin *this)
{
        g_return_if_fail (GDV_IS_DECODEBIN (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* New pad was added */
static void                     new_pad (GstElement *element, GstPad *pad, GdvDecodeBin *this)
{
        g_return_if_fail (GDV_IS_DECODEBIN (this));
        GDV_DEBUG_ELEMENT ("Got a new-pad signal", NULL);

        GstCaps *caps;
        GstStructure *str;

        caps = gst_pad_get_caps (pad);
        g_assert (caps != NULL);
        str = gst_caps_get_structure (caps, 0);
        g_assert (str != NULL);

        if (g_strrstr (gst_structure_get_name (str), "video")) {
                if (((GdvSource *) this)->Protected->HasVideo != FALSE) {
                        if (gdv_source_set_video_target (GDV_SOURCE (this), pad) == FALSE)
                                GDV_WARNING_ELEMENT ("Failed to set video target pad", NULL);
                }
        }
        
        if (g_strrstr (gst_structure_get_name (str), "audio")) {
                if (((GdvSource *) this)->Protected->HasAudio != FALSE) {
                        if (gdv_source_set_audio_target (GDV_SOURCE (this), pad) == FALSE)
                                GDV_WARNING_ELEMENT ("Failed to set audio target pad", NULL);
                }
        }

        gst_caps_unref (caps);

        // Scan for queues
        scan_queues (this, GST_BIN (this->Private->DecodeBin));

        // Return now if there is nothing fancy to be done
        if (this->Private->CachedEvent == NULL)
                return;

        GDV_DEBUG_ELEMENT ("Adding event probe for %s", GST_PAD_NAME (pad));
        gst_pad_add_event_probe (pad, (GCallback) pad_event_probe, this);
        
        GDV_DEBUG_ELEMENT ("Blocking the pad %s", GST_PAD_NAME (pad));
        
        g_return_if_fail (gst_pad_set_blocked_async (pad, TRUE,
                                                     (GstPadBlockCallback) pad_block,
                                                     new_action (this, pad)));
}

static void                     pad_block (GstPad *pad, gboolean blocked, BlockAction *action)
{
        ACTION_LOCK (action);

        GdvDecodeBin *this = action->DecodeBin;

        if (blocked == TRUE && action->KillSwitch == FALSE) {
                GDV_DEBUG_ELEMENT ("%s pad BLOCKED", GST_PAD_NAME (action->Pad));
                
                action->Seek = (! this->Private->SeekScheduled);
                this->Private->SeekScheduled = TRUE;

                if (action->Seek == TRUE)
                        GDV_DEBUG_ELEMENT ("%s pad will seek in idle", GST_PAD_NAME (action->Pad));
                
                schedule_action_for_tower (this, action, (GdvIdleTowerActionFunc) perform_seek_action_func);
                action->KillSwitch = TRUE;
        } else if (blocked == TRUE && action->KillSwitch == TRUE)
                GDV_DEBUG_ELEMENT ("%s pad BLOCKED but ignoring", GST_PAD_NAME (action->Pad));
        else if (blocked == FALSE)
                GDV_DEBUG_ELEMENT ("%s pad UNBLOCKED", GST_PAD_NAME (action->Pad));
        else
                g_assert_not_reached ();
        
        ACTION_UNLOCK (action);
}

/* No more pads will be decoded. We just forward this as our globals bin info */
static void                     no_more_pads (GstElement *element, GdvDecodeBin *this)
{
        GDV_DEBUG_ELEMENT ("No more pads", NULL);
        gst_element_no_more_pads (GST_ELEMENT (this));

        g_mutex_lock (this->Private->StartMutex);
        this->Private->PadsFinished = TRUE;
        g_cond_signal (this->Private->StartCond);
        g_mutex_unlock (this->Private->StartMutex);
}

/* Create a new object */
GdvDecodeBin*                   gdv_decodebin_new (const gchar *filename, GdvVideoFormat *vformat,
                                                   GdvAudioFormat *aformat, GdvTime len)
{
        g_return_val_if_fail (filename != NULL, NULL);
        g_return_val_if_fail (GDV_IS_VIDEOFORMAT (vformat) || GDV_IS_AUDIOFORMAT (aformat), NULL);
        GDV_DEBUG_EXT ("Creating a new GdvDecodeBin for %s", filename);
        
        GdvDecodeBin *this = NULL;
        
        this = g_object_new (GDV_TYPE_DECODEBIN, NULL);
        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }

        if (private_build (this, this->Private, filename) == FALSE) {
                g_object_unref (this);
                return NULL;
        }

        // Ref etc.
        if (aformat != NULL) 
                gdv_source_set_audioformat (GDV_SOURCE (this), aformat);
        
        if (vformat != NULL) 
                gdv_source_set_videoformat (GDV_SOURCE (this), vformat);
        
        // Length
        gdv_source_set_length (GDV_SOURCE (this), len);

        // All ok
        return this;
}

/* Clone the source. We use the filename and the current params to do the trick */
static GdvDecodeBin*            clone (GdvDecodeBin *this)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), NULL);
        g_return_val_if_fail (this->Private->FileName != NULL, NULL);
        
        return gdv_decodebin_new (this->Private->FileName,
                                  ((GdvSource *) this)->Protected->VideoFormat,
                                  ((GdvSource *) this)->Protected->AudioFormat,
                                  ((GdvSource *) this)->Protected->Length);
}

/* GObject get property */
static void                     gdv_decodebin_get_property (GdvDecodeBin *this, guint propid,
                                                            GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_DECODEBIN (this));

        switch (propid) {

                case ARG_BASEFILE:
                        g_return_if_fail (this->Private->FileName != NULL);
                        g_value_set_string (value, this->Private->FileName);
                        break;

                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                        break;
        }
}

static gboolean                 private_init (GdvDecodeBin *this, GdvDecodeBinPrivate *private)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);
        
        private->FileName = NULL;
        private->FileSrc = NULL;
        private->DecodeBin = NULL;
        
        private->Tower = gdv_idletower_new ();
        g_return_val_if_fail (private->Tower != NULL, FALSE);
        
        private->Running = FALSE;
        private->CachedEvent = NULL;
        private->SeekScheduled = FALSE;
        private->PadsFinished = FALSE;
        private->SegmentsDone = FALSE;
        private->SegmentsReceived = 0;
        private->QHandles = NULL;
        
        private->StartMutex = g_mutex_new ();
        g_return_val_if_fail (private->StartMutex != NULL, FALSE);
        
        private->StartCond = g_cond_new ();
        g_return_val_if_fail (private->StartCond != NULL, FALSE);

        private->QMutex = g_mutex_new ();
        g_return_val_if_fail (private->QMutex != NULL, FALSE);

        // Bus
        GstBus *bus = GST_BIN (this)->child_bus;
        g_return_val_if_fail (bus != NULL, FALSE);
        
        private->OrigSyncHandler = bus->sync_handler;
        private->OrigSyncHandlerData = bus->sync_handler_data;
        
        bus->sync_handler = (GstBusSyncHandler) sync_handler;
        bus->sync_handler_data = (gpointer) this;

        private->CachedSegmentDone = NULL;
        private->WaitingForExhaust = FALSE;

        return TRUE;
}

static void                     private_dispose (GdvDecodeBin *this, GdvDecodeBinPrivate *private)
{
        g_return_if_fail (GDV_IS_DECODEBIN (this));
        g_return_if_fail (private != NULL);
        
        if (private->FileName != NULL) {
                g_free (private->FileName);
                private->FileName = NULL;
        }

        if (private->Tower != NULL) {
                g_object_unref (private->Tower);
                private->Tower = NULL;
        }

        if (private->CachedEvent != NULL) {
                gst_event_unref (private->CachedEvent);
                private->CachedEvent = NULL;
        }

        if (private->StartMutex != NULL) {
                g_mutex_free (private->StartMutex);
                private->StartMutex = NULL;
        }

        if (private->StartCond != NULL) {
                gst_event_unref (private->StartCond);
                private->StartCond = NULL;
        }

        if (private->QMutex != NULL) {
                g_mutex_free (private->QMutex);
                private->QMutex = NULL;
        }
}

/* GstElement state change */
static GstStateChangeReturn     change_state (GdvDecodeBin *this, GstStateChange transition)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), GST_STATE_CHANGE_FAILURE);
        GDV_DEBUG_ELEMENT ("Changing state", NULL);
        
        GstStateChangeReturn ret;

        switch (transition) {

                case GST_STATE_CHANGE_READY_TO_NULL: {
                        gdv_source_set_audio_target (GDV_SOURCE (this), NULL);
                        gdv_source_set_video_target (GDV_SOURCE (this), NULL);
                } break;

                default:
                break;

        }

        ret = GST_ELEMENT_CLASS (ParentClass)->change_state (GST_ELEMENT (this), 
                                                             transition);
        
        switch (transition) {

                case GST_STATE_CHANGE_READY_TO_PAUSED: {
                        g_mutex_lock (this->Private->StartMutex);

                        while (this->Private->PadsFinished == FALSE)
                                g_cond_wait (this->Private->StartCond, this->Private->StartMutex);
                        
                        g_mutex_unlock (this->Private->StartMutex);
                        
                        this->Private->Running = TRUE;
                } break;

                case GST_STATE_CHANGE_PAUSED_TO_READY: {
                        GDV_DEBUG_ELEMENT ("PAUSED TO READY", NULL);
                        this->Private->Running = FALSE;
                        this->Private->SeekScheduled = FALSE;
                        this->Private->SegmentsReceived = 0;
                        this->Private->SegmentsDone = FALSE;
                        
                        if (this->Private->CachedEvent != NULL)
                                gst_event_unref (this->Private->CachedEvent);
                        this->Private->CachedEvent = NULL;

                        gdv_idletower_wipe_and_finalize (this->Private->Tower);
                } break;

                case GST_STATE_CHANGE_READY_TO_NULL: {
                        this->Private->PadsFinished = FALSE;
                
                        // Dispose the queue data
                        Q_LOCK (this);
                        GSList *iter;
                        for (iter = this->Private->QHandles; iter; iter = g_slist_next (iter)) {
                                QueueHandle *handle = iter->data;
                                g_free (handle);
                        }
                
                        g_slist_free (this->Private->QHandles);
                        this->Private->QHandles = NULL;
                        Q_UNLOCK (this);
                } break;

                default: 
                break;
        }
                
        return ret;
}

/* GstElement send event */
static gboolean                 send_event (GdvDecodeBin *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), FALSE);

        GDV_DEBUG_ELEMENT ("Send event: %s", GST_EVENT_TYPE_NAME (event));
                
        if (this->Private->Running)
                goto DefaultHandler;

        if (GST_EVENT_TYPE (event) != GST_EVENT_SEEK)
                goto DefaultHandler;

        GDV_DEBUG_ELEMENT ("Caching a seek event", NULL);
        if (this->Private->CachedEvent != NULL) 
                gst_event_unref (this->Private->CachedEvent);

        this->Private->CachedEvent = event;
        gst_event_ref (this->Private->CachedEvent);
        
        return TRUE;
                
 DefaultHandler:

        GDV_DEBUG_ELEMENT ("Using a default handler for event", NULL);
        return gdv_source_send_event_stock (GDV_SOURCE (this), event);
}

static BlockAction*             new_action (GdvDecodeBin *this, GstPad *pad)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), NULL);

        GDV_DEBUG_ELEMENT ("Creating a new action for pad %s", GST_PAD_NAME (pad));
        
        BlockAction *new = g_new (BlockAction, 1);
        g_return_val_if_fail (new != NULL, NULL);
        
        new->Mutex = g_mutex_new ();
        new->DecodeBin = this;
        new->Pad = pad;
        new->KillSwitch = FALSE;
        new->Seek = FALSE;

        return new;
}

static gboolean                 schedule_action_for_tower (GdvDecodeBin *this, BlockAction *action,
                                                           GdvIdleTowerActionFunc action_func)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), FALSE);

        GDV_DEBUG_ELEMENT ("Scheduling one action for the tower", NULL);

        // FIXME: Use global GID's everywhere?
        gint32 id = gdv_idletower_add_item (this->Private->Tower, 1,
                                            action_func,
                                            (GdvIdleTowerDisposeFunc) action_dispose_func,
                                            (GdvIdleTowerFinalizeFunc) action_finalize_func,
                                            action);

        return (id >= 0) ? TRUE : FALSE;
}

static gboolean                 action_dispose_func (BlockAction *action)
{
        return FALSE;
}

static void                     action_finalize_func (BlockAction *action)
{
        GDV_DEBUG_EXT ("Finalizing BlockAction", NULL);
        g_mutex_free (action->Mutex);
        g_free (action);
}

static void                     perform_seek_action_func (BlockAction *action)
{
        ACTION_LOCK (action);

        GdvDecodeBin *this = action->DecodeBin;

        if (action->Seek == TRUE) {
                GDV_DEBUG_ELEMENT ("Idle action func performeing cached seek", NULL);

                if (this->Private->CachedEvent == NULL)
                        goto Failure;

                g_debug ("Locking mutext");
                g_mutex_lock (this->Private->StartMutex);
                g_debug ("Locked, waiting");
                while (this->Private->SegmentsDone == FALSE)
                        g_cond_wait (this->Private->StartCond, this->Private->StartMutex);
                g_debug ("Unlocking");
                g_mutex_unlock (this->Private->StartMutex);

                // Let's check if the event is a flushing seek. If not, add
                // the flush flag
                gdouble rate;
                GstFormat format;
                GstSeekFlags flags;
                GstSeekType curtype;
                GstSeekType stoptype;
                gint64 cur;
                gint64 stop;
                gst_event_parse_seek (this->Private->CachedEvent, &rate, &format, &flags,
                                      &curtype, &cur, &stoptype, &stop);

                flags |= GST_SEEK_FLAG_FLUSH;
                GstEvent *event = gst_event_new_seek (rate, format, flags, 
                                                      curtype, cur, 
                                                      stoptype, stop);
                g_debug ("Seeking");
                gst_pad_send_event (action->Pad, event);
                g_debug ("Seeked");
        }

        GDV_DEBUG_ELEMENT ("Unblocking pad %s", GST_PAD_NAME (action->Pad));
        if (gst_pad_set_blocked_async (action->Pad, FALSE, (GstPadBlockCallback) pad_block, action) == FALSE)
                goto Failure;
        
        goto Done;

 Failure:
        GDV_WARNING_ELEMENT ("Perform seek action failed", NULL);

 Done:
        ACTION_UNLOCK (action);
}

static gboolean                 pad_event_probe (GstPad *pad, GstEvent *event, GdvDecodeBin *this)
{
        GDV_DEBUG_ELEMENT ("Event probe for %s, event %s", GST_PAD_NAME (pad), GST_EVENT_TYPE_NAME (event));

        if (GST_EVENT_TYPE (event) != GST_EVENT_NEWSEGMENT)
                return TRUE;

        int needed = (GDV_SOURCE (this)->Protected->HasVideo && GDV_SOURCE (this)->Protected->HasAudio) ? 2 : 1;
       
        g_mutex_lock (this->Private->StartMutex);
       
        // Pass later new-segments
        if (this->Private->SegmentsDone == TRUE) {
                g_mutex_unlock (this->Private->StartMutex);
                return TRUE;
        }

        this->Private->SegmentsReceived++;
        
        if (this->Private->SegmentsReceived == needed) {
                GDV_DEBUG_ELEMENT ("All new-segments received. Signalling cond", NULL);
                this->Private->SegmentsDone = TRUE;
                g_cond_signal (this->Private->StartCond);
        }

        g_mutex_unlock (this->Private->StartMutex);

        GDV_DEBUG_ELEMENT ("Dropping event %s", GST_EVENT_TYPE_NAME (event));
        return FALSE;
}

static void                     scan_queues (GdvDecodeBin *this, GstBin *scantarget)
{
        g_return_if_fail (GDV_IS_DECODEBIN (this));
        g_return_if_fail (GST_IS_BIN (scantarget));

        GDV_DEBUG_ELEMENT ("Scanning %s for new queues", GST_OBJECT_NAME (scantarget));

        Q_LOCK (this);

        GType queuetype = g_type_from_name ("GstQueue");
        g_return_if_fail (queuetype != 0);

        GList *iter;
        for (iter = GST_BIN_CHILDREN (scantarget); iter; iter = g_list_next (iter)) {
                GstElement *element = (GstElement *) iter->data;

                if (G_TYPE_CHECK_INSTANCE_TYPE ((element), queuetype)) {
                        // This IS a queue. Let's see if we've got it already in our list
                        GSList *found = g_slist_find_custom (this->Private->QHandles, element, (GCompareFunc) queue_find_func);
                        
                        if (found == NULL) {
                                GDV_DEBUG_ELEMENT ("Adding %s to the queue handles", GST_OBJECT_NAME (element));
                                QueueHandle *handle = g_new (QueueHandle, 1);
                                handle->Queue = element;
                                handle->Exhausted = FALSE;

                                // Bind underrun signal
                                g_signal_connect (G_OBJECT (element), "underrun",
                                                  G_CALLBACK (queue_underrun), this);
                                                                
                                this->Private->QHandles = g_slist_append (this->Private->QHandles, handle);
                        }
                }
        }

        Q_UNLOCK (this);
}

static gint                     queue_find_func (const QueueHandle *data, const GstElement *element)
{
        g_return_val_if_fail (data != NULL, -1);

        if (data->Queue == element)
                return 0;
        else
                return -1;
}

static GstBusSyncReply          sync_handler (GstBus *bus, GstMessage *message,
                                              GdvDecodeBin *this)
{
        switch (GST_MESSAGE_TYPE (message)) {
                
                case GST_MESSAGE_SEGMENT_DONE: {
                        GDV_DEBUG_ELEMENT ("Got segment-done, inspecting queues", NULL);
                        
                        Q_LOCK (this);
                        
                        if (check_if_queues_exhausted (this, TRUE)) {
                                Q_UNLOCK (this);
                                GDV_DEBUG_ELEMENT ("All queues fine, passing message", NULL);
                                if (this->Private->OrigSyncHandler != NULL)
                                        return this->Private->OrigSyncHandler (bus, message, this->Private->OrigSyncHandlerData);
                        } else {
                                GDV_DEBUG_ELEMENT ("Queues still streaming, caching the message", NULL);

                                if (this->Private->CachedSegmentDone != NULL)
                                        gst_message_unref (this->Private->CachedSegmentDone);
                                
                                this->Private->CachedSegmentDone = message;
                                gst_message_ref (message);

                                this->Private->WaitingForExhaust = TRUE;
                         
                        }

                        Q_UNLOCK (this);
                        return GST_BUS_DROP;
                        
                } break;
                        
                default: 
                        if (this->Private->OrigSyncHandler != NULL)
                                return this->Private->OrigSyncHandler (bus, message, this->Private->OrigSyncHandlerData);
                        break;
        }

        return GST_BUS_DROP;
}

/* Call with a Q lock held! */
static gboolean                 check_if_queues_exhausted (GdvDecodeBin *this, gboolean probe)
{
        g_return_val_if_fail (GDV_IS_DECODEBIN (this), FALSE);

        GDV_DEBUG_ELEMENT ("Checking if queues exhausted", NULL);

        GSList *iter;
        for (iter = this->Private->QHandles; iter; iter = g_slist_next (iter)) {
                QueueHandle *handle = iter->data;
                g_assert (handle != NULL);
                
                if (handle->Exhausted == FALSE && probe == TRUE) {
                        gint32 level;
                        // Check if filled
                        g_object_get (G_OBJECT (handle->Queue), "current-level-buffers", &level, NULL);
                        handle->Exhausted = (level == 0) ? TRUE : FALSE;
                }

                if (handle->Exhausted == FALSE)
                        return FALSE;
        }

        return TRUE;
}

static void                     queue_underrun (GstElement *queue, GdvDecodeBin *this)
{
        g_return_if_fail (GST_IS_ELEMENT (queue));

        Q_LOCK (this);
        if (this->Private->WaitingForExhaust == FALSE)
                goto done;

        GDV_DEBUG_ELEMENT ("%s was underrun and we're waiting for exhaust. Checking if all.", GST_OBJECT_NAME (queue));

        GSList *found = g_slist_find_custom (this->Private->QHandles, queue, (GCompareFunc) queue_find_func);
        
        if (found == NULL) {
                GDV_WARNING_ELEMENT ("Exhausted queue %s is not a one we controll!!", GST_OBJECT_NAME (queue));
                goto done;
        }

        QueueHandle *handle = found->data;
        g_assert (handle != NULL);

        handle->Exhausted = TRUE;

        if (check_if_queues_exhausted (this, FALSE)) {
                GDV_DEBUG_ELEMENT ("All queues exhausted now. Pushing the cached segment-done message", NULL);

                if (this->Private->CachedSegmentDone == NULL) {
                        GDV_WARNING_ELEMENT ("No cached segment-done message!", NULL);
                        goto done;
                }

                GstMessage *message = this->Private->CachedSegmentDone;
                this->Private->CachedSegmentDone = NULL;
                this->Private->WaitingForExhaust = FALSE;
                
                Q_UNLOCK (this);

                gst_bus_post (GST_BIN (this)->child_bus, message);
                gst_message_unref (message);
        }

 done:
        Q_UNLOCK (this);
}
