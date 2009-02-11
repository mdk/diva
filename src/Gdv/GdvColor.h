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

#ifndef __GDVCOLOR_H__
#define __GDVCOLOR_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include <gdk/gdk.h>

/* Small set of basic colors */
enum                            _GdvColorConst
{ 
        GDV_COLOR_BLACK,
        GDV_COLOR_WHITE,
        GDV_COLOR_RED,
        GDV_COLOR_GREEN,
        GDV_COLOR_BLUE
};

/* A set of Tango colors */
enum                            _GdvColorTango
{
        GDV_COLOR_BUTTER1, GDV_COLOR_BUTTER2, GDV_COLOR_BUTTER3, 
        GDV_COLOR_CHAMELEON1, GDV_COLOR_CHAMELEON2, GDV_COLOR_CHAMELEON3,
        GDV_COLOR_ORANGE1, GDV_COLOR_ORANGE2, GDV_COLOR_ORANGE3,
        GDV_COLOR_SKYBLUE1, GDV_COLOR_SKYBLUE2, GDV_COLOR_SKYBLUE3, 
        GDV_COLOR_PLUM1, GDV_COLOR_PLUM2, GDV_COLOR_PLUM3,
        GDV_COLOR_CHOCOLATE1, GDV_COLOR_CHOCOLATE2, GDV_COLOR_CHOCOLATE3,
        GDV_COLOR_SCARLETRED1, GDV_COLOR_SCARLETRED2, GDV_COLOR_SCARLETRED3,
        GDV_COLOR_ALUMINIUM1, GDV_COLOR_ALUMINIUM2, GDV_COLOR_ALUMINIUM3,
        GDV_COLOR_ALUMINIUM4, GDV_COLOR_ALUMINIUM5, GDV_COLOR_ALUMINIUM6
};

struct                          _GdvColor
{
        gfloat Red;   // The red component 
        gfloat Green; // The blue  component 
        gfloat Blue;  // The green component 
        gfloat Alpha; // The alpha component
};

GType                           gdv_color_get_type (void);

GdvColor*                       gdv_color_copy (const GdvColor *this);

void                            gdv_color_free (GdvColor *this);

gboolean                        gdv_color_is_empty (GdvColor *this);

GdvColor*                       gdv_color_empty (void);

void                            gdv_color_clamp (GdvColor *this);

GdvColor*                       gdv_color_new_rgb_f (gfloat red, gfloat green, gfloat blue);

GdvColor*                       gdv_color_new_rgba_f (gfloat red, gfloat green, gfloat blue, gfloat alpha);

GdvColor*                       gdv_color_new_rgba_8 (guchar red, guchar green, guchar blue, guchar alpha);

GdvColor*                       gdv_color_new_rgb_8 (guchar red, guchar green, guchar blue);

gboolean                        gdv_color_equals (const GdvColor *this, const GdvColor *other);

GdkColor*                       gdv_color_to_gdkcolor (const GdvColor *this);

GdvColor*                       gdv_color_from_gdkcolor (const GdkColor *color);

GdvColor*                       gdv_color_new_const (GdvColorConst val);

GdvColor*                       gdv_color_new_tango (GdvColorTango val);

gchar*                          gdv_color_to_html (const GdvColor *this);

GdvColor*                       gdv_color_highlight (const GdvColor *this);

void                            gdv_color_highlight_this (GdvColor *this);

GdvColor*                       gdv_color_dim (const GdvColor *this);

void                            gdv_color_dim_this (GdvColor *this);

#endif                        
