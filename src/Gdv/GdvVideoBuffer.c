///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Senko Rasic <senko@senko.net>                                        //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//                                                                           //
// Permission is hereby granted, free of charge, to any person obtaining a   //
// copy of this software and associated documentation files (the             //
// "Software"), to deal in the Software without restriction, including       //
// without limitation the rights to use, copy, modify, merge, publish,       //
// distribute, sublicense, and/or sell copies of the Software, and to permit //
// persons to whom the Software is furnished to do so, subject to the        //
// following conditions:                                                     //
//                                                                           //
// The above copyright notice and this permission notice shall be included   //
// in all copies or substantial portions of the Software.                    //
//                                                                           //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

/* This is used for performing low-level operations on gst buffers, such
 * as converting between colorspaces and scaling.
 */

#include "GdvVideoBuffer.h"
#include "GdvBufferUtil.h"

/* Create a new GdvVideoBuffer with desired format and dimensions */
GdvVideoBuffer*                 gdv_videobuffer_new (GdvVideoBuffer *buffer, guint32 fourcc_to, int ow, int oh)
{
        g_assert (buffer != NULL);

        GstCaps *buffercaps = GST_BUFFER_CAPS (buffer);
        GstStructure *str_from = gst_caps_get_structure (buffercaps, 0);

        int w, h;
        guint32 fourcc_from;

        gst_structure_get_fourcc(str_from, "format", &fourcc_from);
        gst_structure_get_int(str_from, "width", &w);
        gst_structure_get_int(str_from, "height", &h);

        // FIXME - this is ugly hack; determine the exact size!
        GdvVideoBuffer *this = (GdvVideoBuffer *) gst_buffer_new_and_alloc (3 * ow * oh);
        g_assert (this != NULL);

        // Use the origin caps, changed to target format/width/height
        // so that caller doesn't need to specify full caps
        // FIXME - what about pixel aspect ratio? need to update that or what?
        GstCaps *thiscaps = gst_caps_copy (buffercaps);
        GstStructure *str = gst_caps_get_structure (thiscaps, 0);

        gst_structure_set (str, "format", GST_TYPE_FOURCC, fourcc_to, "width", G_TYPE_INT, ow, "height", G_TYPE_INT, oh, NULL);
        gst_buffer_set_caps ( GST_BUFFER (this), thiscaps);

        gst_buffer_stamp ( GST_BUFFER (this), GST_BUFFER (buffer));

        // Find the appropriate converter and convert the data
        gdv_videobuffer_converter_t converter = gdv_videobuffer_get_converter (fourcc_from, fourcc_to);
        g_assert (converter != NULL);

        GDV_DEBUG_EXT ("conversion to: %s", gst_caps_to_string (thiscaps));

        // FIXME - what about stride? currently only used in rgb, so default stride is 3*w
        converter (GST_BUFFER_DATA (buffer), w, h, 3 * w, GST_BUFFER_DATA (this), ow, oh, 3 * ow);

        return this;
}

/* Create a new GdvVideoBuffer with the desired caps */
GdvVideoBuffer*                 gdv_videobuffer_new_with_caps (GdvVideoBuffer *buffer, GstCaps *caps)
{
        g_assert (caps != NULL);

        GstStructure *str_to = gst_caps_get_structure (caps, 0);

        int ow, oh;
        guint32 fourcc_to;

        gst_structure_get_fourcc(str_to, "format", &fourcc_to);
        gst_structure_get_int(str_to, "width", &ow);
        gst_structure_get_int(str_to, "height", &oh);

        return gdv_videobuffer_new (buffer, fourcc_to, ow, oh);
}

void                            gdv_videobuffer_start_editing (GdvVideoBuffer *this)
{
        // FIXME: Unref old if changed?
        GstCaps *oldcaps = GST_BUFFER_CAPS (this);
        GstCaps *newcaps = gst_caps_make_writable (oldcaps);
        GST_BUFFER_CAPS ((GstBuffer *) this) = newcaps;
}

/* Set opacity param metadata */
void                            gdv_videobuffer_meta_set_opacity (GdvVideoBuffer *this, gdouble opacity)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gst_structure_set (struc, "gdvmeta-opacity", G_TYPE_DOUBLE, opacity, NULL);
}

/* Get opacity param metadata */
gdouble                         gdv_videobuffer_meta_get_opacity(GdvVideoBuffer *this)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        if (gdv_videobuffer_meta_has_opacity (this) == FALSE)
                return 1.0;
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gdouble opacity;
        gst_structure_get_double (struc, "gdvmeta-opacity", &opacity);
        return opacity;
}

/* Check if opacity param metadata is set */
gboolean                        gdv_videobuffer_meta_has_opacity (GdvVideoBuffer *this)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        return gst_structure_has_field (struc, "gdvmeta-opacity");
}

/* Check if this buffer has a information */
gboolean                        gdv_videobuffer_meta_has_null (GdvVideoBuffer *this)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        return gst_structure_has_field (struc, "gdvmeta-null");
}

/* Check if this buffer has a information */
gboolean                        gdv_videobuffer_meta_get_null (GdvVideoBuffer *this)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));

        if (gdv_videobuffer_meta_has_null (this) == FALSE)
                return FALSE;        
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gboolean null;
        gst_structure_get_boolean (struc, "gdvmeta-null", &null);
        return null;
}

/* Set null param metadata */
void                            gdv_videobuffer_meta_set_null (GdvVideoBuffer *this, gboolean null)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gst_structure_set (struc, "gdvmeta-null", G_TYPE_BOOLEAN, null, NULL);
}

void                            gdv_videobuffer_meta_set_layer (GdvVideoBuffer *this, gint32 layer)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));

        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gst_structure_set (struc, "gdvmeta-layer", G_TYPE_INT, layer, NULL);
        
}

gint32                          gdv_videobuffer_meta_get_layer (GdvVideoBuffer *this)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        if (gdv_videobuffer_meta_has_layer (this) == FALSE)
                return 1;
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        gint32 layer;
        gst_structure_get_int (struc, "gdvmeta-layer", &layer);
        return layer;
}

gboolean                        gdv_videobuffer_meta_has_layer(GdvVideoBuffer *this)
{
        g_assert (GDV_IS_VIDEOBUFFER (this));
        
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS (this), 0);
        g_assert (struc != NULL);

        return gst_structure_has_field (struc, "gdvmeta-layer");
}

/* Create a new GdvVideoBuffer from a GstBuffer. This might or might not
 * involve sub-buffering  */
GdvVideoBuffer*                 gdv_videobuffer_new_from_gst (GstBuffer *buffer)
{
        g_assert (buffer != NULL);

        gdv_videobuffer_start_editing ((GdvVideoBuffer *) buffer);
        
        GstCaps *caps = GST_BUFFER_CAPS (buffer);
        
        // Add out custom caps to recognize that this is a GdvVideoBuffer indeed
        GstStructure *struc = gst_caps_get_structure (caps, 0);
        g_assert (struc != NULL);
        gst_structure_set (struc, "gdvmeta-video", G_TYPE_BOOLEAN, TRUE, NULL);

        return (GdvVideoBuffer *) buffer;
}

/* Check if this is a GdvVideoBuffer indeed, not a plain GstBuffer */
gboolean                        gdv_videobuffer_is (gpointer this)
{
        if (this == NULL)
                return FALSE;
 
        GstStructure *struc = gst_caps_get_structure (GST_BUFFER_CAPS ((GstBuffer *) this), 0);
        g_assert (struc != NULL);

        return gst_structure_has_field (struc, "gdvmeta-video");
}
