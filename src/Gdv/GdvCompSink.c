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

#include "GdvCompSink.h"
#include "GdvCompSinkPrivate.h"

static GstElementClass*         ParentClass = NULL;

static gboolean                 private_init (GdvCompSink *this, GdvCompSinkPrivate *priv)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this) && priv != NULL, FALSE);

        priv->PadCookie = 0;
        priv->PadCount = 0;
        priv->MasterCaps = NULL;

        // Src pad
        priv->SrcPad = gst_pad_new ("src", GST_PAD_SRC);
        g_return_val_if_fail (priv->SrcPad != NULL, FALSE);
        
        gst_pad_set_getcaps_function (priv->SrcPad, (gpointer) srcpad_get_caps_function);
        gst_pad_set_event_function (priv->SrcPad, (GstPadEventFunction) srcpad_event_func);
        gst_element_add_pad (GST_ELEMENT (this), priv->SrcPad);

        // Condition
        priv->Cond = g_cond_new ();
        g_return_val_if_fail (priv->Cond != NULL, FALSE);

        // Mutex
        priv->InputMutex = g_mutex_new ();
        g_return_val_if_fail (priv->InputMutex != NULL, FALSE);
        
        // List
        priv->Inputs = NULL;

        // Other
        priv->Now = GDV_INFINITY; // Adapt to first received
        priv->Started = FALSE;
        priv->NeedsSegment = TRUE;

        return TRUE;
}

/* Dispose private */
static void                     private_dispose (GdvCompSink *this, GdvCompSinkPrivate *priv)
{
        if (priv->MasterCaps != NULL) {
                gst_caps_unref (priv->MasterCaps);
                priv->MasterCaps = NULL;
        }

        if (priv->Inputs != NULL) {
                g_slist_free (priv->Inputs);
                priv->Inputs = NULL;
        }

        if (priv->Cond != NULL) {
                g_cond_free (priv->Cond);
                priv->Cond = NULL;
        }

        if (priv->InputMutex != NULL) {
                g_mutex_free (priv->InputMutex);
                priv->InputMutex = NULL;
        }
}

/* GObject type */
GType                           gdv_compsink_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvCompSinkClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_compsink_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvCompSink),
                        2,
                        (GInstanceInitFunc) gdv_compsink_init,
                };
                objecttype = g_type_register_static (GST_TYPE_ELEMENT, "GdvCompSink",
                                                     &objectinfo, G_TYPE_FLAG_ABSTRACT);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_compsink_class_init (GdvCompSinkClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) (klass);
        
        ParentClass = g_type_class_ref (GST_TYPE_ELEMENT);
        
        elementclass->request_new_pad = (gpointer) request_new_pad;
        elementclass->change_state = (gpointer) change_state;

        gobjectclass->dispose = (gpointer) gdv_compsink_dispose;
        gobjectclass->finalize = (gpointer) gdv_compsink_finalize;

        // Reset 
        klass->FlushFunc = stock_flush_func;
        klass->ComposeFunc = NULL;
        klass->NewPadFunc = NULL;
        klass->ChainEnterFunc = NULL;
        klass->ChainLeaveFunc = NULL;
}

/* GObject init */
static void                     gdv_compsink_init (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        
        GDV_DEBUG_ELEMENT ("Initializing new GdvCompSink", NULL);
        
        this->Private = NULL;
        
        GdvCompSinkPrivate *priv = g_new0 (GdvCompSinkPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Called when someone requests new pads */
GstPad*                         request_new_pad (GdvCompSink *this, GstPadTemplate *template, 
                                                 const gchar *basename)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), NULL);
        g_return_val_if_fail (template != NULL, NULL);
        g_return_val_if_fail (template->direction == GST_PAD_SINK, NULL);
        
        GDV_DEBUG_ELEMENT ("Got a new pad request", NULL);

        gchar *name = NULL;                // Copied name
        GstPad *newpad = NULL;             // New pad
        GdvCompSinkInput *newinput = NULL; // The new input struct
        
        name = g_strdup_printf (basename, this->Private->PadCookie);
        newpad = gst_pad_new_from_template (template, name);
        if (newpad == NULL)
                goto MrFailure;
        
        if (gst_element_add_pad ((GstElement *) this, newpad) == FALSE)
                goto MrFailure;

        this->Private->PadCookie++;
        this->Private->PadCount++;
        GDV_DEBUG_ELEMENT ("Created a new requested pad '%s'", name);

        // Allocate channel
        newinput = g_new (GdvCompSinkInput, 1);
        if (newinput == NULL)
                goto MrFailure;
        
        // Fill initial
        newinput->Pad = newpad;
        newinput->Buffer = NULL;
        newinput->Flushing = FALSE;
        newinput->FlushCookie = -1;
        newinput->Private = NULL;
        newinput->LastCaps = NULL;
        
        // Add this to the list. We need to hold the lock
        INPUT_LOCK (this);
        this->Private->Inputs = g_slist_append (this->Private->Inputs, newinput);

        // Set the functions on the pad
        gst_pad_set_chain_function (newpad, (GstPadChainFunction) sinkpad_chain_func);
        gst_pad_set_event_function (newpad, (GstPadEventFunction) sinkpad_event_func);
        gst_pad_set_getcaps_function (newpad, (GstPadGetCapsFunction) sinkpad_get_caps_function);
        
        // Set this input struct as the pad private
        gst_pad_set_element_private (newpad, newinput);

        // Call our custom func
        if (call_new_pad_func (this, newinput) == FALSE)
                goto MrFailure;

        // Unlock
        INPUT_UNLOCK (this);

        return newpad;

        //// DEAD ////
        
        MrFailure:
        GDV_WARNING_ELEMENT ("Failed to add new pad/input", NULL);
        
        if (newinput != NULL) {
                g_free (newinput);
        }
        
        if (name != NULL)
                g_free (name);
        
        if (newpad != NULL)
                gst_object_unref (newpad);

        INPUT_UNLOCK (this);
        
        return NULL;
}

/* GstElement state change */
static GstStateChangeReturn     change_state (GdvCompSink *this, GstStateChange transition)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), GST_STATE_CHANGE_FAILURE);
        GDV_DEBUG_ELEMENT ("Changing state", NULL);
        
        GstStateChangeReturn ret;
        
        switch (transition) {

                case GST_STATE_CHANGE_READY_TO_PAUSED: {
                        start_pads (this);
                        INPUT_LOCK (this);
                        this->Private->Now = GDV_INFINITY;
                        this->Private->NeedsSegment = TRUE;
                        INPUT_UNLOCK (this);
                } break;

                case GST_STATE_CHANGE_PAUSED_TO_READY: {
                        stop_pads (this);
                        this->Private->NeedsSegment = TRUE;
                        // FIXME: Why no mutexing here? Check
                } break;
                        
                default: 
                break;
        }
        
        ret = GST_ELEMENT_CLASS (ParentClass)->change_state (GST_ELEMENT (this), 
                                                             transition);
        return ret;
}

/* Set the pads as non flushing, start collecting */
static void                     start_pads (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        g_return_if_fail (this->Private->Started == FALSE);

        GDV_DEBUG_ELEMENT ("Starting pads", NULL);
        INPUT_LOCK (this);

        // Iterate through all inputs
        GSList *iterator;
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);

                GST_OBJECT_LOCK (input->Pad);
                GST_PAD_UNSET_FLUSHING (input->Pad);
                GST_OBJECT_UNLOCK (input->Pad);
        }

        // Start us actually
        this->Private->Started = TRUE;
        INPUT_UNLOCK (this);
}

static void                     stop_pads (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        g_return_if_fail (this->Private->Started == TRUE);

        GDV_DEBUG_ELEMENT ("Stopping pads", NULL);
        INPUT_LOCK (this);

        // Iterate through all inputs
        GSList *iterator;
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);

                GST_OBJECT_LOCK (input->Pad);
                GST_PAD_SET_FLUSHING (input->Pad);
                GST_OBJECT_UNLOCK (input->Pad);
        }

        // Stop us actually
        this->Private->Started = FALSE;
        g_cond_broadcast (this->Private->Cond);
        INPUT_UNLOCK (this);
}

/* A Getcaps function for the src pad */
static GstCaps*                 srcpad_get_caps_function (GstPad *pad)
{
        g_return_val_if_fail (pad != NULL, NULL);
                
        GdvCompSink *this = GDV_COMPSINK (gst_pad_get_parent (pad));
        g_return_val_if_fail (this->Private->MasterCaps != NULL, NULL);

        GDV_DEBUG_ELEMENT ("Getting caps for 'src' pad, returning master caps", NULL);
        gst_caps_ref (this->Private->MasterCaps);
        gst_object_unref (this);
        
        return this->Private->MasterCaps;
}

/* A Getcaps function for the sink pad */
static GstCaps*                 sinkpad_get_caps_function (GstPad *pad)
{
        g_return_val_if_fail (pad != NULL, NULL);
        
        GdvCompSink *this = GDV_COMPSINK (gst_pad_get_parent (pad));
        g_return_val_if_fail (GDV_IS_COMPSINK (this), NULL);

        GDV_DEBUG_ELEMENT ("Getting caps for sink '%s' pad", GST_PAD_NAME (pad));
        
        gst_caps_ref (this->Private->MasterCaps);
        gst_object_unref (this);
        return this->Private->MasterCaps;
}

/* Chain function. Individual input threads enter here */
static GstFlowReturn            sinkpad_chain_func (GstPad *pad, GstBuffer *buffer)
{
        g_return_val_if_fail (pad != NULL, GST_FLOW_ERROR);
        g_return_val_if_fail (buffer != NULL, GST_FLOW_ERROR);

        /*
        if (GST_BUFFER_CAPS (buffer) == NULL) {
                // FIXME: This is a temporary fix. Broken plugins? (some send buffers with null caps)
                GDV_WARNING_EXT ("Null caps, dropping this buffer", NULL);
                return GST_FLOW_OK;
        }*/
        
        GdvCompSink *this = GDV_COMPSINK (gst_pad_get_parent (pad));
        g_return_val_if_fail (GDV_IS_COMPSINK (this), GST_FLOW_ERROR);
                
        GDV_DEBUG_ELEMENT ("Chain function for '%s' sink pad", GST_PAD_NAME (pad));

        GstFlowReturn result = GST_FLOW_OK; // Our return value
        GstBuffer *outbuffer = NULL;        // Output buffer we'll push
        GdvCompSinkInput *input = NULL;     // Input of this pad

        // Lock us
        GDV_DEBUG_ELEMENT ("'%s' trying to lock", GST_PAD_NAME (pad));
        INPUT_LOCK (this);
        GDV_DEBUG_ELEMENT ("'%s' Locked", GST_PAD_NAME (pad));

        // Get our input struct as the pad's private
        input = gst_pad_get_element_private (pad);
        if (input == NULL) {
                GDV_WARNING ("Foreign pad?!", NULL);
                goto MrFailure;
        }

        // Check for null capsing on the buffer 
        /*if (GST_BUFFER_CAPS (buffer) != NULL) {
                if (input->LastCaps != NULL)
                        gst_caps_unref (input->LastCaps);
                input->LastCaps = GST_BUFFER_CAPS (buffer);
                gst_caps_ref (input->LastCaps);
        } else {
                if (input->LastCaps == NULL) {
                        GDV_WARNING_ELEMENT ("No caps on buffer and no last caps seen!", NULL);
                        goto MrFree;
                } else {
                        GST_BUFFER_CAPS (buffer) = input->LastCaps;
                        gst_caps_ref (input->LastCaps);
                }
        }*/
        
        while (block_on_flush_cookie (this)) {

                if (! this->Private->Started)
                        goto NotStarted;
                
                if (input->Flushing)
                        goto Flushing;
                
                GDV_DEBUG_ELEMENT ("Pad %s waiting for flush cookie", GST_PAD_NAME (pad));
                g_cond_wait (this->Private->Cond, this->Private->InputMutex);
        }

        GDV_DEBUG_ELEMENT ("Pad %s passing flush cookie cond", GST_PAD_NAME (pad));
        
        // Check if we're actually started
        if (! this->Private->Started)
                goto NotStarted;

        // Check if this pad is flushing
        if (input->Flushing)
                goto Flushing;

        GDV_DEBUG_ELEMENT ("Pad '%s' entering the chain body", GST_PAD_NAME (pad));

        // Put the buffer in the input channel stack. Check if free
        if (input->Buffer != NULL) {
                GDV_DEBUG_ELEMENT ("Whooops... pushing buffer over non-empty input. "
                                   "When did we lost sync?", NULL);
                g_assert_not_reached ();
        }

        input->Buffer = buffer;
        if (call_chain_enter_func (this, input, buffer) == FALSE) {
                GDV_DEBUG_ELEMENT ("Chain helper func failed", NULL);
                goto MrFailure;
        }

        // Check now
        if (this->Private->Now == GDV_INFINITY)
                this->Private->Now = GST_BUFFER_TIMESTAMP (input->Buffer);

        // Check if we have buffers on all inputs
        if (all_stacked (this) == TRUE) {
                // We've got buffers on ALL input channels. We can do some processing here!
                GDV_DEBUG_ELEMENT ("All inputs stacked!", NULL);
                outbuffer = call_compose_func (this);

                // We allow the compose func to return NULL when the buffer has
                // been cached or something like that
                if (outbuffer == NULL) {
                        GDV_DEBUG_ELEMENT ("Null from compose func, skipping", NULL);
                        goto comp_done;
                }
                
                // Increase our timestamp counter
                this->Private->Now += GST_BUFFER_DURATION (outbuffer);

                // Let's check if we need the new segment
                if (this->Private->NeedsSegment == TRUE) 
                        if (push_new_segment (this, this->Private->SrcPad, outbuffer) == FALSE)
                                GDV_WARNING_ELEMENT ("Failed to push new segment", NULL);
                
                GDV_DEBUG_ELEMENT ("Pushing %s [%s]",
                                   gdv_time_to_string (GST_BUFFER_TIMESTAMP (outbuffer)),
                                   gdv_time_to_string (GST_BUFFER_DURATION (outbuffer)));
                
                result = gst_pad_push (this->Private->SrcPad, outbuffer);

                comp_done:
                call_flush_func (this);
                g_cond_broadcast (this->Private->Cond);
        }

        // Sleep
        while (input->Buffer != NULL) {
                GDV_DEBUG_ELEMENT ("Pad '%s' sleeping on cond", GST_PAD_NAME (pad));
                g_cond_wait (this->Private->Cond, this->Private->InputMutex);
                GDV_DEBUG_ELEMENT ("Pad '%s' waking up", GST_PAD_NAME (pad));
        
                // Let's check the status once again...
                if (! this->Private->Started)
                        goto NotStarted;
                
                if (input->Flushing)
                        goto Flushing;
        }
        
        goto MrFree;
                
        //// DEAD ////

 NotStarted:
        GDV_DEBUG_ELEMENT ("Pads collecting not started", NULL);
        result = GST_FLOW_WRONG_STATE;
        goto MrFree;
        
 MrFailure:
        GDV_WARNING_ELEMENT ("Error during sink-pad chain processing", NULL);
        result = GST_FLOW_ERROR;
        goto MrFree;

 Flushing:
        GDV_DEBUG_ELEMENT ("Pad '%s' is flushing", GST_PAD_NAME (pad));
        result = GST_FLOW_WRONG_STATE;
        goto MrFree;
        
 MrFree:
        GDV_DEBUG_ELEMENT ("PAd '%s' leaving func", GST_PAD_NAME (pad));
        
        if (input->Buffer != NULL) {
                gst_buffer_unref (input->Buffer);
                input->Buffer = NULL;
        }

        if (call_chain_leave_func (this, input, buffer) == FALSE)
                GDV_DEBUG_ELEMENT ("Chain helper func failed", NULL);

        INPUT_UNLOCK (this);
        
        return result;
}

/* Check if all inputs have buffers stacked */
static gboolean                 all_stacked (GdvCompSink *this)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);
        
        GDV_DEBUG_ELEMENT ("Checking if all buffers are stacked", NULL);

        GSList *iterator;
        // Iterate through all inputs
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);

                if (input->Buffer == NULL)
                        return FALSE;
        }

        return TRUE;
}

/* A default implementation of the flushing function. Resets (flushes) all buffers */
static void                     stock_flush_func (GdvCompSink *this, GSList *inputs)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        
        GSList *iterator;

        // Iterate through all inputs
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);
                
                if (input->Buffer != NULL) {
                        gst_buffer_unref (input->Buffer);
                        input->Buffer = NULL;
                }
        }
}

static gboolean                 srcpad_event_func (GstPad *pad, GstEvent *event)
{
        GdvCompSink *this = GDV_COMPSINK (GST_PAD_PARENT (pad));
        g_return_val_if_fail (this != NULL, FALSE);
        
        switch (GST_EVENT_TYPE (event)) {

                case GST_EVENT_SEEK: {
                       
                        GDV_DEBUG_ELEMENT ("Got seek event, forwarding to all input pads", NULL);
                        GSList *iterator;

                        // Iterate through all inputs, copy the event
                        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                                g_assert (input != NULL);
                               
                                gst_pad_send_event (GST_PAD_PEER (input->Pad), gst_event_copy (event));
                                // FIXME: What about event unreffing?
                        }
                        return TRUE;
                } break;
                
                default:
                        return gst_pad_event_default (pad, event);
                break;
        }
}

/* Pad event function */
static gboolean                 sinkpad_event_func (GstPad *pad, GstEvent *event)
{
        g_return_val_if_fail (pad != NULL, FALSE);
        g_return_val_if_fail (event != NULL, FALSE);
        
        GdvCompSink *this = GDV_COMPSINK (GST_PAD_PARENT (pad));
        g_return_val_if_fail (this != NULL, FALSE);

        // Get our input struct as the pad's private
        GdvCompSinkInput *input = gst_pad_get_element_private (pad);
        g_return_val_if_fail (input != NULL, FALSE);
        
        GDV_DEBUG_ELEMENT ("Got event %s on pad %s", GST_EVENT_TYPE_NAME (event), 
                           GST_PAD_NAME (pad));
        
        switch (GST_EVENT_TYPE (event)) {
                
                case GST_EVENT_FLUSH_START:
                        
                        gst_pad_push_event (this->Private->SrcPad, event);
                                                
                        INPUT_LOCK (this);

                        if (input->FlushCookie == -1)
                                input->FlushCookie = 0;
                        
                        input->FlushCookie++;
                        input->Flushing = TRUE;
                        this->Private->NeedsSegment = TRUE;

                        g_cond_broadcast (this->Private->Cond);
                        INPUT_UNLOCK (this);
                                                
                        return TRUE;
                        break;
                        
                case GST_EVENT_FLUSH_STOP:

                        INPUT_LOCK (this);

                        this->Private->Now = GDV_INFINITY;
                        input->Flushing = FALSE;
                        input->FlushCookie--;
                        g_assert (input->FlushCookie >= 0);
                        
                        reset_flush_cookies (this);
                        
                        g_cond_broadcast (this->Private->Cond);
                        INPUT_UNLOCK (this);
                        
                        gst_pad_push_event (this->Private->SrcPad, event);
                        return TRUE;
                break;

                
                case GST_EVENT_NEWSEGMENT:
                        // Just kill it
                        return TRUE;
                        break;

                default:
                        return FALSE;
                        break;
        }
                
        return gst_pad_event_default (pad, event);
}

/* Push a new segment event to the given pad according to the current buffer
 * (which is a first buffer of a new segment */
static gboolean                 push_new_segment (GdvCompSink *this, GstPad *pad, GstBuffer *buffer)
{
        g_return_val_if_fail (pad != NULL, FALSE);
        g_return_val_if_fail (buffer != NULL, FALSE);
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);

        GDV_DEBUG_ELEMENT ("Pushing new segment from %s to infinity", gdv_time_to_string (GST_BUFFER_TIMESTAMP (buffer)));
       

        GstEvent *event = gst_event_new_new_segment (FALSE, 1.0, GST_FORMAT_TIME, GST_BUFFER_TIMESTAMP (buffer),
                                                     -1, 0);

        if (gst_pad_push_event (pad, event)) {
                this->Private->NeedsSegment = FALSE;
                return TRUE;
        } else 
                return FALSE;
}

static gboolean                 block_on_flush_cookie (GdvCompSink *this)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);
        
        GSList *iterator;
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);

                if (input->FlushCookie != -1)
                        return TRUE;
        }
        
        return FALSE;
}

static void                     reset_flush_cookies (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        
        GSList *iterator;

        // Bail out if at least one cookie != 0
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);
                
                if (input->FlushCookie != 0)
                        return;
        }

        // Reset all cookies
        for (iterator = this->Private->Inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);
                
                input->FlushCookie = -1;
        }
}

/* Set the private data associated with the input */
void                            gdv_compsink_input_set_private (GdvCompSinkInput *input, gpointer private)
{
        input->Private = private;
}

/* Get the private data associated with the input */
gpointer                        gdv_compsink_input_get_private (GdvCompSinkInput *input)
{
        return input->Private;
}

/* Flush the inputs */
static void                     call_flush_func (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));

        GDV_DEBUG_ELEMENT ("Calling flush function", NULL);
        
        GdvCompSinkClass *klass = (GdvCompSinkClass *) G_OBJECT_GET_CLASS (this);
        g_assert (klass != NULL);
        g_return_if_fail (klass->FlushFunc != NULL);
        
        klass->FlushFunc (this, this->Private->Inputs);
}

/* Compose the output buffer from inputs */
static GstBuffer*               call_compose_func (GdvCompSink *this)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), NULL);

        GDV_DEBUG_ELEMENT ("Calling compose function", NULL);
        
        GdvCompSinkClass *klass = (GdvCompSinkClass *) G_OBJECT_GET_CLASS (this);
        g_assert (klass != NULL);
        g_return_val_if_fail (klass->ComposeFunc != NULL, NULL);
        
        return klass->ComposeFunc (this, this->Private->Inputs, this->Private->Now);
}

static gboolean                 call_new_pad_func (GdvCompSink *this, GdvCompSinkInput *input)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);

        GdvCompSinkClass *klass = (GdvCompSinkClass *) G_OBJECT_GET_CLASS (this);
        g_assert (klass != NULL);

        if (klass->NewPadFunc == NULL)
                return TRUE;
        else {
                GDV_DEBUG_ELEMENT ("Calling new pad func", NULL);
                return klass->NewPadFunc (this, input);
        }
}

static gboolean                 call_chain_enter_func (GdvCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);

        GdvCompSinkClass *klass = (GdvCompSinkClass *) G_OBJECT_GET_CLASS (this);
        g_assert (klass != NULL);

        if (klass->ChainEnterFunc == NULL)
                return TRUE;
        else {
                GDV_DEBUG_ELEMENT ("Calling chain enter func", NULL);
                return klass->ChainEnterFunc (this, input, buffer);
        }
}

static gboolean                 call_chain_leave_func (GdvCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);

        GdvCompSinkClass *klass = (GdvCompSinkClass *) G_OBJECT_GET_CLASS (this);
        g_assert (klass != NULL);

        if (klass->ChainLeaveFunc == NULL)
                return TRUE;
        else {
                GDV_DEBUG_ELEMENT ("Calling chain leave func", NULL);
                return klass->ChainLeaveFunc (this, input, buffer);
        }
}

/* GObject dispose */
static void                     gdv_compsink_dispose (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        // Check
        if (this->Private != NULL &&
            this->Private->Started == TRUE) {
                GDV_WARNING_ELEMENT ("Trying to dispose a started compsink, aborting", NULL);
                return;
        }

        // Dispose
        if (this->Private != NULL) 
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_compsink_finalize (GdvCompSink *this)
{
        g_return_if_fail (GDV_IS_COMPSINK (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);

        // Check
        if (this->Private != NULL &&
            this->Private->Started == TRUE) {
                GDV_WARNING_ELEMENT ("Trying to finalize a started compsink, aborting", NULL);
                return;
        }

        // Free
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

gboolean                        gdv_compsink_set_master_caps (GdvCompSink *this, GstCaps *caps)
{
        g_return_val_if_fail (GDV_IS_COMPSINK (this), FALSE);
        g_return_val_if_fail (caps != NULL, FALSE);

        INPUT_LOCK (this);

        if (this->Private->Started == TRUE) {
                GDV_WARNING_ELEMENT ("Trying to set master caps on started element! Aborting", NULL);
                goto MrFailure;
        }

        // Caps
        GDV_DEBUG_ELEMENT ("Setting the master caps %s", gst_caps_to_string (caps));

        gst_caps_ref (caps);

        if (this->Private->MasterCaps != NULL)
                gst_caps_unref (this->Private->MasterCaps);
        
        this->Private->MasterCaps = caps;

        INPUT_UNLOCK (this);
        return TRUE;

        //// DEAD ////

 MrFailure:
        INPUT_UNLOCK (this);
        return FALSE;
}


