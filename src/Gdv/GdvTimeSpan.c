///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//      Senko Rasic <senko@senko.net>                                        //
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

#include "GdvTimeSpan.h"

/* GBoxed type */
GType                           gdv_timespan_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype)                
                objecttype = g_boxed_type_register_static 
                        ("GdvTimeSpan", 
                         (GBoxedCopyFunc) gdv_timespan_copy,
                         (GBoxedFreeFunc) gdv_timespan_free);
        
        return objecttype;
}

/* GBoxed copy */
GdvTimeSpan*                    gdv_timespan_copy (const GdvTimeSpan *this)
{
        g_return_val_if_fail (this != NULL, NULL);
        
        return gdv_timespan_new (this->Start, this->End);
}

/* GBoxed free */
void                            gdv_timespan_free (GdvTimeSpan *this)
{
        g_return_if_fail (this != NULL);
        g_free (this);
}

/* Create a new timespan. Values are absolute */
GdvTimeSpan*                    gdv_timespan_new (GdvTime start, GdvTime end)
{
        GdvTimeSpan *this = g_new (GdvTimeSpan, 1);
        this->Start = start;
        this->End = end;
        return this;
}

/* Create a new empty. For convinience */
GdvTimeSpan*                    gdv_timespan_new_empty (void)
{
        return gdv_timespan_new (0, 0);
}

/* The duration (length) of the timespan */
GdvTime                         gdv_timespan_duration (const GdvTimeSpan *this)
{
        g_return_val_if_fail (this != NULL, 0);
        if (this->End <= this->Start)
                return 0;
        
        return this->End - this->Start;
}

/* Check if it contains ANY time */
gboolean                        gdv_timespan_is_empty (const GdvTimeSpan *this)
{
        g_return_val_if_fail (this != NULL, TRUE);

        if (this->Start >= this->End)
                return TRUE;
        else
                return FALSE;
}

/* If a given timestamp contains a given time */
gboolean                        gdv_timespan_contains (const GdvTimeSpan *this, GdvTime time)
{
        g_return_val_if_fail (this != NULL, FALSE);

        return gdv_time_inspan (time, this->Start, this->End);
}

/* If a given timestamp contains a given time. Uses a true math algho. */
gboolean                        gdv_timespan_contains_math (const GdvTimeSpan *this, GdvTime time)
{
        g_return_val_if_fail (this != NULL, FALSE);

        if (time >= this->Start && time <= this->End)
                return TRUE;
        else
                return FALSE;
}

/* Return the common (overlapping) part of two timespans */
void                            gdv_timespan_intersect (const GdvTimeSpan *this, const GdvTimeSpan *that,
                                                        GdvTimeSpan *out)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (that != NULL);
        g_return_if_fail (out != NULL);
        
        if (!gdv_timespan_intersects_with (this, that)) {
                out = gdv_timespan_new_empty ();
                return;
        }

        out->Start = MAX (this->Start, that->Start);
        out->End = MIN (this->End, that->End);
}

/* If a given timespan intersects with another one */
gboolean                        gdv_timespan_intersects_with (const GdvTimeSpan *this, const GdvTimeSpan *that)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (that != NULL, FALSE);
        
        if (gdv_timespan_is_empty (this) ||
            gdv_timespan_is_empty (that))
                return FALSE;
        
        // either the start of the second interval is contained in the first ...
        if (that->Start >= this->Start &&
            that->Start < this->End)
                return TRUE;

        // ... or the start of the first interval is contained in the second
        if (this->Start >= that->Start &&
            this->Start < that->End)
                return TRUE;
        
        return FALSE;
}

/* Check if the given timespan is adjacent to the other one */
gboolean                        gdv_timespan_is_adjacent (const GdvTimeSpan *this, 
                                                          const GdvTimeSpan *that)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (that != NULL, FALSE);
                
        if (gdv_timespan_is_empty (this) ||
            gdv_timespan_is_empty (that))
                return FALSE;
        
        // If the interval borders touch, they're adjacent
        if (this->End == that->Start ||
            this->Start == that->End)
                return TRUE;
        
        return FALSE;
}

/* Make a union of two timespans */
void                    gdv_timespan_union (const GdvTimeSpan *this, const GdvTimeSpan *that, GdvTimeSpan *out)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (that != NULL);
        g_return_if_fail (out != NULL);
        
        // FIXME: what if they're *not* adjacent/intersecting?
        // What kind of error should we report?
        if (! gdv_timespan_is_adjacent (this, that) ||
            ! gdv_timespan_intersects_with (this, that)) {
                GDV_WARNING_EXT ("Can't union non-adjacent, non-intersecting timespans! (%s and %s)",
                                 gdv_timespan_to_string (this), gdv_timespan_to_string (that));
                return;
        }

        out->Start = MIN (this->Start, that->Start);
        out->End = MAX (this->End, that->End);
}

/* Shift the timespan to a given time */
void                            gdv_timespan_move_to (GdvTimeSpan *this, GdvTime time)
{
        g_return_if_fail (this != NULL);
        
        GdvTime duration = gdv_timespan_duration (this);
        this->Start = time;
        this->End = time + duration;
}

/* Cut X time units from the left (beg) of the span */
void                            gdv_timespan_cut_left (GdvTimeSpan *this, GdvTime time)
{
        g_return_if_fail (this != NULL);
        this->Start = MIN (this->Start + time, this->End);
}

/* Cut X time units from the right (end) of the span */
void                            gdv_timespan_cut_right (GdvTimeSpan *this, GdvTime time)
{
        g_return_if_fail (this != NULL);
        this->End = MAX (this->End - time, this->Start);
}

/* Split the given timespan into two equal halves 
 * (BTW, "equal halve" == absurd */
void                            gdv_timespan_halve (const GdvTimeSpan *this, GdvTimeSpan *part1, 
                                                    GdvTimeSpan *part2)
{
        g_return_if_fail (this != NULL);
        
        GdvTime h = (this->Start + this->End) / 2;
        
        part1->Start = this->Start;
        part1->End = h;
        
        part2->Start = h;
        part2->End = this->End;
}

/* Check if two timespans are equal */
gboolean                        gdv_timespan_is_equal (const GdvTimeSpan *this, const GdvTimeSpan *other)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (other != NULL, FALSE);
        
        if (this->Start == other->Start &&
            this->End == other->End)
                return TRUE;
        else
                return FALSE;
}

/* Produce a string describing the TimeSpan. This is for debugging purposes only. Managed
 * code should use own functions */
gchar*                          gdv_timespan_to_string (const GdvTimeSpan *timespan)
{
        if (timespan == NULL)
                return "(NULL)";
        else return g_strdup_printf ("%s-%s", gdv_time_to_string (timespan->Start), 
                                     gdv_time_to_string (timespan->End));
}

/* Add an offset. This shifts the TimeSpan */
void                            gdv_timespan_add_offset (GdvTimeSpan *this, GdvTime offset)
{
        g_return_if_fail (this != NULL);
        this->Start += offset;
        this->End += offset;
}
