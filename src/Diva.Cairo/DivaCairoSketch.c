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

#include "DivaCairoSketch.h"

/* Sketch a rounded-rectangle shape. Radius is the radius of the rounded-corners */
void                            diva_cairo_sketch_rounded_rect (DivaCairoGraphics *this, 
                                                                gdouble x0, gdouble y0,
                                                                gdouble width, gdouble height,
                                                                gdouble radius, DivaCairoCut cut)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        
        gdouble x1,y1;
        
        x1 = x0 + width;
        y1 = y0 + height;
        
        if (width / 2 < radius)
                radius = width / 2;
        if (height / 2 < radius)
                radius = height / 2;
        
        cairo_move_to (this->Cairo, x0, y0 + radius);
        
        if (cut & DIVA_CAIRO_CUT_LEFT) {
                cairo_line_to (this->Cairo, x0, y0);
                cairo_line_to (this->Cairo, x0 + radius, y0);
        } else 
                cairo_curve_to (this->Cairo, x0 , y0 + (radius / 2), x0 + (radius / 2), y0, x0 + radius, y0);
        
        cairo_line_to (this->Cairo, x1 - radius, y0);
        
        if (cut & DIVA_CAIRO_CUT_RIGHT) {
                cairo_line_to (this->Cairo, x1, y0);
                cairo_line_to (this->Cairo, x1, y0 + radius);
        } else 
                cairo_curve_to (this->Cairo, x1 - (radius /2), y0, x1, y0 + (radius / 2), x1, y0 + radius);
        
        cairo_line_to (this->Cairo, x1, y1 - radius);

        if (cut & DIVA_CAIRO_CUT_RIGHT) {
                cairo_line_to (this->Cairo, x1, y1);
                cairo_line_to (this->Cairo, x1 - radius, y1);
        } else 
                cairo_curve_to (this->Cairo, x1, y1 - (radius / 2), x1 - (radius / 2), y1, x1 - radius, y1);
                        
        cairo_line_to (this->Cairo, x0 + radius, y1);
        
        if (cut & DIVA_CAIRO_CUT_LEFT) { 
                cairo_line_to (this->Cairo, x0, y1);
                cairo_line_to (this->Cairo, x0, y1 - radius);
        } else
                cairo_curve_to (this->Cairo, x0 + (radius / 2), y1, x0, y1 - (radius / 2), x0, y1 - radius);
                
        cairo_close_path (this->Cairo);
}

/* Sketch a "shield" figure used to draw the current time marker */
void                            diva_cairo_sketch_shield (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                          gdouble size)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (size >= 0);
        
        size = size / 2;
        
        cairo_move_to (this->Cairo, x0 - size, y0 - size);
        cairo_line_to (this->Cairo, x0 + size, y0 - size);
        cairo_curve_to (this->Cairo, x0 + size, y0 - size, x0 + size, y0 + size, x0, y0 + size);
        cairo_curve_to (this->Cairo, x0 - size, y0 + size, x0 - size, y0 - size, x0 - size, y0 - size);
        cairo_close_path (this->Cairo);
}

/* Create a linear, gradiented pattern out of given two colors 
 * The pattern is NOT set as the color and you need to destroy it
 * yourself */
cairo_pattern_t*                diva_cairo_sketch_get_linear_pattern2c (DivaCairoGraphics *this, 
                                                                        gdouble x0, gdouble y0, 
                                                                        gdouble x1, gdouble y1,
                                                                        GdvColor *color1, GdvColor *color2)
{
        g_return_val_if_fail (DIVA_CAIRO_IS_GRAPHICS (this), NULL);
        g_return_val_if_fail (color1 != NULL, NULL);
        g_return_val_if_fail (color2 != NULL, NULL);

        cairo_pattern_t *pattern = cairo_pattern_create_linear (x0, y0, x1, y1);
        g_assert (pattern != NULL);
        
        if (color1->Alpha != 1.0)
                cairo_pattern_add_color_stop_rgba (pattern, 0, 
                                                   color1->Red, 
                                                   color1->Green, 
                                                   color1->Blue, 
                                                   color1->Alpha);
        else
                cairo_pattern_add_color_stop_rgb (pattern, 0, 
                                                  color1->Red, 
                                                  color1->Green,
                                                  color1->Blue);
        
        if (color2->Alpha != 1.0)
                cairo_pattern_add_color_stop_rgba (pattern, 1,
                                                   color2->Red, 
                                                   color2->Green, 
                                                   color2->Blue, 
                                                   color2->Alpha);
        else
                cairo_pattern_add_color_stop_rgb (pattern, 1, 
                                                  color2->Red, 
                                                  color2->Green,
                                                  color2->Blue);
        
        return pattern;
}

void                            diva_cairo_sketch_rounded_rect_out (DivaCairoGraphics *this,
                                                                    gdouble x0, gdouble y0,
                                                                    gdouble width, gdouble height,
                                                                    gdouble radius, DivaCairoCut cut)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        
        gdouble x1,y1;
        
        x1 = x0 + width;
        y1 = y0 + height;
        
        if (width / 2 < radius)
                radius = width / 2;
        if (height / 2 < radius)
                radius = height / 2;
        
        if ((cut & DIVA_CAIRO_CUT_LEFT) && (cut & DIVA_CAIRO_CUT_RIGHT)) {

                cairo_move_to (this->Cairo, x0, y0);
                cairo_line_to (this->Cairo, x1, y0);
                
                cairo_move_to (this->Cairo, x1, y1);
                cairo_line_to (this->Cairo, x0, y1);

        } else if (cut & DIVA_CAIRO_CUT_LEFT) {
                
                cairo_move_to (this->Cairo, x0, y0);
                cairo_line_to (this->Cairo, x1 - radius, y0);
                
                cairo_curve_to (this->Cairo, x1 - (radius /2), y0, x1, y0 + (radius / 2), x1, y0 + radius);
                cairo_line_to (this->Cairo, x1, y1 - radius);
                
                cairo_curve_to (this->Cairo, x1, y1 - (radius / 2), x1 - (radius / 2), y1, x1 - radius, y1);
                cairo_line_to (this->Cairo, x0, y1);
                
        } else if (cut & DIVA_CAIRO_CUT_RIGHT) {
                
                cairo_move_to (this->Cairo, x1, y1);
                cairo_line_to (this->Cairo, x0 + radius, y1);
                cairo_curve_to (this->Cairo, x0 + (radius / 2), y1, x0, y1 - (radius / 2), x0, y1 - radius);
                
                cairo_line_to (this->Cairo, x0, y0 + radius);
                cairo_curve_to (this->Cairo, x0 , y0 + (radius / 2), x0 + (radius / 2), y0, x0 + radius, y0);
                                
                cairo_line_to (this->Cairo, x1, y0);
        } else
                diva_cairo_sketch_rounded_rect (this, x0, y0, width, height, radius, cut);
        
}
