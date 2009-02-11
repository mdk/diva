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

#include "GdvBlackSheep.h"
#include "GdvBlackSheepPrivate.h"

static GstStaticCaps            BaseCaps = GST_STATIC_CAPS ("video/x-raw-yuv, " \
                                                            "format = (fourcc) YUY2");

static GstElementClass*         ParentClass = NULL;

/* GObject type */
GType                           gdv_blacksheep_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvBlackSheepClass),
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_blacksheep_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvBlackSheep), 
                        4, 
                        (GInstanceInitFunc) gdv_blacksheep_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_SHEEP, "GdvBlackSheep", &objectinfo, 0);
        }
        return objecttype;
}

static void                     gdv_blacksheep_class_init (GdvBlackSheepClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        // GstElementClass *elementclass = (GstElementClass *) (klass);
        GdvSheepClass *sheepclass = (GdvSheepClass *) (klass);
        
        // Ref stuff 
        ParentClass = g_type_class_ref (GDV_TYPE_SHEEP);
        
        sheepclass->buffer_func = (gpointer) buffer_func;
        sheepclass->get_caps = (gpointer) get_caps_function;
        gobjectclass->dispose = (gpointer) gdv_blacksheep_dispose;
        gobjectclass->finalize = (gpointer) gdv_blacksheep_finalize;
}

static void                     gdv_blacksheep_init (GdvBlackSheep *this)
{
        GDV_DEBUG_EXT ("Initializing new GdvBlackSheep element", NULL);
        
        g_return_if_fail (GDV_IS_BLACKSHEEP (this));
        this->Private = NULL;

        GdvBlackSheepPrivate *priv = g_new0 (GdvBlackSheepPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                g_free (priv);
        }
}

/* Create a new instance */
GdvBlackSheep*                  gdv_blacksheep_new (GdvProjectFormat *format)
{
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), NULL);
        
        GDV_DEBUG_EXT ("Creating a new GdvBlackSheep", NULL);
        
        GdvBlackSheep *this = g_object_new (GDV_TYPE_BLACKSHEEP, NULL);
        if (this == NULL)
                return NULL;
        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }

        GdvVideoFormat *vformat;
        g_object_get (G_OBJECT (format), "videoformat", &vformat, NULL);

        GdvFraction *fps;
        g_object_get (G_OBJECT (vformat), "fps", &fps, NULL);
        
        private_set (this->Private, fps);

        if (vformat != NULL)
                g_object_unref (vformat);
        
        return this;
}

static GstBuffer*               buffer_func (GdvBlackSheep *this, GstPad *pad, GdvTime time, GdvTime endtime)
{
        guint memneeded = STUB_WIDTH * STUB_HEIGHT * 2;

        GstBuffer *buffer;
        //GstCaps *caps = gst_pad_get_negotiated_caps (pad);
        GstCaps *caps = get_caps_function (this, pad);
        g_return_val_if_fail (caps != NULL, NULL);
        
        buffer = gst_buffer_new_and_alloc (memneeded);
        g_assert (buffer != NULL);
        gst_buffer_set_caps (buffer, caps);


#ifdef TRUE_BLACKNESS
        int i;
        for (i = 0; i < memneeded; i+= 2) {
                GST_BUFFER_DATA (buffer) [i] = 0;
                GST_BUFFER_DATA (buffer) [i + 1] = 128;
        }
#endif

#ifdef GREEN_BLACKNESS
        int i;
        for (i = 0; i < memneeded; i+= 2) {
                GST_BUFFER_DATA (buffer) [i] = 0;
                GST_BUFFER_DATA (buffer) [i + 1] = 0;
        }
#endif
        
        buffer = (GstBuffer *) gdv_videobuffer_new_from_gst (buffer);
        gdv_videobuffer_meta_set_null ((GdvVideoBuffer *) buffer, TRUE);
        
        GST_BUFFER_TIMESTAMP (buffer) = time;
        GST_BUFFER_DURATION (buffer) = this->Private->BufferLength;
        //        (endtime != -1) ? MIN (this->Private->BufferLength, endtime - time) : this->Private->BufferLength;
        
        return buffer;
}

static GstCaps*                 get_caps_function (GdvBlackSheep *this, GstPad *pad)
{
        g_return_val_if_fail (GDV_IS_BLACKSHEEP (this), NULL);
        g_return_val_if_fail (pad != NULL, NULL);
                
        GstCaps *modcaps = gst_caps_make_writable (gst_static_caps_get (&BaseCaps));
        g_return_val_if_fail (modcaps != NULL, NULL);
        
        GstStructure *struc = gst_caps_get_structure (modcaps, 0);
        g_return_val_if_fail (struc != NULL, NULL);
        
        // Add the fps information to the caps
        gdv_fraction_fps_set_gst (this->Private->Fps, struc);
        gst_structure_set (struc, "width", G_TYPE_INT,
                           STUB_WIDTH, NULL);
        gst_structure_set (struc, "height", G_TYPE_INT,
                           STUB_HEIGHT, NULL);
        
        return modcaps;
}

/* GObject dispose */
static void                     gdv_blacksheep_dispose (GdvBlackSheep *this)
{
        g_return_if_fail (GDV_IS_BLACKSHEEP (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_blacksheep_finalize (GdvBlackSheep *this)
{
        g_return_if_fail (GDV_IS_BLACKSHEEP (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Private init */
static gboolean                 private_init (GdvBlackSheep *this, GdvBlackSheepPrivate *private)
{
        g_return_val_if_fail (GDV_IS_BLACKSHEEP (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);
        
        // A sane default
        private->Fps = NULL;
        GdvFraction *new = gdv_fraction_new (25, 1);
        private_set (private, new);
                        
        return TRUE;
}

/* Private set */
static void                     private_set (GdvBlackSheepPrivate *private, GdvFraction *fps)
{
        g_return_if_fail (private != NULL);

        if (private->Fps != NULL)
                gdv_fraction_free (private->Fps);
        
        private->Fps = fps;
        gdouble l = gdv_fraction_to_double (fps);
        private->BufferLength = (GstClockTime) ((1.0 / l) * GST_SECOND);
}
