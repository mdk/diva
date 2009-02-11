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

#include "GdvClip.h"
#include "GdvClipPrivate.h"

static GstBinClass*             ParentClass = NULL;

/* GObject type */
GType                           gdv_clip_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvClipClass),
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_clip_class_init,
                        NULL,
                        NULL, 
                        sizeof (GdvClip), 
                        32, 
                        (GInstanceInitFunc) gdv_clip_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvClip",
                                                     &objectinfo, 0);
        }
        return objecttype;
} 

/* GObject class init*/
static void                     gdv_clip_class_init (GdvClipClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) klass;
        
        gobjectclass->get_property = (gpointer) gdv_clip_get_property;
        gobjectclass->set_property = (gpointer) gdv_clip_set_property;        
                
        g_object_class_install_property (gobjectclass, ARG_PARENTITEM,
                                         g_param_spec_object
                                         ("parentitem", "ParentItem",
                                          "Parent MediaItem of this clip",
                                          GDV_TYPE_MEDIAITEM,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_PARENTTRACK,
                                         g_param_spec_object
                                         ("track", "Track",
                                          "Parent track",
                                          GDV_TYPE_TRACK,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_SOURCE,
                                         g_param_spec_object
                                         ("source", "Source",
                                          "The Source of this clip",
                                          GDV_TYPE_SOURCE,
                                          G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_TIMELINEIN,
                                         g_param_spec_uint64
                                         ("timelinein", "TimelineIn",
                                          "The In point of this clip on the timeline",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_TIMELINEOUT,
                                         g_param_spec_uint64
                                         ("timelineout", "TimelineOut",
                                          "The Out point of this clip on the timeline",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_MINTIMELINEIN,
                                         g_param_spec_uint64
                                         ("mintimelinein", "MinTimelineIn",
                                          "The minimum In point of this clip (while keeping current Out point)",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_MAXTIMELINEOUT,
                                         g_param_spec_uint64
                                         ("maxtimelineout", "MaxTimelineOut",
                                          "The maximum Out point of this clip (while keeping current In point)",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_TIMELINESPAN,
                                         g_param_spec_boxed
                                         ("timelinespan", "TimelineSpan",
                                          "The TimeSpan of this clip on the timeline",
                                          GDV_TYPE_TIMESPAN, G_PARAM_READWRITE));
        
        g_object_class_install_property (gobjectclass, ARG_SOURCESPAN,
                                         g_param_spec_boxed
                                         ("sourcespan", "SourceTimeSpan",
                                          "The TimeSpan of this clip in the source",
                                          GDV_TYPE_TIMESPAN, G_PARAM_READWRITE));
        
        g_object_class_install_property (gobjectclass, ARG_HASAUDIO,
                                         g_param_spec_boolean
                                         ("hasaudio", "HasAudio",
                                          "If this clip has audio",
                                          FALSE, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_HASVIDEO,
                                         g_param_spec_boolean
                                         ("hasvideo", "HasVideo",
                                          "If this clip has video",
                                          FALSE, G_PARAM_READABLE));

        // Signals
        klass->ChangedSignalId =
                g_signal_newv ("changed",
                               GDV_TYPE_CLIP,
                               G_SIGNAL_RUN_LAST,
                               NULL,
                               NULL,
                               NULL,
                               g_cclosure_marshal_VOID__VOID,
                               G_TYPE_NONE,
                               0,
                               NULL);
                
        // FIXME Really needed?
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
        gobjectclass->dispose = (gpointer) gdv_clip_dispose;
        gobjectclass->finalize = (gpointer) gdv_clip_finalize;
        elementclass->send_event = (gpointer) send_event;
        elementclass->change_state = (gpointer) change_state;
}

/* GObject init */
static void                     gdv_clip_init (GdvClip *this)
{
        GDV_DEBUG_EXT ("Initializing fresh GdvClip object", NULL);

        g_return_if_fail (GDV_IS_CLIP (this));
        this->Private = NULL;
        
        GdvClipPrivate *priv = g_new0 (GdvClipPrivate, 1);
        g_return_if_fail (priv != NULL);

        if (private_init (this, priv) == TRUE) 
                this->Private = priv;
        else {
                //private_dispose (this, priv);
                
                g_free (priv);
                //g_free (prot);
                this->Private = NULL;
                //this->Protected = NULL;
        }
}

/* GObject set property */
static void                     gdv_clip_set_property (GdvClip *this, guint propid, 
                                                       GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_CLIP (this));

        switch (propid) {
                
                case ARG_TIMELINESPAN:
                        if (this->Private->TimelineSpan != NULL)
                                gdv_timespan_free (this->Private->TimelineSpan);
                        
                        this->Private->TimelineSpan = g_value_dup_boxed (value);

                        // Emit the signal
                        update_gates (this);
                        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                                       0, NULL);
                        break;

                case ARG_SOURCESPAN:
                        if (this->Private->SourceSpan != NULL)
                                gdv_timespan_free (this->Private->SourceSpan);
                        
                        this->Private->SourceSpan = g_value_dup_boxed (value);

                        // Emit the signal
                        update_gates (this);
                        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                                       0, NULL);
                        break;                        
                        
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (this, propid, pspec);
                        break;
        }
}

/* GObject get property */
void                            gdv_clip_get_property (GdvClip *this, guint propid, 
                                                       GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_CLIP (this));
        
        switch (propid) {
                
                case ARG_HASVIDEO: {
                        gboolean hasvideo;
                        g_object_get (G_OBJECT (this->Private->Source), "hasvideo", &hasvideo, NULL);
                        g_value_set_boolean (value, hasvideo);
                } break;
                
                case ARG_HASAUDIO: {
                        gboolean hasaudio;
                        g_object_get (G_OBJECT (this->Private->Source), "hasaudio", &hasaudio, NULL);
                        g_value_set_boolean (value, hasaudio);
                } break;
                                
                case ARG_TIMELINESPAN:
                        g_return_if_fail (this->Private->TimelineSpan != NULL);
                        g_value_set_boxed (value, this->Private->TimelineSpan);
                        break;
                
                case ARG_SOURCESPAN:
                        g_return_if_fail (this->Private->SourceSpan != NULL);
                        g_value_set_boxed (value, this->Private->SourceSpan);
                        break;
                
                case ARG_TIMELINEIN:
                        g_return_if_fail (this->Private->TimelineSpan != NULL);
                        g_value_set_uint64 (value, this->Private->TimelineSpan->Start);
                        break;
                
                case ARG_TIMELINEOUT:
                        g_return_if_fail (this->Private->TimelineSpan != NULL);
                        g_value_set_uint64 (value, this->Private->TimelineSpan->End);
                        break;
                
                case ARG_PARENTITEM:
                        g_return_if_fail (this->Private->ParentItem != NULL);
                        g_value_set_object (value, this->Private->ParentItem);
                        break;

                case ARG_PARENTTRACK: {
                        GdvTrack *track = GDV_TRACK (GST_ELEMENT_PARENT (this));
                        g_value_set_object (value, track);
                } break;
                
                case ARG_SOURCE:
                        g_return_if_fail (this->Private->Source != NULL);
                        g_value_set_object (value, this->Private->Source);
                        break;
                
                case ARG_MINTIMELINEIN: {
                        g_return_if_fail (this->Private->TimelineSpan != NULL);
                        g_return_if_fail (this->Private->SourceSpan != NULL);
                        g_return_if_fail (this->Private->ParentItem != NULL);
                        
                        if (! GDV_IS_TIMEABLE (this->Private->ParentItem)) {
                                g_value_set_uint64 (value, GDV_ZEROTIME);
                                return;
                        }
                        
                        if (gdv_timespan_is_empty (this->Private->TimelineSpan)) {
                                GDV_WARNING_EXT ("Timeline TimeSpan for clip is empty!", NULL);
                                g_value_set_uint64 (value, GDV_ZEROTIME);
                                break;
                        }
                        
                        if (gdv_timespan_is_empty (this->Private->SourceSpan)) {
                                GDV_WARNING_EXT ("Source TimeSpan for clip is empty!", NULL);
                                g_value_set_uint64 (value, GDV_ZEROTIME);
                                break;
                        }
                        
                        // Calculate
                        if (this->Private->SourceSpan->Start > this->Private->TimelineSpan->Start)
                                g_value_set_uint64 (value, GDV_ZEROTIME);
                        else
                                g_value_set_uint64 (value, this->Private->TimelineSpan->Start - 
                                                    this->Private->SourceSpan->Start);
                        
                        break;
                }
                
                case ARG_MAXTIMELINEOUT: {
                        g_return_if_fail (this->Private->TimelineSpan != NULL);
                        g_return_if_fail (this->Private->SourceSpan != NULL);
                        g_return_if_fail (this->Private->ParentItem != NULL);
                        
                        if (! GDV_IS_TIMEABLE (this->Private->ParentItem)) {
                                g_value_set_uint64 (value, GDV_INFINITY);
                                return;
                        }
                        
                        if (gdv_timespan_is_empty (this->Private->TimelineSpan)) {
                                GDV_WARNING_EXT ("Timeline TimeSpan for clip is empty!", NULL);
                                g_value_set_uint64 (value, GDV_INFINITY);
                                break;
                        }
                        
                        if (gdv_timespan_is_empty (this->Private->SourceSpan)) {
                                GDV_WARNING_EXT ("Source TimeSpan for clip is empty!", NULL);
                                g_value_set_uint64 (value, GDV_INFINITY);
                                break;
                        }
                        
                        // Calculate
                        GdvTime sourcelength = GDV_ZEROTIME;
                        g_object_get (G_OBJECT (this->Private->ParentItem), "length", &sourcelength, NULL);
                        g_assert (sourcelength != GDV_ZEROTIME);
                        g_assert (sourcelength >= this->Private->SourceSpan->End);
                        
                        g_value_set_uint64 (value, this->Private->TimelineSpan->End + 
                                            sourcelength - this->Private->SourceSpan->End);
                        
                        break;
                }

                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (this, propid, pspec);
                        break;
        }
}

/* GObject dispose */
static void                     gdv_clip_dispose (GdvClip *this)
{
        /*
        g_assert (GDV_IS_CLIP (this));

        GDV_DEBUG_EXT ("Disposing...", NULL);

        if (this->_TimelineSpan != NULL)
                gdv_timespan_free (this->_TimelineSpan);
        
        if (this->_SourceSpan != NULL)
                gdv_timespan_free (this->_SourceSpan);
        
        if (this->_ParentItem != NULL)
                g_object_unref (G_OBJECT (this->_ParentItem));
        */
}

/* Place the clip at the given position. This will be (supposingly) most usefull for
 * initial clip placements. Source In is set to 0 and Out is set accordingly to duration */
gboolean                        gdv_clip_place_at_timespan (GdvClip *this, GdvTimeSpan *timespan)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (timespan != NULL, FALSE);
        g_return_val_if_fail (! gdv_timespan_is_empty (timespan), FALSE);
        
        GDV_DEBUG_EXT ("Placing clip at %s", gdv_timespan_to_string (timespan));
        
        if (GDV_IS_TIMEABLE (this->Private->ParentItem)) {
                // Extra checking
                GDV_DEBUG_EXT ("Checking if length is okay with parent MediaItem", NULL);
                g_return_val_if_fail (this->Private->ParentItem != NULL, FALSE);
                
                GdvTime sourcelength = GDV_ZEROTIME;
                g_object_get (G_OBJECT (this->Private->ParentItem), "length", &sourcelength, NULL);
                g_return_val_if_fail (sourcelength != GDV_ZEROTIME, FALSE);
                
                if (gdv_timespan_duration (timespan) > sourcelength) {
                        GDV_WARNING_EXT ("TimeSpan duration longer than parent MediaItem duration! "
                                         "Can't place clip", NULL);
                        return FALSE;
                }
                
                // Okay
        }
        
        // FIXME: Freeze/Thaw GObject notify!
         
        // Timeline span
        if (this->Private->TimelineSpan != NULL)
                gdv_timespan_free (this->Private->TimelineSpan);
        
        this->Private->TimelineSpan = gdv_timespan_copy (timespan);
        
        // Source span
        if (this->Private->SourceSpan != NULL)
                gdv_timespan_free (this->Private->SourceSpan);
        
        this->Private->SourceSpan = gdv_timespan_new (GDV_ZEROTIME, gdv_timespan_duration (timespan));
        
        GDV_DEBUG_EXT ("Clip placed, new duration: %.2f", gdv_time_to_seconds 
                       (gdv_timespan_duration (timespan)));

        // Emit the signal
        update_gates (this);
        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                       0, NULL);

        return TRUE;
}

/* Move a clip to a given time */
gboolean                        gdv_clip_move_to (GdvClip *this, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (this->Private->TimelineSpan != NULL, FALSE);

        GDV_DEBUG_ELEMENT ("Moving clip to %s", gdv_time_to_string (time));
        
        GdvTimeSpan *span = gdv_timespan_new (time,
                                              time + gdv_timespan_duration (this->Private->TimelineSpan));

        if (this->Private->TimelineSpan != NULL)
                gdv_timespan_free (this->Private->TimelineSpan);

        this->Private->TimelineSpan = span;

        // Emit the signal
        update_gates (this);
        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                       0, NULL);

        return TRUE;
}

/* Place at the given time. Works/calls place_at . The while clip span is positioned
 * on the timeline */
gboolean                        gdv_clip_place_at_time (GdvClip *this, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (this->Private->Source != NULL, FALSE);
        g_return_val_if_fail (this->Private->ParentItem != NULL, FALSE);
        
        GDV_DEBUG_EXT ("Placing clip at %s", gdv_time_to_string (time));
        
        GdvTimeSpan *span = NULL;
        
        if (GDV_IS_TIMEABLE (this->Private->ParentItem)) { 
                // Get duration
                GdvTime sourcelength;
                g_object_get (G_OBJECT (this->Private->ParentItem), "length", &sourcelength, NULL);
                span = gdv_timespan_new (time, time + sourcelength);
        } else {
                // FIXME: Use a defined GDV_DEFAULT_DURATION or something 
                span = gdv_timespan_new (time, time + GDV_SECOND * 5);
        }
                
        gboolean ret = gdv_clip_place_at_timespan (this, span);
        
        gdv_timespan_free (span);
        
        return ret;
}

gboolean                        gdv_clip_left_adjust_to (GdvClip *this, GdvTime newtime)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (this->Private->TimelineSpan != NULL, FALSE);
        g_return_val_if_fail (this->Private->SourceSpan != NULL, FALSE);        


        GDV_DEBUG_ELEMENT ("Left adjusting clip to %s", gdv_time_to_string (newtime));

        if (! GDV_IS_TIMEABLE (this->Private->ParentItem)) {
                this->Private->TimelineSpan->Start = newtime;
                this->Private->SourceSpan->Start = GDV_ZEROTIME;
                this->Private->SourceSpan->End = gdv_timespan_duration (this->Private->TimelineSpan);
                goto done;
        }

        if (newtime < this->Private->TimelineSpan->Start) {
                GdvTime mintimelinein;
                g_object_get (G_OBJECT (this), "mintimelinein", &mintimelinein, NULL);
                g_return_val_if_fail (newtime >= mintimelinein, FALSE);

                GdvTime extension = this->Private->TimelineSpan->Start - newtime;
                this->Private->SourceSpan->Start -= extension;
                this->Private->TimelineSpan->Start -= extension;
        } else {
                g_return_val_if_fail (newtime < this->Private->TimelineSpan->End , FALSE);

                GdvTime cuttage = newtime - this->Private->TimelineSpan->Start;
                this->Private->SourceSpan->Start += cuttage;
                this->Private->TimelineSpan->Start += cuttage;
        }

 done:

        // Emit the signal
        update_gates (this);
        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                       0, NULL);
        return TRUE;
}

gboolean                        gdv_clip_right_adjust_to (GdvClip *this, GdvTime newtime)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (this->Private->TimelineSpan != NULL, FALSE);
        g_return_val_if_fail (this->Private->SourceSpan != NULL, FALSE);        
        
        GDV_DEBUG_ELEMENT ("Right adjusting clip to %s", gdv_time_to_string (newtime));

        if (! GDV_IS_TIMEABLE (this->Private->ParentItem)) {
                this->Private->TimelineSpan->End = newtime;
                this->Private->SourceSpan->Start = GDV_ZEROTIME;
                this->Private->SourceSpan->End = gdv_timespan_duration (this->Private->TimelineSpan);
                goto done;
        }

        if (newtime > this->Private->TimelineSpan->End) {
                GdvTime maxtimelineout;
                g_object_get (G_OBJECT (this), "maxtimelineout", &maxtimelineout, NULL);
                g_return_val_if_fail (newtime <= maxtimelineout, FALSE);

                GdvTime extension = newtime - this->Private->TimelineSpan->End;
                this->Private->SourceSpan->End += extension;
                this->Private->TimelineSpan->End += extension;
        } else {
                g_return_val_if_fail (newtime > this->Private->TimelineSpan->Start, FALSE);

                GdvTime cuttage = this->Private->TimelineSpan->End - newtime;
                this->Private->SourceSpan->End -= cuttage;
                this->Private->TimelineSpan->End -= cuttage;
        }

 done:
        // Emit the signal
        update_gates (this);
        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                       0, NULL);
        return TRUE;
}

/* Create a new instance. */
GdvClip*                        gdv_clip_new_explicit (GdvSource *source, GdvMediaItem *item)
{
        g_return_val_if_fail (GDV_IS_MEDIAITEM (item), NULL);
        g_return_val_if_fail (GDV_IS_SOURCE (source), NULL);

        GDV_DEBUG_EXT ("Creating new clip for source '%s'",
                       GST_OBJECT_NAME (source));
        
        GdvClip *this = g_object_new (GDV_TYPE_CLIP, NULL);
        if (this == NULL)
                return NULL;
        
        this->Private->Source = source;
        this->Private->ParentItem = item;
        gst_object_ref (source);
        g_object_ref (item);

        // Add clip to the bin
        GDV_DEBUG_EXT ("Adding the source to the bin", NULL);
        if (gst_bin_add (GST_BIN (this), GST_ELEMENT (source)) == FALSE)
                goto failure;
        
        // Create the pads
        gboolean hasvideo, hasaudio;
        GDV_DEBUG_EXT ("Creating gates", NULL);
        g_object_get (G_OBJECT (source), "hasvideo", &hasvideo, "hasaudio", &hasaudio, NULL);

        // Has video
        if (hasvideo == TRUE) {
                this->Private->VGate = gdv_clipgate_new (GDV_FLAVOR_VIDEO);
                
                if (this->Private->VGate == NULL)
                        goto failure;

                if (gst_bin_add (GST_BIN (this), GST_ELEMENT (this->Private->VGate)) == FALSE)
                        goto failure;

                if (gst_element_link_pads (GST_ELEMENT (this->Private->Source),
                                           "vsrc",
                                           GST_ELEMENT (this->Private->VGate),
                                           "sink") == FALSE)
                        goto failure;

                this->Private->VSrc = gst_ghost_pad_new ("vsrc",
                                                         gst_element_get_pad (GST_ELEMENT (this->Private->VGate), "src"));

                if (this->Private->VSrc == NULL)
                        goto failure;
                
                gst_element_add_pad (GST_ELEMENT (this), GST_PAD (this->Private->VSrc));
        }

        // Has audio
        if (hasaudio == TRUE) {
                this->Private->AGate = gdv_clipgate_new (GDV_FLAVOR_AUDIO);
                
                if (this->Private->AGate == NULL)
                        goto failure;

                if (gst_bin_add (GST_BIN (this), GST_ELEMENT (this->Private->AGate)) == FALSE)
                        goto failure;

                if (gst_element_link_pads (GST_ELEMENT (this->Private->Source),
                                           "asrc",
                                           GST_ELEMENT (this->Private->AGate),
                                           "sink") == FALSE)
                        goto failure;

                this->Private->ASrc = gst_ghost_pad_new ("asrc",
                                                         gst_element_get_pad (GST_ELEMENT (this->Private->AGate), "src"));

                if (this->Private->ASrc == NULL)
                        goto failure;
                
                gst_element_add_pad (GST_ELEMENT (this), GST_PAD (this->Private->ASrc));
        }
        
        // The Bus
        GDV_DEBUG_ELEMENT ("Setting the bus sync handler", NULL);
        GstBus *bus = GST_BIN (this)->child_bus;
        g_assert (bus != NULL);
        
        GST_OBJECT_LOCK (bus);
        
        this->Private->OrigSyncHandler = bus->sync_handler;
        this->Private->OrigSyncHandlerData = bus->sync_handler_data;
        
        bus->sync_handler = (GstBusSyncHandler) sync_handler;
        bus->sync_handler_data = (gpointer) this;

        GST_OBJECT_UNLOCK (bus);
        
        // Default placing
        gdv_clip_place_at_time (this, GDV_ZEROTIME);

        // Update the gates
        update_gates (this);

        return this;
        
        failure:
        if (this != NULL)
                g_object_unref (G_OBJECT (this));
        
        return NULL;
}
/* Create a new instance. */
GdvClip*                        gdv_clip_new (GdvMediaItem *item)
{
        g_return_val_if_fail (GDV_IS_MEDIAITEM (item), NULL);

        // Get the source
        GDV_DEBUG_EXT ("Getting fresh source for item", NULL);
        GdvSource *source;
        source = gdv_mediaitem_get_fresh_source (item);
        g_return_val_if_fail (source != NULL, NULL);

        return gdv_clip_new_explicit (source, item);
}

/* Convert clip (object) time to source time */
GdvTime                         gdv_clip_time_to_source_time (GdvClip *this, GdvTime cliptime)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), GDV_ZEROTIME);
        g_return_val_if_fail (this->Private->TimelineSpan != NULL, GDV_ZEROTIME);
        g_return_val_if_fail (this->Private->SourceSpan != NULL, GDV_ZEROTIME);
        
        if (cliptime + this->Private->SourceSpan->Start < this->Private->TimelineSpan->Start) {
                GDV_WARNING_ELEMENT ("%s time out of bounds!", gdv_time_to_string (cliptime));
                return GDV_ZEROTIME;
        }

        return cliptime + this->Private->SourceSpan->Start - this->Private->TimelineSpan->Start;
}

/* Convert source time to clip (object) time */
GdvTime                         gdv_clip_source_time_to_time (GdvClip *this, GdvTime sourcetime)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), GDV_ZEROTIME);
        g_return_val_if_fail (this->Private->TimelineSpan != NULL, GDV_ZEROTIME);
        g_return_val_if_fail (this->Private->SourceSpan != NULL, GDV_ZEROTIME);

        if (sourcetime > this->Private->SourceSpan->End || sourcetime < this->Private->SourceSpan->Start) {
                GDV_WARNING_ELEMENT ("%s time out of bounds!", gdv_time_to_string (sourcetime));
                return GDV_ZEROTIME;
        }
        
        return this->Private->TimelineSpan->Start + sourcetime - this->Private->SourceSpan->Start;
}

/* Translate a SEGMENT_DONE message */
GstMessage*                     translate_segment_done (GdvClip *this, GstMessage *message)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), NULL);
        g_return_val_if_fail (message != NULL, NULL);
        
        GstFormat format;
        gint64 val;
        
        gst_message_parse_segment_done (message, &format, &val);
        
        if (format != GST_FORMAT_TIME) {
                GstFormat format2 = GST_FORMAT_TIME;
                gint64 val2;

                gboolean hasaudio;
                gboolean hasvideo;
                g_object_get (this->Private->Source, "hasvideo", &hasvideo, "hasaudio", &hasaudio, NULL);

                GstPad *pad = (GstPad *) ((hasvideo == TRUE) ? this->Private->VSrc : this->Private->ASrc);
                
                gboolean l1 = gst_pad_query_convert (gst_ghost_pad_get_target ((GstGhostPad *) pad), format, val,
                                                     &format2, &val2);
                
                if (l1 == FALSE || format2 != GST_FORMAT_TIME) {
                        GDV_WARNING_ELEMENT ("Got non-time event and Failed to convert formats!", NULL);
                        return message;
                } else
                        val = val2;
        }

        GdvTime time = gdv_clip_source_time_to_time (this, (guint64) val);
        GDV_DEBUG_ELEMENT ("Translating SEGMENT_DONE at %s to %s",
                           gdv_time_to_string ((guint64) val), gdv_time_to_string (time));
        
        GstMessage *outmessage = gst_message_new_segment_done (GST_MESSAGE_SRC (message),
                                                               GST_FORMAT_TIME,
                                                               (gint64) time);
        g_return_val_if_fail (outmessage != NULL, message);

        gst_message_unref (message);
        return outmessage;
}

/* The sync handler for this clip. It's fired when we receive messages on the bus */
static GstBusSyncReply          sync_handler (GstBus *bus, GstMessage *message,
                                              GdvClip *this)
{
        switch (GST_MESSAGE_TYPE (message)) {
                
                case GST_MESSAGE_SEGMENT_DONE: 
                        GDV_DEBUG_ELEMENT ("Translating segment_done message", NULL);
                        message = translate_segment_done (this, message);

                        if (this->Private->OrigSyncHandler != NULL)
                                return this->Private->OrigSyncHandler (bus, message, this->Private->OrigSyncHandlerData);
                        
                        break;
                        
                default: 
                        if (this->Private->OrigSyncHandler != NULL)
                                return this->Private->OrigSyncHandler (bus, message, this->Private->OrigSyncHandlerData);
                        break;
        }

        return GST_BUS_DROP;
}

/* A GStreamer send_event. We translate the seek event and pass it on to the source */
static gboolean                 send_event (GdvClip *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (event != NULL, FALSE);
        
        gboolean ret = TRUE;

        GDV_DEBUG_ELEMENT ("Event: %s", GST_EVENT_TYPE_NAME (event));

        switch (GST_EVENT_TYPE (event)) {

                case GST_EVENT_SEEK: {
                        GdvClipGate *gate = (this->Private->VGate != NULL) ?
                                this->Private->VGate :
                                this->Private->AGate;
                        
                        event = gdv_clipgate_translate_seek_event (gate, event);
                        ret = gst_element_send_event (GST_ELEMENT (this->Private->Source), event);
                } break;

                default: {
                        ret = gst_element_send_event (GST_ELEMENT (this->Private->Source), event);
                        goto done;
                }

        }

        done:
        return ret;
}

void                            gdv_clip_force_changed_signal (GdvClip *this)
{
        g_return_if_fail (GDV_IS_CLIP (this));
        
        // Emit the signal
        g_signal_emit (this, GDV_CLIP_GET_CLASS (this)->ChangedSignalId,
                       0, NULL);
}

/* GObject finalize */
static void                     gdv_clip_finalize (GdvClip *this)
{
        g_return_if_fail (GDV_IS_CLIP (this));
}

static gboolean                 private_init (GdvClip *this, GdvClipPrivate *private)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);

        private->Source = NULL;
        private->ParentItem = NULL;
        private->VGate = NULL;
        private->AGate = NULL;
        private->VSrc = NULL;
        private->ASrc = NULL;
        private->TimelineSpan = NULL;
        private->SourceSpan = NULL;
        private->OrigSyncHandlerData = NULL;
        private->OrigSyncHandler = NULL;
        private->Timeable = FALSE;
        
        return TRUE;
}

static GstStateChangeReturn     change_state (GdvClip *this, GstStateChange transition)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), GST_STATE_CHANGE_FAILURE);
        GDV_DEBUG_ELEMENT ("Changing state", NULL);
        
        GstStateChangeReturn ret;

        
        switch (transition) {
                
                default: 
                break;
        }

        ret = GST_ELEMENT_CLASS (ParentClass)->change_state (GST_ELEMENT (this), 
                                                             transition);

        switch (transition) {
                
                case GST_STATE_CHANGE_PAUSED_TO_READY: { 
                        gst_element_send_event (GST_ELEMENT (this->Private->Source), get_full_seek_event (this));
                } break;

                default: 
                break;
        }
                        
        return ret;
}

static GstEvent*                get_full_seek_event (GdvClip *this)
{
        g_return_val_if_fail (GDV_IS_CLIP (this), NULL);
        g_return_val_if_fail (this->Private->SourceSpan != NULL, NULL);
        
        GDV_DEBUG_ELEMENT ("Getting a full segment seek covering clip (%s)",
                           gdv_timespan_to_string (this->Private->SourceSpan));

        return gst_event_new_seek (1.0, GST_FORMAT_TIME,
                                   GST_SEEK_FLAG_SEGMENT | GST_SEEK_FLAG_FLUSH,
                                   GST_SEEK_TYPE_SET, gdv_time_get_int64 (this->Private->SourceSpan->Start),
                                   GST_SEEK_TYPE_SET, gdv_time_get_int64 (this->Private->SourceSpan->End));
}

static void                     update_gates (GdvClip *this)
{
        g_return_if_fail (GDV_IS_CLIP (this));

        if (this->Private->VGate != NULL) 
                gdv_clipgate_set_stream (this->Private->VGate,
                                         this->Private->SourceSpan->Start,
                                         this->Private->TimelineSpan->Start);

        if (this->Private->AGate != NULL) 
                gdv_clipgate_set_stream (this->Private->AGate,
                                         this->Private->SourceSpan->Start,
                                         this->Private->TimelineSpan->Start);
}

        
