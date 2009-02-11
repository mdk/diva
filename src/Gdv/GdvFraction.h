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

#ifndef __GDVFRACTION_H__
#define __GDVFRACTION_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvFrameDimensions.h"

struct _GdvFraction {
        gint32 Numerator;       /* Numerator of the fraction */
        gint32 Denominator;     /* Denominator of the fraction */
        gboolean Simple;        /* Is the fraction simplified already */
};

GType                           gdv_fraction_get_type (void);

GdvFraction*                    gdv_fraction_copy (const GdvFraction *this);

void                            gdv_fraction_free (GdvFraction *this);

gboolean                        gdv_fraction_is_empty (const GdvFraction *this);

GdvFraction*                    gdv_fraction_new (gint32 num, gint32 den);

GdvFraction*                    gdv_fraction_new_empty (void);

gdouble                         gdv_fraction_to_double (const GdvFraction *this);

gboolean                        gdv_fraction_simplify (GdvFraction *this);

gint32                          gdv_fraction_compare (GdvFraction *this, GdvFraction *that);

GdvFraction*                    gdv_fraction_invert (const GdvFraction *this);

gchar*                          gdv_fraction_to_string (GdvFraction *this);

/* Fps */

GdvTime                         gdv_fraction_fps_frame_duration (GdvFraction *this);

GdvTime                         gdv_fraction_fps_time_at_frame (GdvFraction *this, gint32 no);

gint32                          gdv_fraction_fps_frame_at_time_rnd (GdvFraction *this, GdvTime time);

gint32                          gdv_fraction_fps_frame_at_time (GdvFraction *this, GdvTime time);

GdvTime                         gdv_fraction_fps_normalize_time_rnd (GdvFraction *this, GdvTime time);

GdvTime                         gdv_fraction_fps_normalize_time (GdvFraction *this, GdvTime time);

void                            gdv_fraction_fps_set_gst (const GdvFraction *this, GstStructure *struc);

gint32                          gdv_fraction_fps_digitize (GdvFraction *this);

/* Aspect */

GdvFrameDimensions*             gdv_fraction_aspect_pixelize (GdvFraction *f1, GdvFraction *f2,
                                                              GdvFrameDimensions *dimensions);
        
GdvFraction*                    gdv_fraction_aspect_new_from_gst (GstStructure *struc);

GdvFraction*                    gdv_fraction_aspect_new_from_frame (GdvFrameDimensions *dimensions);

void                            gdv_fraction_aspect_set_gst (const GdvFraction *this, GstStructure *struc);

/* Macros */

#define                         GDV_FRACTION_IS_SIMPLE(obj) (obj->Simple)

#define                         GDV_FRACTION_IS_EMPTY(obj) (gdv_fraction_is_empty (obj))

#define                         GDV_FRACTION_SET_NUM(obj,val) (obj->Numerator = (val)) ; (obj->Simple = FALSE)

#define                         GDV_FRACTION_SET_DEN(obj,val) (obj->Denominator = (val)) ; (obj->Simple = FALSE)


        
#endif
