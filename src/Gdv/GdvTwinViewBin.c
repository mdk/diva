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

#include "GdvTwinViewBin.h"
#include "GdvTwinViewBinPrivate.h"

static GstBinClass*             ParentClass = NULL;

/* Private constructor */
static gboolean                 private_init (GdvTwinViewBin *this, GdvTwinViewBinPrivate *private)
{
        g_return_val_if_fail (private != NULL && this != NULL, FALSE);

        private->Limit1 = 0;
        private->Limit2 = 0;
        
        private->Source1 = NULL;
        private->Source2 = NULL;

        private->TwinCompSink = gdv_twincompsink_new ();
        gst_bin_add (GST_BIN (this), GST_ELEMENT (private->TwinCompSink));
        
        private->SrcPad = gst_ghost_pad_new ("src", gst_element_get_pad (GST_ELEMENT (private->TwinCompSink), "src"));
        g_return_val_if_fail (private->SrcPad != NULL, FALSE);

        gst_element_add_pad (GST_ELEMENT (this), private->SrcPad);
        
        return TRUE;
}

/* Private destructor */
static void                     private_dispose (GdvTwinViewBin *this, GdvTwinViewBinPrivate *private)
{
        // Everything gets done in the parent dispose
}

/* GObject Type */
GType                           gdv_twinviewbin_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvTwinViewBinClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_twinviewbin_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvTwinViewBin),
                        16,
                        (GInstanceInitFunc) gdv_twinviewbin_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvTwinViewBin", &objectinfo, 0);
        }

        return objecttype;
}

/* GObject class init */
static void                     gdv_twinviewbin_class_init (GdvTwinViewBinClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;

        gobjectclass->dispose = (gpointer) gdv_twinviewbin_dispose;
        gobjectclass->finalize = (gpointer) gdv_twinviewbin_finalize;
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
}

/* GObject init */
static void                     gdv_twinviewbin_init (GdvTwinViewBin *this)
{
        g_return_if_fail (GDV_IS_TWINVIEWBIN (this));
        this->Private = NULL;
        
        GdvTwinViewBinPrivate *priv = g_new0 (GdvTwinViewBinPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* GObject dispose */
static void                     gdv_twinviewbin_dispose (GdvTwinViewBin *this)
{
        g_return_if_fail (GDV_IS_TWINVIEWBIN (this));
        
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_twinviewbin_finalize (GdvTwinViewBin *this)
{
        g_return_if_fail (GDV_IS_TWINVIEWBIN (this));
        
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Create a new instance */
GdvTwinViewBin*                 gdv_twinviewbin_new (GdvProjectFormat *format, GdvSource *source1, GdvSource *source2)
{
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), NULL);
        g_return_val_if_fail (GDV_IS_SOURCE (source1), NULL);
        g_return_val_if_fail (GDV_IS_SOURCE (source2), NULL);

        GDV_DEBUG_EXT ("Creating new GdvTwinViewBin for '%s' and '%s'",
                       GST_OBJECT_NAME (source1), GST_OBJECT_NAME (source2));
                
        GdvTwinViewBin *this = g_object_new (GDV_TYPE_TWINVIEWBIN, NULL);
        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        }

        gdv_twincompsink_set_format (this->Private->TwinCompSink, format);

        if (! set_sources (this, source1, source2)) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else 
                return this;
}

static gboolean                 set_sources (GdvTwinViewBin *this, GdvSource *source1, GdvSource *source2)
{
        g_return_val_if_fail (GDV_IS_TWINVIEWBIN (this), FALSE);
        g_return_val_if_fail (GDV_IS_SOURCE (source1), FALSE);
        g_return_val_if_fail (GDV_IS_SOURCE (source2), FALSE);
        
        g_return_val_if_fail (this->Private->Source1 == NULL, FALSE);
        g_return_val_if_fail (this->Private->Source2 == NULL, FALSE);

        // Left source
        GDV_DEBUG_ELEMENT ("Setting source1 to '%s'", GST_OBJECT_NAME (source1));
        
        gst_object_ref (source1);
        this->Private->Source1 = source1;

        gst_bin_add (GST_BIN (this), GST_ELEMENT (source1));

        gst_element_link_pads (GST_ELEMENT (source1), "vsrc", GST_ELEMENT (this->Private->TwinCompSink), "leftsink");

        // Right source
        GDV_DEBUG_ELEMENT ("Setting source2 to '%s'", GST_OBJECT_NAME (source2));
        
        gst_object_ref (source2);
        this->Private->Source2 = source2;

        gst_bin_add (GST_BIN (this), GST_ELEMENT (source2));

        gst_element_link_pads (GST_ELEMENT (source2), "vsrc", GST_ELEMENT (this->Private->TwinCompSink), "rightsink");
        
        // Now, let's check the limits
        this->Private->Limit1 = check_limit (source1);
        this->Private->Limit2 = check_limit (source2);

        return TRUE;
}

static GdvTime                  check_limit (GdvSource *source)
{
        GdvTime limit = 0;
        // Now, let's check the limit
        g_object_get (G_OBJECT (source), "length", &limit, NULL);

        if (limit == 0)
                return limit;

        GdvVideoFormat *vformat = NULL;
        GdvFraction *fps;
        g_object_get (G_OBJECT (source), "videoformat", &vformat, NULL);
        g_return_val_if_fail (vformat != NULL, FALSE);

        g_object_get (G_OBJECT (vformat), "fps", &fps, NULL);
        g_return_val_if_fail (fps != NULL, FALSE);

        limit -= gdv_fraction_fps_frame_duration (fps);

        g_object_unref (vformat);
        gdv_fraction_free (fps);

        return limit;
}
        

gboolean                        gdv_twinviewbin_seek (GdvTwinViewBin *this, GdvTime time1, GdvTime time2, gboolean first)
{
        g_return_val_if_fail (GDV_IS_TWINVIEWBIN (this), FALSE);
        
        GDV_DEBUG_ELEMENT ("Seeking to %s:%s, limit is %s:%s",
                           gdv_time_to_string (time1), gdv_time_to_string (time2),
                           gdv_time_to_string (this->Private->Limit1),
                           gdv_time_to_string (this->Private->Limit2));

        // Let's make sure we never seek > than limit

        if (this->Private->Limit1 != 0)
                time1 = MIN (time1, this->Private->Limit1);

        if (this->Private->Limit2 != 0)
                time2 = MIN (time2, this->Private->Limit2);

        gst_element_seek (GST_ELEMENT (this->Private->Source1),
                          1.0,
                          GST_FORMAT_TIME,
                          GST_SEEK_FLAG_FLUSH,
                          GST_SEEK_TYPE_SET,
                          time1,
                          GST_SEEK_TYPE_SET,
                          -1);

        gst_element_seek (GST_ELEMENT (this->Private->Source2),
                          1.0,
                          GST_FORMAT_TIME,
                          GST_SEEK_FLAG_FLUSH,
                          GST_SEEK_TYPE_SET,
                          time2,
                          GST_SEEK_TYPE_SET,
                          -1);

        return TRUE;
}

