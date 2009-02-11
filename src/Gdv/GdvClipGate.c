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

#include "GdvClipGate.h"
#include "GdvClipGatePrivate.h"

static GstBaseTransformClass*   ParentClass = NULL;

static GstStaticPadTemplate     SinkFactory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                       GST_PAD_SINK,
                                                                       GST_PAD_ALWAYS,
                                                                       GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate     SrcFactory = GST_STATIC_PAD_TEMPLATE  ("src",
                                                                       GST_PAD_SRC,
                                                                       GST_PAD_ALWAYS,
                                                                       GST_STATIC_CAPS_ANY);

/* Base init to all the BaseTransform pads */
void                            gdv_clipgate_base_init (gpointer klass)
{
        GstElementClass *elementclass = GST_ELEMENT_CLASS (klass);
        
        gst_element_class_add_pad_template (elementclass, 
                                            gst_static_pad_template_get (&SinkFactory));
        
        gst_element_class_add_pad_template (elementclass,
                                            gst_static_pad_template_get (&SrcFactory));
}

/* GObject type */
GType                           gdv_clipgate_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvClipGateClass), 
                        gdv_clipgate_base_init, 
                        NULL,
                        (GClassInitFunc) gdv_clipgate_class_init,
                        NULL,
                        NULL, 
                        sizeof (GdvClipGate), 
                        4, 
                        (GInstanceInitFunc) gdv_clipgate_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BASE_TRANSFORM, "GdvClipGate",
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_clipgate_class_init (GdvClipGateClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstBaseTransformClass *basetransformclass = (GstBaseTransformClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) klass;

        gobjectclass->dispose = (gpointer) gdv_clipgate_dispose;
        gobjectclass->finalize = (gpointer) gdv_clipgate_finalize;

        basetransformclass->prepare_output_buffer = (gpointer) prepare_output_buffer;
        basetransformclass->transform_ip = (gpointer) transform_ip; 

        elementclass->change_state = (gpointer) change_state;
        
        ParentClass = g_type_class_ref (GST_TYPE_BASE_TRANSFORM);
}

/* GObject init */
static void                     gdv_clipgate_init (GdvClipGate *this)
{
        g_return_if_fail (GDV_IS_CLIPGATE (this));
        GDV_DEBUG_ELEMENT ("Initializing new GdvClipGate", NULL);

        this->Private = NULL;

        GdvClipGatePrivate *priv = g_new0 (GdvClipGatePrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE) 
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
                this->Private = NULL;
        }
}

static gboolean                 private_init (GdvClipGate *this, GdvClipGatePrivate *private)
{
        g_return_val_if_fail (GDV_IS_CLIPGATE (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);

        private->SourceIn = 0;
        private->TimelineIn = 0;
        
        private->Mutex = g_mutex_new ();
        g_return_val_if_fail (private->Mutex != NULL, FALSE);
        
        private->Flavor = GDV_FLAVOR_NONE;

        private->SinkOrigEventFunction = GST_BASE_TRANSFORM (this)->sinkpad->eventfunc;
        gst_pad_set_event_function (GST_BASE_TRANSFORM (this)->sinkpad,
                                    (gpointer) sink_pad_event_func);

        private->Killer = TRUE;
        
        return TRUE;
}

static void                     private_dispose (GdvClipGatePrivate *private)
{
        g_return_if_fail (private != NULL);

        if (private->Mutex != NULL) {
                g_mutex_free (private->Mutex);
                private->Mutex = NULL;
        }
}

/* GObject dispose */
static void                     gdv_clipgate_dispose (GdvClipGate *this)
{
        g_return_if_fail (GDV_IS_CLIPGATE (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this->Private);

        // FIXME: Wrong dispose
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_clipgate_finalize (GdvClipGate *this)
{
        g_return_if_fail (GDV_IS_CLIPGATE (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }

        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Here we actually modify the metadata of the buffer */
static GstFlowReturn            prepare_output_buffer (GdvClipGate *this, GstBuffer *input,
                                                       gint size, GstCaps *caps, GstBuffer **buf)
{
        g_return_val_if_fail (input != NULL, GST_FLOW_ERROR);
        g_return_val_if_fail (GST_BUFFER_CAPS (input) != NULL, GST_FLOW_ERROR);
        g_return_val_if_fail (GDV_IS_CLIPGATE (this), GST_FLOW_ERROR);
        
        gst_buffer_ref (input);
        
        GATE_LOCK (this);

        // Disable the kill switch
        this->Private->Killer = FALSE;
        
        // Get the current time/timestamp
        GdvTime buffertime = GST_BUFFER_TIMESTAMP (input);
        GdvTime newbuffertime = buffertime - this->Private->SourceIn + this->Private->TimelineIn;
        
        GDV_DEBUG_ELEMENT ("Restamping buffer from %s to %s",
                           gdv_time_to_string (buffertime), gdv_time_to_string (newbuffertime));
        
        GST_BUFFER_TIMESTAMP (input) = newbuffertime;
        
        *buf = input;
        
        if (this->Private->Flavor == GDV_FLAVOR_VIDEO) {
                if (GDV_IS_VIDEOBUFFER ((GdvVideoBuffer *) input))
                        *buf = input;
                else
                        *buf = (GstBuffer *) gdv_videobuffer_new_from_gst (input);
        }
        
        if (this->Private->Flavor == GDV_FLAVOR_AUDIO) {
                if (GDV_IS_AUDIOBUFFER ((GdvAudioBuffer *) input))
                        *buf = input;
                else
                        *buf = (GstBuffer *) gdv_audiobuffer_new_from_gst (input);
        }
                
        GATE_UNLOCK (this);
        
        return GST_FLOW_OK;
}

/* A stubby function just to force the _prepare_output_buffer func */
static GstFlowReturn            transform_ip (GdvClipGate *this, GstBuffer *buf)
{
        // No work here. It's all done in _prepare_output_buffer.
        return GST_FLOW_OK;
}

/* Create a new instance */
GdvClipGate*                    gdv_clipgate_new (GdvFlavor flavor)
{
        GdvClipGate *this = g_object_new (GDV_TYPE_CLIPGATE, NULL);
        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }
        
        this->Private->Flavor = flavor;
        return this;
}

static gboolean                 sink_pad_event_func (GstPad *pad, GstEvent *event)
{
        GdvClipGate *this = (GdvClipGate *) gst_pad_get_parent (pad);
        g_return_val_if_fail (GDV_IS_CLIPGATE (this), FALSE);
        
        GDV_DEBUG_ELEMENT ("'sink' pad event: %s", GST_EVENT_TYPE_NAME (event));
        gboolean ret; 
        
        switch (GST_EVENT_TYPE (event)) {

                case GST_EVENT_FLUSH_START: {
                        GATE_LOCK (this);
                        
                        if (this->Private->Killer == TRUE) 
                                GDV_DEBUG_ELEMENT ("KILLING FLUSH-START", NULL);
                        else
                                ret = gst_pad_event_default (pad, event);

                        GATE_UNLOCK (this);
                } break;

                case GST_EVENT_FLUSH_STOP: {
                        GATE_LOCK (this);

                        if (this->Private->Killer == TRUE) 
                                GDV_DEBUG_ELEMENT ("KILLING FLUSH-STOP", NULL);
                        else
                                ret = gst_pad_event_default (pad, event);

                        GATE_UNLOCK (this);
                } break;
                
                case GST_EVENT_NEWSEGMENT: {
                        GDV_DEBUG_ELEMENT ("Translating new-segment event", NULL);
                        event = gdv_clipgate_translate_new_segment (this, event);
                        ret = this->Private->SinkOrigEventFunction (pad, event);
                } break;
                
                default: {
                        // Chain to the parent
                        ret = this->Private->SinkOrigEventFunction (pad, event);
                } break;
                        
        }
        
        gst_object_unref (this);
        return ret;
}

/* Translate an incoming seek event */
GstEvent*                       gdv_clipgate_translate_seek_event (GdvClipGate *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_CLIPGATE (this), NULL);
        g_return_val_if_fail (event != NULL, NULL);
        
        gdouble rate;
        GstSeekFlags flags;
        GstFormat format;
        GstSeekType curtype, stoptype;
        gint64 cur, stop;
        GstEvent *outevent = NULL;
        
        gst_event_parse_seek (event, &rate, &format, &flags,
                              &curtype, &cur, &stoptype, &stop);
        
        // Check if we operate on GST_FORMAT_TIME (our only possibility)
        if (format != GST_FORMAT_TIME) {
                GDV_WARNING_ELEMENT ("Can't translate an event in a non-time format", NULL);
                return NULL;
        }
        
        // Check start type
        if (curtype != GST_SEEK_TYPE_SET) { 
                GDV_WARNING_ELEMENT ("Can't translate an event with a non-set start type", NULL);
                return NULL;
        }
        
        // Check stop type
        if (stoptype != GST_SEEK_TYPE_SET) {
                GDV_WARNING_ELEMENT ("Can't translate an event with a non-set stop type", NULL);
                return NULL;
        }
        
        GDV_DEBUG_EXT ("Translating %s-%s @ %.2f seek event",
                       (cur != -1) ? gdv_time_to_string ((guint64) cur) : "NULL",
                       (stop != -1) ? gdv_time_to_string ((guint64) stop) : "NULL",
                       rate);
        
        // Make a writable copy
        outevent = GST_EVENT (gst_mini_object_make_writable (GST_MINI_OBJECT (event)));
        g_assert (outevent != NULL);
        g_assert (gst_mini_object_is_writable (GST_MINI_OBJECT (outevent)) == TRUE);
        GstStructure *struc = (GstStructure *) gst_event_get_structure (outevent);
        g_assert (struc != NULL);

        if (cur != -1) {
                cur = cur - gdv_time_get_int64 (this->Private->TimelineIn) +
                        gdv_time_get_int64 (this->Private->SourceIn);
                if (cur < 0) {
                        GDV_WARNING_ELEMENT ("Resulting cur is negative!", NULL);
                        return NULL;
                }
        }
        
        if (stop != -1) {
                stop = stop - gdv_time_get_int64 (this->Private->TimelineIn) +
                        gdv_time_get_int64 (this->Private->SourceIn);

                if (stop < 0) {
                        GDV_WARNING_ELEMENT ("Resulting stop is negative!", NULL);
                        return NULL;
                }

                if (stop < cur) {
                        GDV_WARNING_ELEMENT ("Resulting start is < stop ", NULL);
                        return NULL;
                }
        }

        // Success
        GDV_DEBUG_EXT ("Translating the seek event to %s-%s",
                       (cur != -1) ? gdv_time_to_string ((guint64) cur) : "NULL",
                       (stop != -1) ? gdv_time_to_string ((guint64) stop) : "NULL");
        
        gst_structure_set (struc, "cur", G_TYPE_INT64,  cur, NULL);
        gst_structure_set (struc, "stop", G_TYPE_INT64, stop, NULL);
        
        return outevent;
}

GstEvent*                       gdv_clipgate_translate_new_segment (GdvClipGate *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_CLIPGATE (this), NULL);
        g_return_val_if_fail (event != NULL, NULL);
        
        gboolean update;
        gdouble rate;
        GstFormat format;
        gint64 start, stop, streamin;
        
        gst_event_parse_new_segment (event, &update, &rate, &format, &start, &stop, &streamin);
        
        // Check if we operate on GST_FORMAT_TIME (our only possibility)
        if (format != GST_FORMAT_TIME) {
                GDV_WARNING_ELEMENT ("Can't translate an new-segment event in a non-time format", NULL);
                return NULL;
        }

        GDV_DEBUG_ELEMENT ("Converting new-segment %s-%s IN: %s RATE: %.2f",
                           (start != -1) ? gdv_time_to_string ((guint64) start) : "NULL",
                           (stop != -1) ? gdv_time_to_string ((guint64) stop) : "NULL",
                           (streamin != -1) ? gdv_time_to_string ((guint64) streamin) : "NULL",
                           rate);
        
        // Translate
        streamin = gdv_time_get_int64 (this->Private->TimelineIn);
        if (start != -1) {
                start = start - gdv_time_get_int64 (this->Private->SourceIn) +
                        gdv_time_get_int64 (this->Private->TimelineIn);
                if (start < 0) {
                        GDV_WARNING_ELEMENT ("Resulting start is negative!", NULL);
                        return NULL;
                }
        }
                
                
        if (stop != -1) {
                stop = stop - gdv_time_get_int64 (this->Private->SourceIn) +
                        gdv_time_get_int64 (this->Private->TimelineIn);

                if (stop < 0) {
                        GDV_WARNING_ELEMENT ("Resulting stop is negative!", NULL);
                        return NULL;
                }

                if (stop < start) {
                        GDV_WARNING_ELEMENT ("Resulting start is < stop ", NULL);
                        return NULL;
                }
        }
        
        GDV_DEBUG_ELEMENT ("Converted new-segment to %s-%s IN: %s",
                           (start != -1) ? gdv_time_to_string ((guint64) start) : "NULL",
                           (stop != -1) ? gdv_time_to_string ((guint64) stop) : "NULL",
                           (streamin != -1) ? gdv_time_to_string ((guint64) streamin) : "NULL");
        
        GstEvent *transevent = gst_event_new_new_segment (update, rate, format,
                                                          start, stop, streamin);

        gst_event_unref (event);
        return transevent;
}

void                            gdv_clipgate_set_stream (GdvClipGate *this, GdvTime sourcein, GdvTime timelinein)
{
        g_return_if_fail (GDV_IS_CLIPGATE (this));

        GDV_DEBUG_ELEMENT ("Setting stream params to SourceIn: %s TimelineIn: %s",
                           gdv_time_to_string (sourcein), gdv_time_to_string (timelinein));

        GATE_LOCK (this);
        this->Private->SourceIn = sourcein;
        this->Private->TimelineIn = timelinein;
        GATE_UNLOCK (this);
}

static GstStateChangeReturn     change_state (GdvClipGate *this, GstStateChange transition)
{
        g_return_val_if_fail (GDV_IS_CLIPGATE (this), GST_STATE_CHANGE_FAILURE);
        
        GstStateChangeReturn ret;
        
        switch (transition) {
                
                case GST_STATE_CHANGE_PAUSED_TO_READY: {
                        this->Private->Killer = TRUE;
                } break;

                default: 
                break;
        }

        ret = GST_ELEMENT_CLASS (ParentClass)->change_state (GST_ELEMENT (this), 
                                                             transition);

        return ret;
}

