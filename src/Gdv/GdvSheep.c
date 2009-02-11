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

#include "GdvSheep.h"
#include "GdvSheepPrivate.h"

static GstElementClass*         ParentClass = NULL;

/* GObject type */
GType                           gdv_sheep_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvSheepClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) gdv_sheep_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvSheep), 
                        4, 
                        (GInstanceInitFunc) gdv_sheep_init,
                };
                objecttype = g_type_register_static (GST_TYPE_ELEMENT, "GdvSheep", 
                                                     &objectinfo, G_TYPE_FLAG_ABSTRACT);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_sheep_class_init (GdvSheepClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) (klass);
        
        // Ref stuff 
        ParentClass = g_type_class_ref (GST_TYPE_ELEMENT);
        
        gobjectclass->dispose = (gpointer) gdv_sheep_dispose;
        gobjectclass->finalize = (gpointer) gdv_sheep_finalize;
        
        elementclass->send_event = (gpointer) send_event;
        
        klass->get_caps = NULL;
        klass->buffer_func = NULL;
}

/* GObject dispose */
static void                     gdv_sheep_dispose (GdvSheep *this)
{
        g_return_if_fail (GDV_IS_SHEEP (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_sheep_finalize (GdvSheep *this)
{
        g_return_if_fail (GDV_IS_SHEEP (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject init */
static void                     gdv_sheep_init (GdvSheep *this, gpointer klass)
{
        g_return_if_fail (GDV_IS_SHEEP (this));
        g_return_if_fail (klass != NULL);
        this->Private = NULL;

        GdvSheepPrivate *priv = g_new0 (GdvSheepPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv, klass) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Private init */
static gboolean                 private_init (GdvSheep *this, GdvSheepPrivate *private, gpointer klass)
{
        g_return_val_if_fail (GDV_IS_SHEEP (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);
        
        private->Segment = gdv_timespan_new (GDV_ZEROTIME, GDV_INFINITY_SEEK);
        private->CurrentTime = GDV_ZEROTIME;
        private->CachedSeek = NULL;
        private->Running = FALSE;

        GDV_DEBUG_ELEMENT ("Creating a pad", NULL);
        private->SrcPad = gst_pad_new ("src", GST_PAD_SRC);
        g_return_val_if_fail (private->SrcPad != NULL, FALSE);
        
        GDV_DEBUG_ELEMENT ("Setting pad functions & adding pad", NULL);
        gst_pad_set_activatepush_function (private->SrcPad, activate_push);
        gst_pad_set_getcaps_function (private->SrcPad, pad_getcaps);
        gst_pad_set_event_function (private->SrcPad, event_func);
                
        GDV_DEBUG_ELEMENT ("Adding the pad", NULL);
        gst_element_add_pad (GST_ELEMENT (this), private->SrcPad);

        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvSheep *this, GdvSheepPrivate *private)
{
        g_return_if_fail (GDV_IS_SHEEP (this));
        g_return_if_fail (private != NULL);
        
        if (private->Segment != NULL) {
                gdv_timespan_free (private->Segment);
                private->Segment = NULL;
        }
        
        if (private->CachedSeek != NULL) {
                gst_event_unref (this->Private->CachedSeek);
                private->CachedSeek = NULL;
        }
}

static gboolean                 activate_push (GstPad *pad, gboolean active)
{
        g_return_val_if_fail (pad != NULL, FALSE);
        
        GdvSheep *this = GDV_SHEEP (GST_PAD_PARENT (pad));
        g_return_val_if_fail (GDV_IS_SHEEP (this), FALSE);
        
        // Quick action if deactivation
        if (active == FALSE) {
                GDV_DEBUG_ELEMENT ("Deactivating push mode", NULL);
                return gst_pad_stop_task (pad);
        }
                
        GDV_DEBUG_ELEMENT ("Activating in push mode", NULL);
        g_assert (active == TRUE);
        
        // Free if we have something
        if (this->Private->Segment != NULL)
                gdv_timespan_free (this->Private->Segment);
        
        // Initialize
        this->Private->Segment = gdv_timespan_new (GDV_ZEROTIME, GDV_INFINITY_SEEK);
        this->Private->CurrentTime = GDV_ZEROTIME;
        
        // Stubby fixating
        if (! caps_negotiate (this))
                goto error;
        
        // This starts the task
        if (! perform_seek (this, this->Private->CachedSeek))
                goto error;
        
        if (this->Private->CachedSeek != NULL) {
                gst_event_unref (this->Private->CachedSeek);
                this->Private->CachedSeek = NULL;
        }
        
        return TRUE;
        
        // Error
        error:
        GDV_WARNING_ELEMENT ("Failed to activate in push mode", NULL);
        return FALSE;
}

static void                     task_loop (GstPad *pad)
{
        g_return_if_fail (pad != NULL);
        GdvSheep *this = GDV_SHEEP (GST_PAD_PARENT (pad));
        if (this == NULL)
                goto error;
        
        GdvSheepClass *klass = GDV_SHEEP_GET_CLASS (this);
        g_assert (klass != NULL);

        GstBuffer *buffer = klass->buffer_func (this, pad, this->Private->CurrentTime, 
                                                this->Private->Segment->End);
        
        if (buffer == NULL)
                goto pause;

        GstClockTime duration = GST_BUFFER_DURATION (buffer);
        GstFlowReturn flow = gst_pad_push (pad, buffer);
        
        if (flow != GST_FLOW_OK)
                goto pause;
        else
                this->Private->CurrentTime += duration;
        
        if (this->Private->Segment->End != GDV_INFINITY &&
            this->Private->CurrentTime >= this->Private->Segment->End)
                goto eos;
                
        return;

        // Pause
        pause:
        pause_task (this);
        return;
        
        // Eos
        eos:
        pause_task (this);
        if (this->Private->Segment->End != GDV_INFINITY) {
                GDV_DEBUG_ELEMENT ("Posting message segment_done at %s", 
                                   gdv_time_to_string (this->Private->Segment->End));
                gst_element_post_message (GST_ELEMENT (this),
                                          gst_message_new_segment_done (GST_OBJECT (this),
                                                                        GST_FORMAT_TIME, 
                                                                        this->Private->Segment->End));
        } else {
                GDV_DEBUG_ELEMENT ("Posting message eos after %s", 
                                   gdv_time_to_string (this->Private->CurrentTime));
                gst_pad_push_event (pad, gst_event_new_eos ());
        } 
        return;
        
        // Error 
        error:
        pause_task (this);
        gst_pad_push_event (pad, gst_event_new_eos ());
        if (this != NULL) 
                GDV_WARNING_ELEMENT ("Error in the loop function", NULL);
        return;
}

static GstCaps*                 pad_getcaps (GstPad * pad)
{
        g_return_val_if_fail (pad != NULL, NULL);
        GdvSheepClass *klass;
        GdvSheep *this;
        GstCaps *caps = NULL;
        
        this = GDV_SHEEP (GST_PAD_PARENT (pad));
        g_return_val_if_fail (this != NULL, NULL);
        
        klass = GDV_SHEEP_GET_CLASS (this);
        g_assert (klass != NULL);
        
        if (klass->get_caps)
                caps = klass->get_caps (this, pad);
        
        if (caps == NULL) {
                GstPadTemplate *padtemplate = gst_element_class_get_pad_template (GST_ELEMENT_CLASS (klass), "src");
                if (padtemplate != NULL) {
                        caps = gst_caps_ref (gst_pad_template_get_caps (padtemplate));
                }
        }
        
        GDV_DEBUG_ELEMENT ("Returning pad caps: %s", gst_caps_to_string (caps));
        return caps;
}

/* Perform a seek and/or start pause a task. If no event is specified, we're not flushing
 * nor sending a new-segment event */
static gboolean                 perform_seek (GdvSheep *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_SHEEP (this), FALSE);
        
        GstSeekFlags flags;
        gdouble rate;
        GstFormat format;
        GstSeekType curtype, stoptype;
        gint64 cur, stop;
        GdvTimeSpan *newspan = NULL;
                        
        GDV_DEBUG_ELEMENT ("Performing seek", NULL);
        
        if (event) {
                gst_event_parse_seek (event, &rate, &format, &flags,
                                             &curtype, &cur, &stoptype, &stop);
                
                g_return_val_if_fail (rate == 1.0, FALSE);
                g_return_val_if_fail (format == GST_FORMAT_TIME, FALSE);
                g_return_val_if_fail (curtype == GST_SEEK_TYPE_SET, FALSE);
                g_return_val_if_fail (stoptype == GST_SEEK_TYPE_SET, FALSE);
                //g_return_val_if_fail (cur >= 0, FALSE);
                if (cur < 0) {
                        g_warning ("CUR is %lld", cur);
                        return FALSE;
                }

                newspan = gdv_timespan_new (cur, (stop != -1) ? stop : GDV_INFINITY);
        } else 
                flags = 0;
        
        if (flags & GST_SEEK_FLAG_FLUSH) {
                GDV_DEBUG_ELEMENT ("Pushing flush start", NULL);
                gst_pad_push_event (GDV_SHEEP_PAD (this), gst_event_new_flush_start ());
        } else {
                GDV_DEBUG_ELEMENT ("Pausing task", NULL);
                pause_task (this);
        }

        GST_PAD_STREAM_LOCK (GDV_SHEEP_PAD (this));
        
        // Update our current span with the new one
        if (event) {
                if (this->Private->Segment != NULL) 
                        gdv_timespan_free (this->Private->Segment);
                this->Private->Segment = newspan;
                this->Private->CurrentTime = newspan->Start;
        
                // Stop flushing
                GDV_DEBUG_ELEMENT ("Pushing flush stop", NULL);
                if (flags & GST_SEEK_FLAG_FLUSH) 
                        gst_pad_push_event (GDV_SHEEP_PAD (this), gst_event_new_flush_stop ());
        }
        
        // Push new segment
        GDV_DEBUG_ELEMENT ("Pushing new-segment event", NULL);
        gst_pad_push_event (GDV_SHEEP_PAD (this),
                            gst_event_new_new_segment (FALSE,
                                                       1.0,
                                                       GST_FORMAT_TIME,
                                                       this->Private->Segment->Start,
                                                       (this->Private->Segment->End != GDV_INFINITY) ? 
                                                       this->Private->Segment->End : -1,
                                                       this->Private->Segment->Start));

        GDV_DEBUG_ELEMENT ("Starting new pad task", NULL);
        start_task (this, task_loop);
        
        GST_PAD_STREAM_UNLOCK (GDV_SHEEP_PAD (this));
        
        return TRUE;
}

/* A default negotation function */
static gboolean                 caps_negotiate (GdvSheep *this)
{
        GstCaps *thiscaps;
        // GstCaps *caps = NULL;
        // GstCaps *peercaps = NULL;
        // gboolean result = FALSE;
        
        // first see what is possible on our source pad
        thiscaps = gst_pad_get_caps (GDV_SHEEP_PAD (this));
        if (thiscaps == NULL || gst_caps_is_any (thiscaps))
                goto error;
        
        gst_pad_set_caps (GDV_SHEEP_PAD (this), thiscaps);
        return TRUE;
        
        // Error
        error:
        GDV_WARNING_ELEMENT ("Failed to fixate/negotiate caps", NULL);
        return FALSE;
}

static gboolean                 event_func (GstPad *pad, GstEvent *event)
{
        g_return_val_if_fail (pad != NULL, FALSE);
        g_return_val_if_fail (event != NULL, FALSE);
                
        gboolean result;
        GdvSheep *this = GDV_SHEEP (GST_PAD_PARENT (pad));
        g_return_val_if_fail (this != NULL, FALSE);
        
        switch (GST_EVENT_TYPE (event)) {
                case GST_EVENT_SEEK:
                result = perform_seek (this, event);
                break;
                
                case GST_EVENT_FLUSH_START:
                        break;
                        
                case GST_EVENT_FLUSH_STOP:
                default:
                result = TRUE;
                break;
        }
        
        gst_event_unref (event);
        return result;
        
}

/* Events that are sent to the element directly, ie. a Seek in READY */
static gboolean                 send_event (GdvSheep *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_SHEEP (this), FALSE);
        GDV_DEBUG_ELEMENT ("Got event via element send", NULL);
        gboolean result;
        
        switch (GST_EVENT_TYPE (event)) {
                
                case GST_EVENT_SEEK: {

                        GstState state;
                        gst_element_get_state (GST_ELEMENT (this), &state, NULL, GST_CLOCK_TIME_NONE);
                        
                        if (this->Private->Running || state > GST_STATE_READY)
                                perform_seek (this, event);
                        else {
                                GST_OBJECT_LOCK (this);
                                // FIXME: Why this doesn't work?
                                //if (this->Private->CachedSeek != NULL)
                                //gst_event_unref (this->Private->CachedSeek);
                                
                                gst_event_ref (event);
                
                                this->Private->CachedSeek = event;
                                GST_OBJECT_UNLOCK (this);
                        }
                        
                        result = TRUE;
                } break;
                
                default:
                result = FALSE;
                break;
        }
        
        gst_event_unref (event);
        return result;
}

static gboolean                 pause_task (GdvSheep *this)
{
        g_return_val_if_fail (GDV_IS_SHEEP (this), FALSE);
        GDV_DEBUG_ELEMENT ("Pausing pad task", NULL);
        
        gst_pad_pause_task (GDV_SHEEP_PAD (this));
        this->Private->Running = FALSE;

        return TRUE;
}

static gboolean                 start_task (GdvSheep *this, gpointer loop)
{
        g_return_val_if_fail (GDV_IS_SHEEP (this), FALSE);
        GDV_DEBUG_ELEMENT ("Starting pad task", NULL);
        
        this->Private->Running = TRUE;
        gst_pad_start_task (GDV_SHEEP_PAD (this), (GstTaskFunction) loop, GDV_SHEEP_PAD (this));

        return TRUE;
}
        
