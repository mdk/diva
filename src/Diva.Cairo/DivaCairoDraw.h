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

#ifndef __DIVA_CAIRO_DRAW_H__
#define __DIVA_CAIRO_DRAW_H__

#include <gdk/gdk.h>
#include <cairo.h>
#include <gdv.h>
#include "DivaCairoTypes.h"
#include "DivaCairoGraphics.h"
#include "DivaCairoSketch.h"

void                            diva_cairo_draw_line (DivaCairoGraphics *this, 
                                                      gdouble x0, gdouble y0,
                                                      gdouble x1, gdouble y1, GdvColor *color, 
                                                      gdouble lw, gboolean correct);

void                            diva_cairo_draw_centered_text (DivaCairoGraphics *this, 
                                                               gdouble x, gdouble y,
                                                               gdouble size, gboolean bold,
                                                               gchar *face, gchar *text);

void                            diva_cairo_draw_rounded_pixbuf (DivaCairoGraphics *this, 
                                                                gdouble x0, gdouble y0,
                                                                gdouble width, gdouble height,
                                                                gdouble radius, GdkPixbuf *pixbuf,
                                                                DivaCairoCut cut);

void                            diva_cairo_draw_rounded_solid (DivaCairoGraphics *this,
                                                               gdouble x0, gdouble y0,
                                                               gdouble width, gdouble height,
                                                               gdouble radius, GdvColor *color,
                                                               DivaCairoCut cut);

void                            diva_cairo_draw_rounded_gradient (DivaCairoGraphics *this, 
                                                                  gdouble x0, gdouble y0,
                                                                  gdouble width, gdouble height,
                                                                  gdouble radius,
                                                                  GdvColor *color1, GdvColor *color2,
                                                                  DivaCairoCut cut);

void                            diva_cairo_draw_rounded_outline (DivaCairoGraphics *this, 
                                                                 gdouble x0, gdouble y0,
                                                                 gdouble width, gdouble height,
                                                                 gdouble radius, DivaCairoOutline outline,
                                                                 DivaCairoCut cut);

void                            diva_cairo_draw_solid_rect (DivaCairoGraphics *this, 
                                                            gdouble x0, gdouble y0,
                                                            gdouble width, gdouble height,
                                                            GdvColor *color);

#endif
