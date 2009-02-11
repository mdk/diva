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

/* A boxed struct to represent a fraction. This is used for various apspect rates,
 * framerates, etc.
 */

#include "GdvFraction.h"

/* Private */

static gint32                   calculate_gcd (gint32 a, gint32 b, gint32 *c, gint32 *d);

/* GBoxed type */
GType                           gdv_fraction_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {
                objecttype = g_boxed_type_register_static 
                        ("GdvFraction", 
                         (GBoxedCopyFunc) gdv_fraction_copy,
                         (GBoxedFreeFunc) gdv_fraction_free);
        }
        
        return objecttype;
}

/* GBoxed copy */
GdvFraction*                    gdv_fraction_copy (const GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, NULL);

        GdvFraction *new = g_new (GdvFraction, 1);
        new->Numerator = this->Numerator;
        new->Denominator = this->Denominator;
        new->Simple = this->Simple;
        
        return new;
}

/* GBoxed free */
void                            gdv_fraction_free (GdvFraction *this)
{
        g_assert (this != NULL);
        g_free (this);
}

/* A fraction is 'empty' if denominator == 0 */
gboolean                        gdv_fraction_is_empty (const GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, FALSE);

        return (this->Denominator == 0) ? TRUE : FALSE ;
}

/* Create a new fraction. We try to simplify it right from the start*/
GdvFraction*                    gdv_fraction_new (gint32 num, gint32 den)
{
        GdvFraction *new = g_new (GdvFraction, 1);
        g_assert (new != NULL);
        
        new->Numerator = num;
        new->Denominator = den;

        if (den != 0)
                gdv_fraction_simplify (new);
        
        return new;
}

/* Create a new, empty fraction */
GdvFraction*                    gdv_fraction_new_empty (void)
{
        GdvFraction *new = g_new (GdvFraction, 1);
        new->Numerator = 0;
        new->Denominator = 0;
        return new;
}

/* Convert to a double value */
gdouble                         gdv_fraction_to_double (const GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, -1);

        if (this->Denominator == 0)
                return 0.0;

        return ((double) this->Numerator / (double) this->Denominator);
}

/* Try to simplify the fraction using gcd. Returns TRUE if the fraction was simplified.
 * It doesn't check if the fraction is simple already (on purpose) */
gboolean                        gdv_fraction_simplify (GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (this->Denominator != 0, FALSE);
        
        gint32 c,d;
        this->Simple = TRUE;
        gint32 gcd = calculate_gcd (this->Numerator, this->Denominator, &c, &d);
        
        if (gcd == -1)
                return FALSE;
        else {
                this->Numerator /= gcd;
                this->Denominator /= gcd;
                this->Simple = TRUE;
                
                return TRUE;
        }
}

/* A standard comparer. 0 = equals, -1 = less, 1 = more etc. It might simplify the fraction */
gint32                          gdv_fraction_compare (GdvFraction *this, GdvFraction *that)
{
        g_return_val_if_fail (this != NULL && that != NULL, 0);

        // Empty equals
        if (GDV_FRACTION_IS_EMPTY (this) && GDV_FRACTION_IS_EMPTY (that))
                return 0;

        // Simplify
        if (! GDV_FRACTION_IS_SIMPLE (this))
                gdv_fraction_simplify (this);

        if (! GDV_FRACTION_IS_SIMPLE (that))
                gdv_fraction_simplify (that);

        // Equals
        if (this->Numerator == that->Numerator &&
            this->Denominator == that->Denominator)
                return 0;

        gdouble this_dbl = gdv_fraction_to_double (this);
        gdouble that_dbl = gdv_fraction_to_double (that);

        return (this_dbl < that_dbl) ? -1 : 1 ;
}

/* Invert a fraction */
GdvFraction*                    gdv_fraction_invert (const GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, NULL);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), NULL);

        GdvFraction *new = g_new (GdvFraction, 1);
        g_assert (new != NULL);

        new->Numerator = this->Denominator;
        new->Denominator = this->Numerator;
        new->Simple = this->Simple;
        return new;
}

/* Convert the fraction to string. For debugging purposes mostly/only. Will try to
 * simplify the fraction */
gchar*                          gdv_fraction_to_string (GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, "(null)");

        if (GDV_FRACTION_IS_EMPTY (this))
                return "(empty)";
        
        if (! GDV_FRACTION_IS_SIMPLE (this))
                gdv_fraction_simplify (this);
        
        return g_strdup_printf ("%d/%d", this->Numerator, this->Denominator);
}

/* If the fraction represents a fps (frames per second -- ie. 30/1) calculate the
 * duration of a single frame (in nano-seconds) */
GdvTime                         gdv_fraction_fps_frame_duration (GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        if (! GDV_FRACTION_IS_SIMPLE (this))
                gdv_fraction_simplify (this);
        
        return (this->Denominator * GDV_SECOND) / this->Numerator;
}

/* Convert the number of frames (no) into a valid time using the supplied fraction as
 * the fps value */
GdvTime                         gdv_fraction_fps_time_at_frame (GdvFraction *this, gint32 no)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        return (guint64) no * gdv_fraction_fps_frame_duration (this);
}

/* Return a frame at a given time. The frame number is rounded down or up. */
gint32                          gdv_fraction_fps_frame_at_time_rnd (GdvFraction *this, GdvTime time)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        GdvTime frameduration = gdv_fraction_fps_frame_duration (this);
        g_assert (frameduration > 2);
        GdvTime halfduration = frameduration / 2;

        gint32 no = time / frameduration;

        return (time % frameduration >= halfduration) ? no + 1 : no ;
}

/* Return a frame at a given time always DOWN ROUNDING */
gint32                          gdv_fraction_fps_frame_at_time (GdvFraction *this, GdvTime time)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        GdvTime frameduration = gdv_fraction_fps_frame_duration (this);
        return time / frameduration;
}

/* Normalize a time to the nearest frame time using rounding */
GdvTime                         gdv_fraction_fps_normalize_time_rnd (GdvFraction *this, GdvTime time)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        gint32 no = gdv_fraction_fps_frame_at_time_rnd (this, time);
        return no * gdv_fraction_fps_frame_duration (this);
}

/* Normalize a time to the nearest frame always DOWN ROUNDING  */
GdvTime                         gdv_fraction_fps_normalize_time (GdvFraction *this, GdvTime time)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        gint32 no = gdv_fraction_fps_frame_at_time (this, time);
        return no * gdv_fraction_fps_frame_duration (this);
}

/* Get the digit (integer) value of the frames per second. For SMPTE functions etc. 
 * The value is rounded up if it's not integer */
gint32                          gdv_fraction_fps_digitize (GdvFraction *this)
{
        g_return_val_if_fail (this != NULL, 0);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (this), 0);

        if (! GDV_FRACTION_IS_SIMPLE (this))
                gdv_fraction_simplify (this);

        if (this->Denominator == 1)
                return this->Numerator;
        else {
                gint32 v = this->Numerator / this->Denominator;
                gint32 mo = this->Numerator / this->Denominator;
                if (mo != 0)
                        v++;
                
                return v;
        }
}

void                            gdv_fraction_fps_set_gst (const GdvFraction *this, GstStructure *struc)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (struc != NULL);
        g_return_if_fail (GDV_FRACTION_IS_EMPTY (this) == FALSE);

        gst_structure_set (struc, "framerate", GST_TYPE_FRACTION,
                           this->Numerator, this->Denominator, NULL);
        
}

/* Translate frame dimensions in one aspect (f1) into another aspect (f2) */
GdvFrameDimensions*             gdv_fraction_aspect_pixelize (GdvFraction *f1, GdvFraction *f2,
                                                              GdvFrameDimensions *dimensions)
{
        g_return_val_if_fail (f1 != NULL && f2 != NULL && dimensions != NULL, NULL);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (f1), NULL);
        g_return_val_if_fail (! GDV_FRACTION_IS_EMPTY (f2), NULL);
        // FIXME: Check if dimensions not empty

        // Calculate our final ratio
        GdvFraction *multiplier = gdv_fraction_new_empty ();
        GDV_FRACTION_SET_NUM (multiplier, f1->Numerator * f2->Numerator);
        GDV_FRACTION_SET_DEN (multiplier, f1->Denominator * f2->Denominator);
        gdv_fraction_simplify (multiplier);

        GdvFrameDimensions *new = gdv_framedimensions_new (multiplier->Numerator * dimensions->Width,
                                                           multiplier->Denominator * dimensions->Height);
        return new;
}

GdvFraction*                    gdv_fraction_aspect_new_from_frame (GdvFrameDimensions *dimensions)
{
        return gdv_fraction_new (dimensions->Width, dimensions->Height);
}

GdvFraction*                    gdv_fraction_aspect_new_from_gst (GstStructure *struc)
{
        g_return_val_if_fail (struc != NULL, NULL);

        gint Num, Den;
        
        if (gst_structure_has_field_typed (struc, "pixel-aspect-ratio", GST_TYPE_FRACTION) == FALSE) {
                Num = 1;
                Den = 1;
        } else
                gst_structure_get_fraction (struc, "pixel-aspect-ratio", &Num, &Den);

                
        return gdv_fraction_new (Num, Den);
}

void                            gdv_fraction_aspect_set_gst (const GdvFraction *this, GstStructure *struc)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (struc != NULL);
        g_return_if_fail (GDV_FRACTION_IS_EMPTY (this) == FALSE);

        gst_structure_set (struc, "pixel-aspect-ratio", GST_TYPE_FRACTION,
                           this->Numerator, this->Denominator, NULL);
}

/* Calculate the gcd (greatest common divisor) for a and b.
 * With kind regards to Terry R. McConnell */
static gint32                   calculate_gcd (gint32 a, gint32 b, gint32 *c, gint32 *d)
{
	int q, r, t, C, D, rval;
	int sgn_a = 1, sgn_b = 1, swap = 0;

	/* Normalize so that 0 < a <= b */
	if ((a == 0) || (b == 0))
                return -1;
	if (a < 0) {
	       	a = -a;
		sgn_a = -1;
	}
	if (b < 0) {
	       	b = -b;
		sgn_b = -1;
	}
	if (b < a) {
		t = b;
		b = a;
		a = t;
		swap = 1;
	}

	/* Now a <= b and both >= 1. */

	q = b / a;
	r = b - a*q;
	if (r == 0) {
                
		if (swap) {
			*d = 1;
			*c = 0;
		} else {
			*c = 1;
			*d = 0;
		}
                
		*c = sgn_a*(*c);
		*d = sgn_b*(*d);
                return a;
	}
        
	rval =  calculate_gcd (a, r, &C, &D);
        
	if (swap) {
		*d = (C-D*q);
		*c = D;
	} else {
		*d = D;
		*c = (C-D*q);
	}
        
	*c = sgn_a * (*c);
	*d = sgn_b * (*d);
	return rval;
}
