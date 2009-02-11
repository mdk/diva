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

#ifndef __DIVA_CAIRO_SKETCH_H__
#define __DIVA_CAIRO_SKETCH_H__

#include <gdk/gdk.h>
#include <gdv.h>
#include <cairo.h>
#include "DivaCairoTypes.h"
#include "DivaCairoGraphics.h"

void                            diva_cairo_sketch_rounded_rect (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                                gdouble width, gdouble height,
                                                                gdouble radius, DivaCairoCut cut);

cairo_pattern_t*                diva_cairo_sketch_get_linear_pattern2c (DivaCairoGraphics *this, 
                                                                        gdouble x0, gdouble y0, 
                                                                        gdouble x1, gdouble y1,
                                                                        GdvColor *color1, GdvColor *color2);

void                            diva_cairo_sketch_rounded_rect_out (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                                    gdouble width, gdouble height,
                                                                    gdouble radius, DivaCairoCut cut);

void                            diva_cairo_sketch_shield (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                          gdouble size);

#endif

