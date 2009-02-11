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

#ifndef __GDVTIMESPAN_H__
#define __GDVTIMESPAN_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvTime.h"

struct _GdvTimeSpan
{
        GdvTime Start; // in point
        GdvTime End;   // out point
};

GType                           gdv_timespan_get_type (void);

GdvTimeSpan*                    gdv_timespan_copy (const GdvTimeSpan *this);

void                            gdv_timespan_free (GdvTimeSpan *this);

GdvTimeSpan*                    gdv_timespan_new (GdvTime start, GdvTime end);

GdvTimeSpan*                    gdv_timespan_new_empty (void);

GdvTime                         gdv_timespan_duration (const GdvTimeSpan *this);

gboolean                        gdv_timespan_is_empty (const GdvTimeSpan *this);

gboolean                        gdv_timespan_contains (const GdvTimeSpan *this, GdvTime time);

gboolean                        gdv_timespan_contains_math (const GdvTimeSpan *this, GdvTime time);

void                            gdv_timespan_intersect (const GdvTimeSpan *this, const GdvTimeSpan *that,
                                                        GdvTimeSpan *out);

gboolean                        gdv_timespan_intersects_with (const GdvTimeSpan *this, const GdvTimeSpan *that);

void                            gdv_timespan_union (const GdvTimeSpan *this, const GdvTimeSpan *that,
                                                    GdvTimeSpan *out);

gboolean                        gdv_timespan_is_adjacent (const GdvTimeSpan *this, const GdvTimeSpan *that);

gboolean                        gdv_timespan_is_equal (const GdvTimeSpan *this, const GdvTimeSpan *other);

void                            gdv_timespan_move_to (GdvTimeSpan *this, GdvTime time);

void                            gdv_timespan_cut_left (GdvTimeSpan *this, GdvTime time);

void                            gdv_timespan_cut_right (GdvTimeSpan *this, GdvTime time);

void                            gdv_timespan_halve (const GdvTimeSpan *this, GdvTimeSpan *part1, 
                                                    GdvTimeSpan *part2);

gchar*                          gdv_timespan_to_string (const GdvTimeSpan *timespan);

void                            gdv_timespan_add_offset (GdvTimeSpan *this, GdvTime offset);

#endif                        
