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

#include "GdvSingleViewBin.h"
#include "GdvSingleViewBinPrivate.h"

static GstBinClass*             ParentClass = NULL;

/* Private constructor */
static gboolean                 private_init (GdvSingleViewBin *this, GdvSingleViewBinPrivate *private)
{
        g_return_val_if_fail (private != NULL && this != NULL, FALSE);

        private->Limit = 0;
        private->Source = NULL;
        private->SrcPad = (GstPad *) gst_ghost_pad_new_no_target ("src", GST_PAD_SRC);
        g_return_val_if_fail (private->SrcPad != NULL, FALSE);

        gst_element_add_pad (GST_ELEMENT (this), private->SrcPad);
        
        return TRUE;
}

/* Private destructor */
static void                     private_dispose (GdvSingleViewBin *this, GdvSingleViewBinPrivate *private)
{
        // Everything gets done in the parent dispose
}

/* GObject Type */
GType                           gdv_singleviewbin_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvSingleViewBinClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_singleviewbin_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvSingleViewBin),
                        16,
                        (GInstanceInitFunc) gdv_singleviewbin_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvSingleViewBin", &objectinfo, 0);
        }

        return objecttype;
}

/* GObject class init */
static void                     gdv_singleviewbin_class_init (GdvSingleViewBinClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;

        gobjectclass->dispose = (gpointer) gdv_singleviewbin_dispose;
        gobjectclass->finalize = (gpointer) gdv_singleviewbin_finalize;
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
}

/* GObject init */
static void                     gdv_singleviewbin_init (GdvSingleViewBin *this)
{
        g_return_if_fail (GDV_IS_SINGLEVIEWBIN (this));
        this->Private = NULL;
        
        GdvSingleViewBinPrivate *priv = g_new0 (GdvSingleViewBinPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* GObject dispose */
static void                     gdv_singleviewbin_dispose (GdvSingleViewBin *this)
{
        g_return_if_fail (GDV_IS_SINGLEVIEWBIN (this));
        
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_singleviewbin_finalize (GdvSingleViewBin *this)
{
        g_return_if_fail (GDV_IS_SINGLEVIEWBIN (this));
        
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Create a new instance */
GdvSingleViewBin*               gdv_singleviewbin_new (GdvProjectFormat *format, GdvSource *source)
{
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), NULL);
        g_return_val_if_fail (GDV_IS_SOURCE (source), NULL);

        GDV_DEBUG_EXT ("Creating new GdvSingleViewBin for '%s'", GST_OBJECT_NAME (source));
                
        GdvSingleViewBin *this = g_object_new (GDV_TYPE_SINGLEVIEWBIN, NULL);
        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        }

        if (! set_source (this, source)) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else 
                return this;
}

static gboolean                 set_source (GdvSingleViewBin *this, GdvSource *source)
{
        g_return_val_if_fail (GDV_IS_SINGLEVIEWBIN (this), FALSE);
        g_return_val_if_fail (GDV_IS_SOURCE (source), FALSE);
        g_return_val_if_fail (this->Private->Source == NULL, FALSE);

        GDV_DEBUG_ELEMENT ("Setting source to '%s'", GST_OBJECT_NAME (source));
        
        gst_object_ref (source);
        this->Private->Source = source;

        gst_bin_add (GST_BIN (this), GST_ELEMENT (source));

        GstPad *sourcepad = gst_element_get_pad (GST_ELEMENT (source), "vsrc");
        g_return_val_if_fail (sourcepad != NULL, FALSE);
        // FIXME: Unref it after?

        gst_ghost_pad_set_target ((GstGhostPad *) this->Private->SrcPad, sourcepad);

        // Now, let's check the limit
        g_object_get (G_OBJECT (source), "length", &this->Private->Limit, NULL);

        // A source without a duration
        if (this->Private->Limit == 0)
                return TRUE;

        GdvVideoFormat *vformat = NULL;
        GdvFraction *fps;
        g_object_get (G_OBJECT (source), "videoformat", &vformat, NULL);
        g_return_val_if_fail (vformat != NULL, FALSE);

        g_object_get (G_OBJECT (vformat), "fps", &fps, NULL);
        g_return_val_if_fail (fps != NULL, FALSE);

        this->Private->Limit -= gdv_fraction_fps_frame_duration (fps);

        g_object_unref (vformat);
        gdv_fraction_free (fps);

        return TRUE;
}

gboolean                        gdv_singleviewbin_seek (GdvSingleViewBin *this, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_SINGLEVIEWBIN (this), FALSE);

        GDV_DEBUG_ELEMENT ("Seeking to %s, limit is %s",
                           gdv_time_to_string (time),
                           gdv_time_to_string (this->Private->Limit));

        // Let's make sure we never seek > than limit

        if (this->Private->Limit != 0)
                time = MIN (time, this->Private->Limit);
        
        return gst_element_seek (GST_ELEMENT (this->Private->Source),
                                 1.0,
                                 GST_FORMAT_TIME,
                                 GST_SEEK_FLAG_FLUSH,
                                 GST_SEEK_TYPE_SET,
                                 time,
                                 GST_SEEK_TYPE_SET,
                                 -1);
}
