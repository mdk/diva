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

/* A boxed struct to represent & manipulate frame dimensions (Width & Height)
 */

#include "GdvFrameDimensions.h"

/* GBoxed type */
GType                           gdv_framedimensions_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {
                objecttype = g_boxed_type_register_static 
                        ("GdvFrameDimensions", 
                         (GBoxedCopyFunc) gdv_framedimensions_copy,
                         (GBoxedFreeFunc) gdv_framedimensions_free);
        }
        
        return objecttype;
}

/* GBoxed copy */
GdvFrameDimensions*             gdv_framedimensions_copy (const GdvFrameDimensions *this)
{
        g_return_val_if_fail (this != NULL, NULL);
        
        return gdv_framedimensions_new (this->Width, this->Height);
}

/* GBoxed free */
void                            gdv_framedimensions_free (GdvFrameDimensions *this)
{
        g_return_if_fail (this != NULL);
        g_free (this);
}

/* Create new frame dimensions from given values */
GdvFrameDimensions*            gdv_framedimensions_new (gint32 width, gint32 height)
{
        GdvFrameDimensions *new = g_new (GdvFrameDimensions, 1);
        new->Width = width;
        new->Height = height;
        
        return new;
}

/* Scale to fit specified height (out is set) */
void                            gdv_framedimensions_fit_h (const GdvFrameDimensions *this, gint32 height,
                                                           GdvFrameDimensions *out)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (out != NULL);
        g_return_if_fail (height > 0);

        gdouble aspect = (gdouble) height / (gdouble) this->Height;
        gint32 w = aspect * this->Width;
        out->Width = w;
        out->Height = height;
}

/* Scale to fit specified width (out is set) */
void                            gdv_framedimensions_fit_w (const GdvFrameDimensions *this, gint32 width,
                                                           GdvFrameDimensions *out)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (out != NULL);
        g_return_if_fail (width > 0);
        
        gdouble aspect = (gdouble) width / (gdouble) this->Width;
        gint32 h = aspect * this->Height;
        out->Width = width;
        out->Height = h;
}

/* A typical -1, 0, 1 compare function */
gint                            gdv_framedimensions_compare (const GdvFrameDimensions *a, 
                                                             const GdvFrameDimensions *b)
{
        // FIXME: I don't know if this smaller/bigger comparisons are of any
        // use. Maybe we shouldn't treat this as comparable structs. Leaving 
        // this for now, but think twice before you use (doesn't apply to 
        // checking for equality = 0)

        g_return_val_if_fail (a != NULL && b != NULL, -1);
        
        if (a->Width == b->Width && a->Height == b->Height)
                return 0;
        
        if (a->Width * a->Height > b->Width * b->Height) 
                return 1;
        else
                return -1;
}

/* Set Width/Height. That simple */
void                            gdv_framedimensions_set (GdvFrameDimensions *this, guint32 width, guint32 height)
{
        this->Width = width;
        this->Height = height;
}

/* Produce a string describing the FrameDimensions. This is for debugging purposes only. Managed
 * code should use own functions */
gchar*                          gdv_framedimensions_to_string (const GdvFrameDimensions *this)
{
        if (this == NULL)
                return "(NULL)";
        else
                return g_strdup_printf ("%dx%d", this->Width, this->Height);
}

/* Round (down) the width/height to be multipliers of denomx/denomy. Will never make the frame
 * bigger than the original */
void                            gdv_framedimensions_roundify   (GdvFrameDimensions *this, int denomx, int denomy)
{
        g_return_if_fail (this != NULL);

        int xr = this->Width / denomx;
        int yr = this->Height / denomy;

        this->Width = xr * denomx;
        this->Height = yr * denomy;
}

GdvFrameDimensions*             gdv_framedimensions_from_caps (const GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, NULL);

        GstStructure *struc = gst_caps_get_structure (caps, 0);
        g_return_val_if_fail (struc != NULL, NULL);
        
        gint32 w, h;
        
        if (gst_structure_has_field_typed (struc, "width", G_TYPE_INT) == FALSE ||
            gst_structure_has_field_typed (struc, "height", G_TYPE_INT) == FALSE)
                return NULL;
        else {
                gst_structure_get_int (struc, "width", &w);
                gst_structure_get_int (struc, "height", &h);
                return gdv_framedimensions_new (w, h);
        }
}

