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

#include "GdvAudioBoy.h"
#include "GdvAudioBoyPrivate.h"

static GObjectClass*            ParentClass = NULL;

/* GObject type */
GType                           gdv_audioboy_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvAudioBoyClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) gdv_audioboy_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvAudioBoy),
                        8, 
                        (GInstanceInitFunc) gdv_audioboy_init,
                };
                objecttype = g_type_register_static (G_TYPE_OBJECT, "GdvAudioBoy", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_audioboy_class_init (GdvAudioBoyClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (G_TYPE_OBJECT);
        
        gobjectclass->dispose = (gpointer) gdv_audioboy_dispose;
        gobjectclass->finalize = (gpointer) gdv_audioboy_finalize;
}

/* GObject dispose */
static void                     gdv_audioboy_dispose (GdvAudioBoy *this)
{
        g_return_if_fail (GDV_IS_AUDIOBOY (this));
        GDV_DEBUG_EXT ("Disposing", NULL);

        if (this->Private != NULL) 
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_audioboy_finalize (GdvAudioBoy *this)
{
        g_return_if_fail (GDV_IS_AUDIOBOY (this));
        GDV_DEBUG_EXT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject init */
static void                     gdv_audioboy_init (GdvAudioBoy *this)
{
        g_return_if_fail (GDV_IS_AUDIOBOY (this));
        this->Private = NULL;

        GdvAudioBoyPrivate *priv = g_new0 (GdvAudioBoyPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Private init */
static gboolean                 private_init (GdvAudioBoyPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        private->ChannelList = NULL;
        private->MasterCaps = NULL;
        private->MasterRate = -1;
        private->MasterDepth = -1;
        private->MasterChannels = -1;
        private->Volume = 1.0;
        
        private->Mutex = g_mutex_new ();
        g_return_val_if_fail (private->Mutex != NULL, FALSE);
                
        return TRUE;
}

static gboolean                 private_set (GdvAudioBoyPrivate *private, GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, FALSE);

        private->MasterCaps = caps;
        gst_caps_ref (private->MasterCaps);

        // Extract some data from caps
        GstStructure *capsstruct = gst_caps_get_structure (caps, 0);
        g_return_val_if_fail (capsstruct != NULL, FALSE);

        gint32 rate = -1;
        gint32 width = -1;
        gint32 channels = -1;
        gboolean ssigned = FALSE;

        gst_structure_get_int (capsstruct, "rate", &rate);
        gst_structure_get_int (capsstruct, "width", &width);
        gst_structure_get_int (capsstruct, "channels", &channels);
        gst_structure_get_boolean (capsstruct, "signed", &ssigned);
        
        g_return_val_if_fail (width != -1 && channels != -1 &&
                              rate != -1, FALSE);

        private->MasterRate = rate;
        private->MasterChannels = channels;
        private->MasterDepth = width;
        private->MasterSigned = ssigned;

        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvAudioBoy *this, GdvAudioBoyPrivate *private)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (GDV_IS_AUDIOBOY (this));

        if (private->Mutex != NULL) {
                g_mutex_free (private->Mutex);
                private->Mutex = NULL;
        }

        //if (private->DataList != NULL)
        //        free_data_list (this);

        if (private->MasterCaps != NULL) {
                gst_caps_unref (private->MasterCaps);
                private->MasterCaps = NULL;
        }

}

/* Create a new instance */
GdvAudioBoy*                    gdv_audioboy_new (GstCaps *caps)
{
        GDV_DEBUG_EXT ("Creating a new GdvAudioBoy", NULL);
        
        GdvAudioBoy *this = g_object_new (GDV_TYPE_AUDIOBOY, NULL);

        if (this == NULL)
                return NULL;

        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }
        
        if (! private_set (this->Private, caps)) {
                g_object_unref (this);
                return NULL;
        }
        
        return this;
}

gboolean                        gdv_audioboy_channel_flush (GdvAudioBoy *this, GdvAudioBoyChannel *channel)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), FALSE);
        
        BOY_LOCK (this);

        gboolean result;

        if (channel->Status == CHANNEL_STATUS_EMPTY)
                result = FALSE;
        else {
                gst_buffer_unref (channel->Buffer);
                channel->Buffer = NULL;
                channel->Status = CHANNEL_STATUS_EMPTY;
                result = TRUE;
        }

        BOY_UNLOCK (this);

        return result;
}

gboolean                        gdv_audioboy_channel_is_empty (GdvAudioBoy *this,
                                                               GdvAudioBoyChannel *channel)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), FALSE);

        BOY_LOCK (this);

        gboolean result;

        if (channel->Status == CHANNEL_STATUS_EMPTY)
                result = TRUE;
        else
                result = FALSE;

        BOY_UNLOCK (this);

        return result;
}

GdvAudioBoyChannel*             gdv_audioboy_add_channel (GdvAudioBoy *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), NULL);

        GDV_DEBUG_EXT ("Adding new channel", NULL);

        GdvAudioBoyChannel *channel = g_new (GdvAudioBoyChannel, 1);
        channel->Buffer = NULL;
        channel->Status = CHANNEL_STATUS_EMPTY;
        channel->Position = 0;
        channel->Nullish = FALSE;

        BOY_LOCK (this);
        this->Private->ChannelList = g_list_append (this->Private->ChannelList, (gpointer) channel);
        BOY_UNLOCK (this);

        return channel;
}

void                            gdv_audioboy_channel_add_buffer (GdvAudioBoy *this, GdvAudioBoyChannel *channel,
                                                                 GstBuffer *buffer)
{
        g_return_if_fail (GDV_IS_AUDIOBOY (this));

        BOY_LOCK (this);

        // Extract some data from caps
        GstCaps *caps = GST_BUFFER_CAPS (buffer);
        g_return_if_fail (caps != NULL);
        
        GstStructure *capsstruct = gst_caps_get_structure (caps, 0);
        g_return_if_fail (capsstruct != NULL);

        gint32 rate = -1;
        gint32 width = -1;
        gint32 channels = -1;
        gboolean ssigned = FALSE;
        gboolean nullish = FALSE;

        gst_structure_get_int (capsstruct, "rate", &rate);
        gst_structure_get_int (capsstruct, "width", &width);
        gst_structure_get_int (capsstruct, "channels", &channels);
        gst_structure_get_boolean (capsstruct, "signed", &ssigned);

        // Check if it's a null one
        if (GDV_IS_AUDIOBUFFER (buffer)) {

                GdvAudioBuffer *audiobuffer = (GdvAudioBuffer *) buffer;

                // We skip it completely
                if (gdv_audiobuffer_meta_get_null (audiobuffer) == TRUE)
                        nullish = TRUE;
        }

        GDV_DEBUG_EXT ("Adding buffer %d %d bpp %d channeld signed %d size %d nullish %d",
                       rate, width, channel, ssigned, GST_BUFFER_SIZE (buffer), nullish);

        // Check if we're compatible
        if (rate != this->Private->MasterRate ||
            width != this->Private->MasterDepth ||
            channels != this->Private->MasterChannels ||
            ssigned != this->Private->MasterSigned) {
                GDV_WARNING_EXT ("Incompatible buffer, ignoring", NULL);
                goto done;
        }

        // Check if we're empty
        if (channel->Buffer != NULL) {
                GDV_WARNING_EXT ("Adding a buffer to a non-empty channel", NULL);
                gst_buffer_unref (channel->Buffer);
        }

        channel->Buffer = buffer;
        channel->Status = CHANNEL_STATUS_FILLED;
        channel->Position = 0;
        channel->Nullish = nullish;
        
        gst_buffer_ref (buffer);

 done:
        BOY_UNLOCK (this);
}

/* Call with a lock held */
static void                     get_sizes (GdvAudioBoy *this, gint32 *min,
                                           gint32 *max)
{
        g_return_if_fail (GDV_IS_AUDIOBOY (this));
        g_return_if_fail (min != NULL || max != NULL);

        gint32 minsize = G_MAXINT32;  // The smallest buffer size
        gint32 maxsize = 0;           // The largest buffer size

        GDV_DEBUG_EXT ("Checking min/max size of the buffers in channels", NULL);
        
        GList *iterator;

        // Iterate through all inputs
        for (iterator = this->Private->ChannelList; iterator; iterator = g_list_next (iterator)) {
                GdvAudioBoyChannel *channel = (GdvAudioBoyChannel *) iterator->data;
                
                if (channel == NULL)
                        goto error;

                if (channel->Status != CHANNEL_STATUS_FILLED)
                        goto error;

                if (channel->Buffer == NULL)
                        goto error;
                                
                minsize = MIN (minsize, GST_BUFFER_SIZE (channel->Buffer) - channel->Position);
                maxsize = MAX (maxsize, GST_BUFFER_SIZE (channel->Buffer) - channel->Position);
        }
        
        goto done;

 error:
        GDV_WARNING_EXT ("Bad data in channels", NULL);
        minsize = -1;
        maxsize = -1;

 done:
        if (min != NULL)
                *min = minsize;
        if (max != NULL)
                *max = maxsize;
        
}

GstBuffer*                      gdv_audioboy_get_buffer (GdvAudioBoy *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), NULL);

        GDV_DEBUG_EXT ("Getting a mixed buffer", NULL);

        BOY_LOCK (this);

        gint32 min, max;
        GstBuffer *buffer = NULL;

        // Get the sizes
        get_sizes (this, &min, &max);
        if (min == -1 ||
            max == -1) {
                GDV_WARNING_EXT ("Failed to get min/max sizes", NULL);
                goto error;
        }
        GDV_DEBUG_EXT ("Max mix: %d Min mix: %d", max, min);
                
        // Create a buffer
        buffer = get_master (this, min);
        if (buffer == NULL) {
                GDV_WARNING_EXT ("Failed to allocate the master mix buffer", NULL);
                goto error;
        }

        // For each buffer mix it in. For a first buffer we just copy the mem
        GList *iterator;
        gboolean first = TRUE;
        gboolean something = FALSE;
        gint32 num = 0;

        GDV_DEBUG_EXT ("Starting mixing", NULL);
        for (iterator = this->Private->ChannelList; iterator; iterator = g_list_next (iterator)) {

                GdvAudioBoyChannel *channel = (GdvAudioBoyChannel *) iterator->data;
                g_assert (channel != NULL);

                gpointer inputmem = read_input (this, channel, min);
                gpointer outputmem = GST_BUFFER_DATA (buffer);
                
#ifdef OPTIMIZE_NULL
                if (channel->Nullish == TRUE)
                        goto skip;
#endif

                if (first && (this->Private->Volume == 1.0)) {
                        memcpy (outputmem, inputmem, min);
                } else {
                        if (first) {
                                memset (outputmem, 0, min);
                        } 

                        if (this->Private->MasterDepth == 16) {
                                // Mixing 16
                                if (this->Private->MasterSigned == TRUE)
                                        gdv_audiobuffer_mix_16_signed (outputmem, inputmem, outputmem, min, this->Private->Volume);
                                else
                                        gdv_audiobuffer_mix_16_unsigned (outputmem, inputmem, outputmem, min, this->Private->Volume);
                        } else if (this->Private->MasterDepth == 8) {
                                // Mixing 8
                                if (this->Private->MasterSigned == TRUE)
                                        gdv_audiobuffer_mix_8_signed (outputmem, inputmem, outputmem, min, this->Private->Volume);
                                else
                                        gdv_audiobuffer_mix_8_unsigned (outputmem, inputmem, outputmem, min, this->Private->Volume);
                        }
                }

                something = TRUE;

                first = FALSE;

        skip:
                num++;
        }

        // If we haven't filled the buffer at all...
        if (something == FALSE)
                memset (GST_BUFFER_DATA (buffer), 0, min);
        
        GDV_DEBUG_EXT ("Mixed %d channels", num);

        gint divisor = (this->Private->MasterDepth == 16) ? 2 : 1;
        divisor *= this->Private->MasterChannels;

        GST_BUFFER_DURATION (buffer) = GST_SECOND / (this->Private->MasterRate / (min / divisor));

        // Flush
        flush (this);
        goto done;        

 error:
        if (buffer != NULL)
                gst_buffer_unref (buffer);
        buffer = NULL;
        
 done:
        BOY_UNLOCK (this);
        return buffer;
}

static GstBuffer*               get_master (GdvAudioBoy *this, gint32 size)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), NULL);

        GDV_DEBUG_EXT ("Getting master buffer, size %d", size);
        
        GstBuffer *master = gst_buffer_new_and_alloc (size);
        g_return_val_if_fail (master != NULL, NULL);
        
        gst_buffer_set_caps (master, this->Private->MasterCaps);

        return master;
}

/* Read the given amount of bytes from a stacked input. This doesn't actually "read"
 * anything, just gets a pointer and increases the offset. Call with a lock held. */
static gpointer                 read_input (GdvAudioBoy *this, GdvAudioBoyChannel *channel,
                                            gint32 bytes)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), NULL);
        g_return_val_if_fail (channel != NULL, NULL);
        g_return_val_if_fail (channel->Status == CHANNEL_STATUS_FILLED, NULL);

        if (bytes + channel->Position > GST_BUFFER_SIZE (channel->Buffer)) {
                GDV_WARNING_EXT ("Trying to read more than availible", NULL);
                return NULL;
        }
        
        gpointer ptr = GST_BUFFER_DATA (channel->Buffer) + channel->Position;
        channel->Position += bytes;
        
        return ptr;
}

static gint32                  flush (GdvAudioBoy *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), 0);
        
        gint32 flushed = 0; // Number of buffers flushed

        GList *iterator;
        // Iterate through all inputs
        for (iterator = this->Private->ChannelList; iterator; iterator = g_list_next (iterator)) {
                GdvAudioBoyChannel *channel = (GdvAudioBoyChannel *) iterator->data;
                g_assert (channel != NULL);
                
                if (channel->Status != CHANNEL_STATUS_EMPTY &&
                    channel->Buffer != NULL &&
                    channel->Position >= GST_BUFFER_SIZE (channel->Buffer)) {
                        // We need to flush it
                        gst_buffer_unref (channel->Buffer);
                        channel->Buffer = NULL;
                        channel->Status = CHANNEL_STATUS_EMPTY;
                        channel->Position = 0;
                        flushed++;
                }
        }

        return flushed;
}

gboolean                        gdv_audioboy_all_filled (GdvAudioBoy *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), FALSE);

        BOY_LOCK (this);

        GList *iterator;
        gboolean result = FALSE;
        
        // Iterate through all inputs
        for (iterator = this->Private->ChannelList; iterator; iterator = g_list_next (iterator)) {
                GdvAudioBoyChannel *channel = (GdvAudioBoyChannel *) iterator->data;
                g_assert (channel != NULL);

                if (channel->Status != CHANNEL_STATUS_FILLED)
                        goto done;
        }

        result = TRUE;

 done:
        BOY_UNLOCK (this);
        return result;
}

// FIXME: Move to property
void                            gdv_audioboy_set_volume (GdvAudioBoy *this, gdouble volume)
{
        g_return_if_fail (GDV_IS_AUDIOBOY (this));

        BOY_LOCK (this);
        this->Private->Volume = volume;
        BOY_UNLOCK (this);
}

// FIXME: Move to property
gdouble                         gdv_audioboy_get_volume (GdvAudioBoy *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOBOY (this), 0);

        gdouble volume;

        BOY_LOCK (this);
        volume = this->Private->Volume;
        BOY_UNLOCK (this);

        return volume;
}

