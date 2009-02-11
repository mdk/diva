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

#ifndef __GDVFRAMEDIMENSIONS_H__
#define __GDVFRAMEDIMENSIONS_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvFraction.h"

struct _GdvFrameDimensions
{
        gint32 Width;  // Width in pixels
        gint32 Height; // Height in pixels
};

GType                           gdv_framedimensions_get_type (void);

GdvFrameDimensions*             gdv_framedimensions_copy (const GdvFrameDimensions *this);

void                            gdv_framedimensions_free (GdvFrameDimensions *this);

GdvFrameDimensions*             gdv_framedimensions_new (gint32 width, gint32 height);

void                            gdv_framedimensions_fit_h (const GdvFrameDimensions *this, gint32 height,
                                                           GdvFrameDimensions *out);

void                            gdv_framedimensions_fit_w (const GdvFrameDimensions *this, gint32 width,
                                                           GdvFrameDimensions *out);

gint                            gdv_framedimensions_compare (const GdvFrameDimensions *a,
                                                             const GdvFrameDimensions *b);

gchar*                          gdv_framedimensions_to_string (const GdvFrameDimensions *this);

void                            gdv_framedimensions_roundify (GdvFrameDimensions *this, int denomx, int denomy);

GdvFrameDimensions*             gdv_framedimensions_from_caps (const GstCaps *caps);  

#endif                        
