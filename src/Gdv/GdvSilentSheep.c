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

#include "GdvSilentSheep.h"
#include "GdvSilentSheepPrivate.h"


static GstStaticCaps            BaseCaps = GST_STATIC_CAPS ("audio/x-raw-int, " \
                                                            "width = (int) 16, " \
                                                            "depth = (int) 16, " \
                                                            "channels = (int) 2, " \
                                                            "signed = (boolean) true, " \
                                                            "endianness = (int) 1234");

static GstElementClass*         ParentClass = NULL;

/* GObject type */
GType                           gdv_silentsheep_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvSilentSheepClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_silentsheep_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvSilentSheep), 
                        4, 
                        (GInstanceInitFunc) gdv_silentsheep_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_SHEEP, "GdvSilentSheep", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_silentsheep_class_init (GdvSilentSheepClass *klass)
{
        // Ref stuff 
        ParentClass = g_type_class_ref (GDV_TYPE_SHEEP);

        GdvSheepClass *sheepclass = (GdvSheepClass *) (klass);
        
        sheepclass->buffer_func = (gpointer) buffer_func;
        sheepclass->get_caps = (gpointer) get_caps_function;
}

/* GObject init */
static void                     gdv_silentsheep_init (GdvSilentSheep *this)
{
        GDV_DEBUG_EXT ("Initializing new GdvSilentSheep element", NULL);
}

/* Create a new instance */
GdvSilentSheep*                 gdv_silentsheep_new (GdvProjectFormat *format)
{
        GdvSilentSheep *this = g_object_new (GDV_TYPE_SILENTSHEEP, NULL);
        if (this == NULL)
                return NULL;

        return this;
}

/* PushSrc overriden. Our main loopy function */
static GstBuffer*               buffer_func (GdvSilentSheep *this, GstPad *pad, GdvTime time, GdvTime endtime)
{
        g_return_val_if_fail (GDV_IS_SILENTSHEEP (this), NULL);
        g_return_val_if_fail (pad != NULL, NULL);
        
        int size = STUB_SAMPLES / 10;

        GstBuffer* buffer = gst_buffer_new_and_alloc (size * 4);
        g_return_val_if_fail (buffer != NULL, NULL);

#ifdef NOISY_SILENCE
        // Generate brown noise
        int i = 0;
        gint16 *p = (gint16 *) GST_BUFFER_DATA (buffer);
        for (i = 0; i < size * 2; i++) {
                p [i] = rand () / (RAND_MAX / 1000); 
        }
#endif
        // Stamp it
        GST_BUFFER_TIMESTAMP (buffer) = time;
        GST_BUFFER_DURATION (buffer) = GST_SECOND / 10;
        GST_BUFFER_CAPS (buffer) = gst_pad_get_negotiated_caps (pad);

        // Mark it as null
        buffer = (GstBuffer *) gdv_audiobuffer_new_from_gst (buffer);
        gdv_audiobuffer_meta_set_null ((GdvAudioBuffer *) buffer, TRUE);
                
        return buffer;
}

static GstCaps*                 get_caps_function (GdvSilentSheep *this, GstPad *pad)
{
        g_return_val_if_fail (GDV_IS_SILENTSHEEP (this), NULL);
        g_return_val_if_fail (pad != NULL, NULL);
                
        GstCaps *modcaps = gst_caps_make_writable (gst_static_caps_get (&BaseCaps));
        g_return_val_if_fail (modcaps != NULL, NULL);
        
        GstStructure *struc = gst_caps_get_structure (modcaps, 0);
        g_return_val_if_fail (struc != NULL, NULL);
        
        // Add the rate information to the caps
        gst_structure_set (struc, "rate", G_TYPE_INT,
                           STUB_SAMPLES, NULL);
        
        return modcaps;
}

