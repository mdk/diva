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

#include "GdvVideoFormat.h"
#include "GdvVideoFormatPrivate.h"

/* GObject type */
GType                           gdv_videoformat_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {
                
                static const GTypeInfo objectinfo = {
                        sizeof (GdvVideoFormatClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_videoformat_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvVideoFormat),
                        32,
                        (GInstanceInitFunc) gdv_videoformat_init,
                };
                objecttype = g_type_register_static
                        (G_TYPE_OBJECT, "GdvVideoFormat", &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_videoformat_class_init (GdvVideoFormatClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;

        g_type_class_ref (G_TYPE_OBJECT);
        
        gobjectclass->get_property = (gpointer) gdv_videoformat_get_property;
        gobjectclass->set_property = (gpointer) gdv_videoformat_set_property;

        g_object_class_install_property (gobjectclass, ARG_FPS,
					 g_param_spec_boxed
					 ("fps", "Fps",
					  "Frames per second",
					  GDV_TYPE_FRACTION, G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_FRAMEDIMENSIONS,
                                         g_param_spec_boxed
					 ("framedimensions", "FrameDimensions",
					  "The frame dimensions describing this format",
					  GDV_TYPE_FRAMEDIMENSIONS, G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_PIXEL_ASPECT,
                                         g_param_spec_boxed
					 ("pixelaspect", "PixelAspect",
					  "The pixel aspect of the format",
					  GDV_TYPE_FRACTION, G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_FRAME_ASPECT,
                                         g_param_spec_boxed
					 ("frameaspect", "FrameAspect",
					  "The frame aspect ratio",
					  GDV_TYPE_FRACTION, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_CAPS,
                                         g_param_spec_pointer
					 ("caps", "Caps",
					  "The GstCaps of this format",
					  G_PARAM_READABLE));

        gobjectclass->dispose = (gpointer) gdv_videoformat_dispose;
        gobjectclass->finalize = (gpointer) gdv_videoformat_finalize;
}

/* GObject init */
static void                     gdv_videoformat_init (GdvVideoFormat *this)
{
        g_return_if_fail (GDV_IS_VIDEOFORMAT (this));
        GDV_DEBUG_EXT ("Initializing fresh GdvVideoFormat object", NULL);

        this->Private = NULL;

        GdvVideoFormatPrivate *priv = g_new0 (GdvVideoFormatPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Check if the given caps contain all the information that is needed to create
 * a valid video format */
gboolean                        gdv_videoformat_caps_ok (GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, FALSE);

        // Kick some initial assumptions
        if (gst_caps_is_any (caps) || gst_caps_is_empty (caps))
                return FALSE;

        if (! gst_caps_is_fixed (caps))
                return FALSE;

        GstStructure *struc = gst_caps_get_structure (caps, 0);

        // Check for availibility of all the crucial fields
        if (! gst_structure_has_field_typed (struc, "width", G_TYPE_INT))
                return FALSE;
        
        if (! gst_structure_has_field_typed (struc, "height", G_TYPE_INT))
                return FALSE;

        if (! gst_structure_has_field_typed (struc, "framerate", GST_TYPE_FRACTION))
                return FALSE;

        return TRUE;
}

/* Create a new object from the given caps. This refs the caps.
 * We check if the caps has all the params needed and if it's fixed */
GdvVideoFormat*                 gdv_videoformat_new (GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, NULL);
        g_return_val_if_fail (gdv_videoformat_caps_ok (caps), NULL);
        
        GdvVideoFormat *this = g_object_new (GDV_TYPE_VIDEOFORMAT, NULL);
        GDV_DEBUG_EXT ("Created a VideoFormat based on caps %s", gst_caps_to_string (caps));

        this->Private->Caps = gst_caps_copy (caps);
        
        return this;
}

/* Create from a string describing the caps (GstCaps) */
GdvVideoFormat*                 gdv_videoformat_new_from_string (const gchar *string)
{
        g_return_val_if_fail (string != NULL, NULL);
        GDV_DEBUG_EXT ("Creating new VideoFormat from string %s", string);

        GstCaps *caps = NULL;
        caps = gst_caps_from_string (string);
        g_return_val_if_fail (caps != NULL, NULL);

        return gdv_videoformat_new (caps);
}

GdvVideoFormat*                 gdv_videoformat_new_default (void)
{
        GstCaps *vcaps = gst_caps_new_simple ("video/x-raw-yuv",
                                              "width", G_TYPE_INT, 720,
                                              "height", G_TYPE_INT, 576,
                                              "format", GST_TYPE_FOURCC, GST_STR_FOURCC ("YUY2"),
                                              "framerate", GST_TYPE_FRACTION, 25, 1,
                                              "pixel-aspect-ratio", GST_TYPE_FRACTION, 59, 54,
                                              NULL);
        
        GdvVideoFormat *vformat = gdv_videoformat_new (vcaps);
        gst_caps_unref (vcaps);

        return vformat;
}

/* Save the format to a string, that can be later on used to bring the format 
 * back */
gchar*                          gdv_videoformat_serialize_to_string (GdvVideoFormat *this)
{
        g_return_val_if_fail (GDV_IS_VIDEOFORMAT (this), NULL);
        g_return_val_if_fail (this->Private->Caps != NULL, NULL);
        GDV_DEBUG_EXT ("Serializing VideoFormat to string", NULL);

        return gst_caps_to_string (this->Private->Caps);
}

/* GObject dispose */
void                            gdv_videoformat_dispose (GdvVideoFormat *this)
{
        g_return_if_fail (GDV_IS_VIDEOFORMAT (this));
        GDV_DEBUG_EXT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this->Private);
}

/* GObject finalize */
void                            gdv_videoformat_finalize (GdvVideoFormat *this)
{
        g_return_if_fail (GDV_IS_VIDEOFORMAT (this));
        GDV_DEBUG_EXT ("Finalizing", NULL);

        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
}        

/* GObject get property */
static void                     gdv_videoformat_get_property (GdvVideoFormat *this, guint propid,
                                                              GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_VIDEOFORMAT (this));

        GstStructure *structure = NULL;
        if (this->Private->Caps != NULL)
                structure = gst_caps_get_structure (this->Private->Caps, 0);

        g_return_if_fail (structure != NULL);
        
        switch (propid) {

	        case ARG_FRAMEDIMENSIONS: {
                        gint32 width = -1, height = -1;
                        
                        gst_structure_get_int (structure, "width", &width);
                        gst_structure_get_int (structure, "height", &height);
                        g_assert (width != -1 && height != -1);

                        GdvFrameDimensions *dimensions = gdv_framedimensions_new (width, height);
                        g_value_take_boxed (value, dimensions);
                } break;
                        
                case ARG_FPS: {
                        gint32 numerator = -1, denominator = -1;
                        
                        gst_structure_get_fraction (structure, "framerate", &numerator, &denominator);
                        g_assert (numerator != -1 && denominator != -1);

                        GdvFraction *fps = gdv_fraction_new (numerator, denominator);
                        g_value_take_boxed (value, fps);
                } break;

                case ARG_PIXEL_ASPECT: {
                        gint32 numerator = -1, denominator = -1;
                        gst_structure_get_fraction (structure, "pixel-aspect-ratio", &numerator, &denominator);

                        GdvFraction *pixelaspect = NULL;

                        if (numerator == -1 || denominator == -1)
                                pixelaspect = gdv_fraction_new (1, 1);
                        else
                                pixelaspect = gdv_fraction_new (numerator, denominator);
                        g_value_take_boxed (value, pixelaspect);
                } break;

                case ARG_FRAME_ASPECT: {
                        GdvFraction *pixelaspect = NULL;
                        GdvFrameDimensions *dimensions = NULL;
                        GdvFraction *one = gdv_fraction_new (1, 1);
                        
                        g_object_get (G_OBJECT (this),
                                      "pixelaspect", &pixelaspect,
                                      "framedimensions", &dimensions, NULL);
                        
                        g_return_if_fail (pixelaspect != NULL && dimensions != NULL);
                        GdvFrameDimensions *pixeldim = gdv_fraction_aspect_pixelize (one, pixelaspect, dimensions);
                        GdvFraction *output = gdv_fraction_aspect_new_from_frame (pixeldim);

                        gdv_fraction_free (pixelaspect);
                        gdv_fraction_free (one);
                        gdv_framedimensions_free (dimensions);
                        gdv_framedimensions_free (pixeldim);

                        g_value_take_boxed (value, output);
                } break;

                case ARG_CAPS:
                g_assert (this->Private->Caps != NULL);
                g_value_set_pointer (value, this->Private->Caps);
                gst_caps_ref (this->Private->Caps);
                break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GObject set property */
static void                     gdv_videoformat_set_property (GdvVideoFormat *this, guint propid,
                                                              const GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_VIDEOFORMAT (this));

        GstStructure *structure = NULL;
        if (this->Private->Caps != NULL)
                structure = gst_caps_get_structure (this->Private->Caps, 0);
        
        g_return_if_fail (structure != NULL);
        
        switch (propid) {
                
                case ARG_FRAMEDIMENSIONS: {
                        GdvFrameDimensions *dimensions = g_value_get_boxed (value);

                        g_return_if_fail (dimensions != NULL &&
                                          dimensions->Width > 0 &&
                                          dimensions->Height > 0);

                        gst_structure_set (structure, "width", G_TYPE_INT, dimensions->Width, NULL);
                        gst_structure_set (structure, "height", G_TYPE_INT, dimensions->Height, NULL);
                } break;
                        
                case ARG_FPS: {
                        GdvFraction *fps = g_value_get_boxed (value);

                        g_return_if_fail (fps != NULL &&
                                          (! GDV_FRACTION_IS_EMPTY (fps)));

                        gst_structure_set (structure, "framerate", GST_TYPE_FRACTION, fps->Numerator, fps->Denominator, NULL);
                } break;

                case ARG_PIXEL_ASPECT: {
                        GdvFraction *pixelaspect = g_value_get_boxed (value);

                        g_return_if_fail (pixelaspect != NULL &&
                                          (! GDV_FRACTION_IS_EMPTY (pixelaspect)));

                        gst_structure_set (structure, "pixel-aspect-ratio", GST_TYPE_FRACTION, pixelaspect->Numerator,
                                           pixelaspect->Denominator, NULL);
                } break;

                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* Check if the format caps are a part of supplied caps */
gboolean                        gdv_videoformat_is_compatible_caps (GdvVideoFormat *this, GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, FALSE);
        g_return_val_if_fail (GDV_IS_VIDEOFORMAT (this), FALSE);

        GDV_DEBUG_EXT ("Checking if %s is compatible with %s", gst_caps_to_string (caps), 
                       gst_caps_to_string (this->Private->Caps));

        GstCaps *newcaps = gst_caps_intersect (this->Private->Caps, caps);
        gboolean ret = gst_caps_is_empty (newcaps);
        gst_caps_unref (newcaps);
        
        return ret;
}

GdvVideoFormat*                 gdv_videoformat_clone (GdvVideoFormat *this)
{
        g_return_val_if_fail (GDV_IS_VIDEOFORMAT (this), NULL);

        GDV_DEBUG_EXT ("Cloning videoformat %s", gst_caps_to_string (this->Private->Caps));

        return gdv_videoformat_new (this->Private->Caps);
}

/* Private init */
static gboolean                 private_init (GdvVideoFormatPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        private->Caps = NULL;

        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvVideoFormatPrivate *private)
{
        g_return_if_fail (private != NULL);

        if (private->Caps != NULL) {
                gst_caps_unref (private->Caps);
                private->Caps = NULL;
        }
}
