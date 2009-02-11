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

#include "GdvAudioBuffer.h"

/* Create a new GdvAudioBuffer from a GstBuffer. This might or might not
 * involve sub-buffering  */
GdvAudioBuffer*                 gdv_audiobuffer_new_from_gst (GstBuffer *buffer)
{
        g_assert (buffer != NULL);

        gdv_audiobuffer_start_editing ((GdvAudioBuffer *) buffer);
        
        GstCaps *caps = GST_BUFFER_CAPS (buffer);
        
        // Add out custom caps to recognize that this is a GdvVideoBuffer indeed
        GstStructure *struc = gst_caps_get_structure (caps, 0);
        g_assert (struc != NULL);
        gst_structure_set (struc, "gdvmeta-audio", G_TYPE_BOOLEAN, TRUE, NULL);

        return (GdvAudioBuffer *) buffer;
}

/* Check if this is a GdvAudioBuffer indeed, not a plain GstBuffer */
gboolean                        gdv_audiobuffer_is (gpointer this)
{
        if (this == NULL)
                return FALSE;
 
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS ((GstBuffer *) this), 0);
        g_assert (struc != NULL);

        return gst_structure_has_field (struc, "gdvmeta-audio");
}

GstBuffer*                      gdv_audiobuffer_resample_16_2  (GstBuffer *buffer, gint32 targetrate)
{
        // Check the buffer caps
        GstCaps *caps = GST_BUFFER_CAPS (buffer);
        g_assert (caps != NULL);

        GstStructure *bufferstruct = gst_caps_get_structure (caps, 0);
        g_assert (bufferstruct != NULL);

        // Get the width, depth, endianess
        int samplerate;
        gst_structure_get_int (bufferstruct, "rate", &samplerate);

        gint samples = GST_BUFFER_SIZE (buffer) / 4;
        gint newsamples = ((float) samples / (float) samplerate) * targetrate;

        GDV_DEBUG_EXT ("Resampling %d samples at %d rate to %d at %d rate",
                       samples, samplerate, newsamples, targetrate);

        GstBuffer *newbuffer = gst_buffer_new_and_alloc (newsamples * 4);
        g_assert (newbuffer != NULL);

        GstCaps *newcaps = gst_caps_copy (GST_BUFFER_CAPS (buffer));
        gst_caps_set_simple (newcaps, "rate", G_TYPE_INT, targetrate, NULL);

        GST_BUFFER_CAPS (newbuffer) = newcaps;

        gfloat iterator = (float) samplerate / (float) targetrate;

        gint16 *outdata = (gpointer) GST_BUFFER_DATA (newbuffer);
        gint16 *indata = (gpointer) GST_BUFFER_DATA (buffer);

        gfloat cur = 0;
        int i = 0;

        
        // Left channel
        for (i = 0; i < newsamples; i++) {
                outdata [i * 2] = indata [((int) cur) * 2];
                cur += iterator;
        }

        cur = 0;
        // Right channel
        for (i = 0; i < newsamples; i++) {
                outdata [(i * 2) + 1] = indata [(((int) cur) * 2) + 1];
                cur += iterator;
        }
        
        // Copy the remaining data
        GST_BUFFER_DURATION (newbuffer) = GST_BUFFER_DURATION (buffer);
        GST_BUFFER_TIMESTAMP (newbuffer) = GST_BUFFER_TIMESTAMP (buffer);

        return newbuffer;
}

void                            gdv_audiobuffer_start_editing (GdvAudioBuffer *this)
{
        // FIXME: Unref old if changed?
        GstCaps *oldcaps = GST_BUFFER_CAPS (this);
        GstCaps *newcaps = gst_caps_make_writable (oldcaps);
        GST_BUFFER_CAPS ((GstBuffer *) this) = newcaps;
}

void                            gdv_audiobuffer_mix_16_signed (gpointer a, gpointer b, gpointer c,
                                                               gint32 size, gdouble volume)
{
        g_return_if_fail (size % 2 == 0);
        g_return_if_fail (a != NULL && b != NULL && c != NULL);

        gint16 *insamplea = a;
        gint16 *insampleb = b;
        gint16 *outsample = c;
        gint32 val;
        gint32 vol = (volume * volume * 256.0);

        int i;
        for (i = 0; i < size / 2; i++) {
                val = (256 * insamplea [i] + insampleb [i] * vol) / 256;
                val = MIN (G_MAXINT16, val);
                val = MAX (G_MININT16, val);

                outsample [i] = val;
        }
}

void                            gdv_audiobuffer_mix_16_unsigned (gpointer a, gpointer b, gpointer c,
                                                                 gint32 size, gdouble volume)
{
        g_return_if_fail (size % 2 == 0);
        g_return_if_fail (a != NULL && b != NULL && c != NULL);

        guint16 *insamplea = a;
        guint16 *insampleb = b;
        guint16 *outsample = c;
        guint32 val;
        gint32 vol = (volume * volume * 256.0);

        int i;
        for (i = 0; i < size / 2; i++) {
                val = (256 * insamplea [i] + insampleb [i] * vol) / 256;
                val = MIN (G_MAXUINT16, val);
                
                outsample [i] = val;
        }
}

void                            gdv_audiobuffer_mix_8_signed (gpointer a, gpointer b, gpointer c,
                                                              gint32 size, gdouble volume)
{
        g_return_if_fail (a != NULL && b != NULL && c != NULL);

        gint8 *insamplea = a;
        gint8 *insampleb = b;
        gint8 *outsample = c;
        gint16 val;
        gint32 vol = (volume * volume * 256.0);

        int i;
        for (i = 0; i < size; i++) {
                val = (256 * insamplea [i] + insampleb [i] * vol) / 256;
                val = MIN (G_MAXINT8, val);
                val = MAX (G_MININT8, val);

                outsample [i] = val;
        }
}

void                            gdv_audiobuffer_mix_8_unsigned (gpointer a, gpointer b, gpointer c,
                                                                gint32 size, gdouble volume)
{
        g_return_if_fail (a != NULL && b != NULL && c != NULL);

        guint8 *insamplea = a;
        guint8 *insampleb = b;
        guint8 *outsample = c;
        guint16 val;
        gint32 vol = (volume * volume * 256.0);

        int i;
        for (i = 0; i < size; i++) {
                val = (256 * insamplea [i] + insampleb [i] * vol) / 256;
                val = MIN (G_MAXUINT8, val);

                outsample [i] = val;
        }
}

gboolean                        gdv_audiobuffer_meta_has_null (GdvAudioBuffer *this)
{
        g_assert (GDV_IS_AUDIOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        return gst_structure_has_field (struc, "gdvmeta-null");
}

gboolean                        gdv_audiobuffer_meta_get_null (GdvAudioBuffer *this)
{
        g_assert (GDV_IS_AUDIOBUFFER (this));

        if (gdv_audiobuffer_meta_has_null (this) == FALSE)
                return FALSE;        
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gboolean null;
        gst_structure_get_boolean (struc, "gdvmeta-null", &null);
        return null;
}

void                            gdv_audiobuffer_meta_set_null (GdvAudioBuffer *this, gboolean null)
{
        g_assert (GDV_IS_AUDIOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gst_structure_set (struc, "gdvmeta-null", G_TYPE_BOOLEAN, null, NULL);
}
