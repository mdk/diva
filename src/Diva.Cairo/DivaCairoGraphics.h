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

#ifndef __DIVA_CAIRO_GRAPHICS_H__
#define __DIVA_CAIRO_GRAPHICS_H__

#include <gdk/gdk.h>
#include <gtk/gtk.h>
#include <cairo.h>
#include "DivaCairoTypes.h"
#include <gdv.h>

struct                          _DivaCairoGraphics 
{
        GtkStyle *Style;
        GdkWindow *Window;
        PangoContext *Context;
        cairo_t *Cairo;
};
        
DivaCairoGraphics*              diva_cairo_graphics_new (GdkWindow *window, GtkStyle *style,
                                                         PangoContext *context);

DivaCairoGraphics*              diva_cairo_graphics_new_limited (GdkWindow *window);

DivaCairoGraphics*              diva_cairo_graphics_new_limited_context (PangoContext *context);

void                            diva_cairo_graphics_dispose (DivaCairoGraphics *this);

void                            diva_cairo_graphics_finalize (DivaCairoGraphics *this);

void                            diva_cairo_graphics_set_color (DivaCairoGraphics *this, GdvColor *color);

GtkStyle*                       diva_cairo_graphics_get_style (DivaCairoGraphics *this);

GdkWindow*                      diva_cairo_graphics_get_window (DivaCairoGraphics *this);

PangoContext*                   diva_cairo_graphics_get_context (DivaCairoGraphics *this);

#endif
