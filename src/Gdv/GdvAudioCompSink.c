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

#include "GdvAudioCompSink.h"
#include "GdvAudioCompSinkPrivate.h"

static GstElementClass*         ParentClass = NULL;

static GstStaticPadTemplate     SinkFactory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                       GST_PAD_SINK,
                                                                       GST_PAD_REQUEST,
                                                                       AUDIO_CAPS);

static gboolean                 private_init (GdvAudioCompSink *this, GdvAudioCompSinkPrivate *priv)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);
        g_return_val_if_fail (priv != NULL, FALSE);
        
        // Boy
        priv->AudioBoy = NULL;        
 
        // Partial buffers
        priv->PartialBuffers = NULL;
                
        return TRUE;
}

static gboolean                 private_dispose (GdvAudioCompSink *this, GdvAudioCompSinkPrivate *priv)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);
        g_return_val_if_fail (priv != NULL, FALSE);
        
        if (priv->AudioBoy != NULL) {
                g_object_unref (priv->AudioBoy);
                priv->AudioBoy = NULL;
        }
        
        free_partials (this);
                
        return TRUE;
}

/* Base init to add the template */
static void                     gdv_audiocompsink_base_init (gpointer klass)
{
        GstElementClass *elementclass = GST_ELEMENT_CLASS (klass);
        
        GDV_DEBUG_EXT ("Adding sink pad template", NULL);
        gst_element_class_add_pad_template (elementclass, 
                                            gst_static_pad_template_get (&SinkFactory));
}

/* GObject type */
GType                           gdv_audiocompsink_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvAudioCompSinkClass), 
                        gdv_audiocompsink_base_init, 
                        NULL,
                        (GClassInitFunc) gdv_audiocompsink_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvAudioCompSink),
                        2,
                        (GInstanceInitFunc) gdv_audiocompsink_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_COMPSINK, "GdvAudioCompSink",
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_audiocompsink_class_init (GdvAudioCompSinkClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GdvCompSinkClass *compsinkclass = (GdvCompSinkClass *) klass;
        
        ParentClass = g_type_class_ref (GST_TYPE_ELEMENT);
        
        gobjectclass->dispose = (gpointer) gdv_audiocompsink_dispose;
        gobjectclass->finalize = (gpointer) gdv_audiocompsink_finalize;

        compsinkclass->ComposeFunc = (gpointer) compose_func;
        compsinkclass->FlushFunc = (gpointer) flush_func;
        compsinkclass->NewPadFunc = (gpointer) new_pad_func;
        compsinkclass->ChainEnterFunc = (gpointer) chain_enter_func;
        compsinkclass->ChainLeaveFunc = (gpointer) chain_leave_func;
}

/* GObject init */
static void                     gdv_audiocompsink_init (GdvAudioCompSink *this)
{
        g_return_if_fail (GDV_IS_AUDIOCOMPSINK (this));
        
        GDV_DEBUG_ELEMENT ("Initializing new GdvAudioCompSink", NULL);
        
        this->Private = NULL;
        
        GdvAudioCompSinkPrivate *priv = g_new0 (GdvAudioCompSinkPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Link a track audio to this sink */
gboolean                        gdv_audiocompsink_link_track (GdvAudioCompSink *this, GdvTrack *track)
{
        g_return_val_if_fail (GDV_IS_TRACK (track), FALSE);
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Trying to link in track '%s'", GST_OBJECT_NAME (track));
        return gst_element_link_pads (GST_ELEMENT (track), 
                                      "asrc", GST_ELEMENT (this), NULL);
}

/* Prepare an output buffer from the current stack. Call with a lock held */
static GstBuffer*               compose_func (GdvAudioCompSink *this, GSList *inputs, GdvTime now)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), NULL);

        GDV_DEBUG_ELEMENT ("Mixing buffers using boy", NULL);

        GstBuffer *buffer = gdv_audioboy_get_buffer (this->Private->AudioBoy);
        if (buffer == NULL)
                goto MrFailure;

        // Now let's assign timestamps
        GST_BUFFER_TIMESTAMP (buffer) = now;
        
        GDV_DEBUG_ELEMENT ("Buffer timestamp: %s duration: %s",
                           gdv_time_to_string (GST_BUFFER_TIMESTAMP (buffer)),
                           gdv_time_to_string (GST_BUFFER_DURATION (buffer)));

        return handle_new_buffer_with_partials (this, buffer);
        
        //// DEAD ////

 MrFailure:
        GDV_WARNING_ELEMENT ("Failed to process stacked buffers", NULL);
        
        if (buffer != NULL)
                gst_buffer_unref (buffer);
        
        return NULL;
}

static void                     flush_func (GdvAudioCompSink *this, GSList *inputs)
{
        g_return_if_fail (GDV_IS_AUDIOCOMPSINK (this));

        GSList *iterator;
		int nullified = 0;
        
        // Iterate through all inputs
        for (iterator = inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);
                
                GdvAudioBoyChannel *channel = gdv_compsink_input_get_private (input);
                g_assert (channel != NULL);

                if (gdv_audioboy_channel_is_empty (this->Private->AudioBoy, channel)) {
                        // We need to flush it
                        gst_buffer_unref (input->Buffer);
                        input->Buffer = NULL;
                        nullified++;
                }
        }

        if (nullified == 0)
                GDV_WARNING_ELEMENT ("Not a single buffer was nullfified!", NULL);
        
        GDV_DEBUG_ELEMENT ("%d buffers nullified", nullified);
}

/* Create a new instance */
GdvAudioCompSink*               gdv_audiocompsink_new (void)
{
        GdvAudioCompSink *this = NULL;
        
        this = g_object_new (GDV_TYPE_AUDIOCOMPSINK, NULL);
        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else 
                return this;
}

gboolean                        gdv_audiocompsink_set_format (GdvAudioCompSink *this, GdvProjectFormat *format)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), FALSE);

        // Caps
        GDV_DEBUG_ELEMENT ("Setting the audio format", NULL);

        GdvAudioFormat *aformat = NULL;
        g_object_get (G_OBJECT (format), "audioformat", &aformat, NULL);
        g_return_val_if_fail (aformat != NULL, FALSE);

        GstCaps *caps;
        g_object_get (G_OBJECT (aformat), "caps", &caps, NULL);
        g_return_val_if_fail (caps != NULL, FALSE);

        // Call the parent func
        gdv_compsink_set_master_caps (GDV_COMPSINK (this), caps);
        
        GDV_DEBUG_ELEMENT ("Format caps is: %s", gst_caps_to_string (caps));
        
        GDV_DEBUG_ELEMENT ("Creating the audioboy", NULL);
        this->Private->AudioBoy = gdv_audioboy_new (caps);
        
        g_object_unref (aformat);
        gst_caps_unref (caps);
        
        return TRUE;
}

/* GObject dispose */
static void                     gdv_audiocompsink_dispose (GdvAudioCompSink *this)
{
        g_return_if_fail (GDV_IS_AUDIOCOMPSINK (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        // Dispose
        if (this->Private != NULL) 
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_audiocompsink_finalize (GdvAudioCompSink *this)
{
        g_return_if_fail (GDV_IS_AUDIOCOMPSINK (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);

        // Free
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

static gboolean                 new_pad_func (GdvAudioCompSink *this, GdvCompSinkInput *input)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);
        g_return_val_if_fail (input != NULL, FALSE);

        GDV_DEBUG_ELEMENT ("Adding a channel to new input", NULL);

        GdvAudioBoyChannel *channel = gdv_audioboy_add_channel (this->Private->AudioBoy);
        gdv_compsink_input_set_private (input, channel);

        return TRUE;
}

static gboolean                 chain_enter_func (GdvAudioCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);

        GDV_DEBUG_ELEMENT ("Adding buffer %s to audioboy", gdv_time_to_string (GST_BUFFER_TIMESTAMP (buffer)));

        GdvAudioBoyChannel *channel = gdv_compsink_input_get_private (input);
        g_assert (channel != NULL);

        gdv_audioboy_channel_add_buffer (this->Private->AudioBoy, channel, buffer);
        
        return TRUE;
}

static gboolean                 chain_leave_func (GdvAudioCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), FALSE);

        GdvAudioBoyChannel *channel = gdv_compsink_input_get_private (input);
        g_assert (channel != NULL);

        gdv_audioboy_channel_flush (this->Private->AudioBoy, channel);
        
        return TRUE;
}

/* Queue a partial buffer. Call with a lock held */
static void                     add_partial_buffer (GdvAudioCompSink *this, GstBuffer *buffer)
{
        g_return_if_fail (GDV_IS_AUDIOCOMPSINK (this));
        g_return_if_fail (buffer != NULL);
        
        GDV_DEBUG_ELEMENT ("Queuing a partial buffer of length %s", 
                           gdv_time_to_string (GST_BUFFER_TIMESTAMP (buffer)));
        
        this->Private->PartialBuffers = g_slist_append (this->Private->PartialBuffers, buffer);
}

/* Free all the buffers in the partial queue. This unrefs the buffers */
static int                      free_partials (GdvAudioCompSink *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), 0);
        
        GDV_DEBUG_ELEMENT ("Freeing all partial buffers", NULL);
        int count = 0;
        
        if (this->Private->PartialBuffers == NULL)
                return 0;

        GSList *iterator;
        // Iterate through all buffers
        for (iterator = this->Private->PartialBuffers; iterator; iterator = g_slist_next (iterator)) {
                GstBuffer *buffer = (GstBuffer *) iterator->data;
                g_assert (buffer != NULL);
                
                gst_buffer_unref (buffer);
        }
        
        g_slist_free (this->Private->PartialBuffers);
        this->Private->PartialBuffers = NULL;
        
        GDV_DEBUG_ELEMENT ("Freed %d partials", count);
        return count;
}

static int                      calculate_partials_size (GdvAudioCompSink *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), 0);
        
        GDV_DEBUG_ELEMENT ("Calculating partials size", NULL);
        int size = 0;
        
        if (this->Private->PartialBuffers == NULL)
                return 0;

        GSList *iterator;
        // Iterate through all buffers
        for (iterator = this->Private->PartialBuffers; iterator; iterator = g_slist_next (iterator)) {
                GstBuffer *buffer = (GstBuffer *) iterator->data;
                g_assert (buffer != NULL);
                
                size += GST_BUFFER_SIZE (buffer);
        }
                
        GDV_DEBUG_ELEMENT ("Partials size is %d", size);
        return size;
}

/* Main routine to handle the buffers coming out of the AudioBoy. 
 * Will return the buffer itself or add it to partials or render it with current partials */
static GstBuffer*               handle_new_buffer_with_partials (GdvAudioCompSink *this, GstBuffer *buffer)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), NULL);
        
        GDV_DEBUG_ELEMENT ("Handling new buffer with partials", NULL);
        
        // Fast pass-through
        if (GST_BUFFER_SIZE (buffer) >= MIN_BUFFER_SIZE && 
            this->Private->PartialBuffers == NULL) {
                GDV_DEBUG_ELEMENT ("Fast buffer pass-through", NULL);
                return buffer;
        }
        
        add_partial_buffer (this, buffer);
        
        if (calculate_partials_size (this) >= MIN_BUFFER_SIZE) {
                GstBuffer *mixedbuf = get_glued_partials (this);
                free_partials (this);
                return mixedbuf;
        } else 
                return NULL;
}

static GstBuffer*               get_glued_partials (GdvAudioCompSink *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), NULL);
        g_return_val_if_fail (this->Private->PartialBuffers != NULL, NULL);
        
        int size = calculate_partials_size (this);
        GstBuffer *firstbuffer = (GstBuffer *) this->Private->PartialBuffers->data;
        GstCaps *firstcaps = GST_BUFFER_CAPS (firstbuffer);
        GstClockTime firsttime = GST_BUFFER_TIMESTAMP (firstbuffer);
        GstClockTime duration = 0;
        gpointer outoffset;
        
        GDV_DEBUG_ELEMENT ("Gluing all partials into a %d buffer", size);
        
        GstBuffer *glue = gst_buffer_new_and_alloc (size);
        g_return_val_if_fail (glue != NULL, NULL);
        outoffset = GST_BUFFER_DATA (glue);
        
        // Glue it all via iteration
        GSList *iterator;
        
        for (iterator = this->Private->PartialBuffers; iterator; iterator = g_slist_next (iterator)) {
                GstBuffer *buffer = (GstBuffer *) iterator->data;
                g_assert (buffer != NULL);
                
                memcpy (outoffset, GST_BUFFER_DATA (buffer), GST_BUFFER_SIZE (buffer));
                outoffset += GST_BUFFER_SIZE (buffer);
                duration += GST_BUFFER_DURATION (buffer);
        }
        
        gst_buffer_set_caps (glue, firstcaps);
        GST_BUFFER_TIMESTAMP (glue) = firsttime;
        GST_BUFFER_DURATION (glue) = duration;
        
        return glue;                
}

// FIXME: Move to property
void                            gdv_audiocompsink_set_volume (GdvAudioCompSink *this, gdouble volume)
{
        g_return_if_fail (GDV_IS_AUDIOCOMPSINK (this));
        g_return_if_fail (this->Private->AudioBoy != NULL);

        gdv_audioboy_set_volume (this->Private->AudioBoy, volume);
}

// FIXME: Move to property
gdouble                         gdv_audiocompsink_get_volume (GdvAudioCompSink *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOCOMPSINK (this), 0);
        g_return_val_if_fail (this->Private->AudioBoy != NULL, 0);

        return gdv_audioboy_get_volume (this->Private->AudioBoy);
}


