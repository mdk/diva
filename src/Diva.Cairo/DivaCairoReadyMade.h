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

#ifndef __DIVA_CAIRO_READYMADE_H__
#define __DIVA_CAIRO_READYMADE_H__

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkpixbuf.h>
#include <cairo.h>
#include <gdv.h>
#include "DivaCairoTypes.h"
#include "DivaCairoGraphics.h"
#include "DivaCairoSketch.h"
#include "DivaCairoDraw.h"
#include "DivaCairoUtil.h"

void                            diva_cairo_readymade_checker_frame (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                                    gdouble width, gdouble height, gint checker);


void                            diva_cairo_readymade_timeline_rect (DivaCairoGraphics *this, const GdkRectangle *rect, 
                                                                    const GdkRectangle *totalrect);

void                            diva_cairo_readymade_lock_rect (DivaCairoGraphics *this, const GdkRectangle *rect);

void                            diva_cairo_readymade_clip (DivaCairoGraphics *this, gdouble x0, gdouble y0, 
                                                           gdouble width, 
                                                           GdvColor *color,
                                                           gchar *markup, GdkPixbuf *pixbuf, 
                                                           DivaCairoCut cut,
                                                           DivaCairoState state);

void                            diva_cairo_readymade_shield (DivaCairoGraphics *this, gdouble x0, gdouble y0, 
                                                             DivaCairoState state);

void                            diva_cairo_readymade_tag (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                          gchar *tag);

void                            diva_cairo_readymade_tag_get_size (DivaCairoGraphics *this,
                                                                   gchar *tag, gint32 *tagwidth, gint32 *tagheight);

void                            diva_cairo_readymade_time_stripe (DivaCairoGraphics *this,
                                                                  int startx, int width, GdvColor *color,
                                                                  const GdkRectangle *exposerect,
                                                                  const GdkRectangle *totalrect);

#endif
