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

/* Functions that draw various simple primitives */

#include "DivaCairoDraw.h"

/* Draw a line from (x0,y0) to (x1,y1). Color specified by Color. If correct == true
 * than special antia-aliasing correction is applied (+0.5 coords) */
void                            diva_cairo_draw_line (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                      gdouble x1, gdouble y1, GdvColor *color, 
                                                      gdouble lw, gboolean correct)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));

        // To make 1px lines crisp
        if (correct == TRUE) {
                x0 += 0.5;
                y0 += 0.5;
                x1 += 0.5;
                y1 += 0.5;
        }
        
        cairo_save (this->Cairo);
        
        cairo_move_to (this->Cairo, x0, y0);
        cairo_line_to (this->Cairo, x1, y1);
        
        cairo_set_source_rgba (this->Cairo, color->Red, color->Green, color->Blue, color->Alpha);
        cairo_set_line_width (this->Cairo, lw);

        cairo_stroke (this->Cairo);
        
        cairo_restore (this->Cairo);
}

/* Draw a rounded rectangle with a given pixbuf */
void                            diva_cairo_draw_rounded_pixbuf (DivaCairoGraphics *this, 
                                                                gdouble x0, gdouble y0,
                                                                gdouble width, gdouble height,
                                                                gdouble radius, GdkPixbuf *pixbuf,
                                                                DivaCairoCut cut)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (pixbuf != NULL);

        cairo_save (this->Cairo);
        gdk_cairo_set_source_pixbuf (this->Cairo, pixbuf, x0, y0);

        // Texture
        diva_cairo_sketch_rounded_rect (this, x0, y0, width, height, radius, cut);
        cairo_fill (this->Cairo);
                
        cairo_restore (this->Cairo);
}

/* Draw a rounded rectangle filled with a solid color */
void                            diva_cairo_draw_rounded_solid (DivaCairoGraphics *this, 
                                                               gdouble x0, gdouble y0,
                                                               gdouble width, gdouble height,
                                                               gdouble radius, GdvColor *color,
                                                               DivaCairoCut cut)
{
        g_return_if_fail (color != NULL);
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));

        cairo_save (this->Cairo);
        diva_cairo_graphics_set_color (this, color);

        // Texture
        diva_cairo_sketch_rounded_rect (this, x0, y0, width, height, radius, cut);
        cairo_fill (this->Cairo);

        cairo_restore (this->Cairo);
}

/* Draw a centered text (at given x/y) with a given size, spacing, etc. */
void                            diva_cairo_draw_centered_text (DivaCairoGraphics *this, 
                                                               gdouble x, gdouble y,
                                                               gdouble size, gboolean bold, 
                                                               gchar *face, gchar *text)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (text != NULL);
        g_return_if_fail (face != NULL);
        g_return_if_fail (size > 0);
        
        cairo_save (this->Cairo);

        cairo_select_font_face (this->Cairo,
                                face,
                                CAIRO_FONT_SLANT_NORMAL,
                                (bold != TRUE) ? CAIRO_FONT_WEIGHT_NORMAL : CAIRO_FONT_WEIGHT_BOLD);
        cairo_set_font_size (this->Cairo, size);

        cairo_text_extents_t *extents = g_new (cairo_text_extents_t, 1);
        cairo_text_extents (this->Cairo, text, extents);
        
        cairo_move_to (this->Cairo,
                       x - (extents->width / 2) + extents->x_bearing,
                       y - (extents->height / 2) - extents->y_bearing);

        cairo_show_text (this->Cairo, text);

        cairo_restore (this->Cairo);
}

/* Draw a rounded, stroked rectangle filled with a gradient color */
void                            diva_cairo_draw_rounded_gradient (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                                  gdouble width, gdouble height,
                                                                  gdouble radius,
                                                                  GdvColor *color1, GdvColor *color2,
                                                                  DivaCairoCut cut)
{
        g_return_if_fail (color1 != NULL);
        g_return_if_fail (color2 != NULL);
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        
        cairo_save (this->Cairo);
        
        // Get the pattern
        cairo_pattern_t *gradient_pattern = diva_cairo_sketch_get_linear_pattern2c (this, x0, y0,
                                                                                    x0, y0 + height,
                                                                                    color1, color2);
        // Main rect
        diva_cairo_sketch_rounded_rect (this, x0, y0, width, height, radius, cut);
        cairo_set_source (this->Cairo, gradient_pattern);
        cairo_fill (this->Cairo);
        cairo_pattern_destroy (gradient_pattern);
        
        cairo_restore (this->Cairo);
}

/* Draw a rounded outline. The outline can be black and/or white (tango-style */
void                            diva_cairo_draw_rounded_outline (DivaCairoGraphics *this, 
                                                                 gdouble x0, gdouble y0,
                                                                 gdouble width, gdouble height,
                                                                 gdouble radius, DivaCairoOutline outline,
                                                                 DivaCairoCut cut)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        
        if (outline == DIVA_CAIRO_OUTLINE_NONE)
                return;
        
        cairo_save (this->Cairo);
        
        if (outline & DIVA_CAIRO_OUTLINE_BLACK) {
                // Black line
                cairo_set_line_width (this->Cairo, 1.0);
                diva_cairo_sketch_rounded_rect_out (this, x0 + 0.5, y0 + 0.5,
                                                    width - 1, height - 1, radius * 0.9, cut);
                cairo_set_source_rgba (this->Cairo, 0, 0, 0, 0.8);
                cairo_stroke (this->Cairo);
        }
        
        if (outline & DIVA_CAIRO_OUTLINE_WHITE) {
                // White outline
                diva_cairo_sketch_rounded_rect_out (this, x0 + 1.5, y0 + 1.5,
                                                    width - 3, height - 3, radius * 0.8, cut);
                
                cairo_pattern_t *pattern = cairo_pattern_create_linear (x0, y0, x0, y0 + height);
                g_assert (pattern != NULL);
        
                cairo_pattern_add_color_stop_rgba (pattern, 0, 1, 1, 1, 0.2);
                cairo_pattern_add_color_stop_rgba (pattern, 1, 1, 1, 1, 0.55);
                cairo_set_source (this->Cairo, pattern);
                cairo_set_line_width (this->Cairo, 1.0);
                cairo_stroke (this->Cairo);
                cairo_pattern_destroy (pattern);
        }

        cairo_restore (this->Cairo);
}

/* Draw a rounded rectangle filled with a solid color */
void                            diva_cairo_draw_solid_rect (DivaCairoGraphics *this, 
                                                            gdouble x0, gdouble y0,
                                                            gdouble width, gdouble height,
                                                            GdvColor *color)
{
        g_return_if_fail (color != NULL);
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));

        // Save state
        cairo_save (this->Cairo);
        
        // Shape
        cairo_move_to (this->Cairo, x0, y0);
        cairo_line_to (this->Cairo, x0 + width, y0);
        cairo_line_to (this->Cairo, x0 + width, y0 + height);
        cairo_line_to (this->Cairo, x0, y0 + height);
        cairo_line_to (this->Cairo, x0, y0);
        cairo_close_path (this->Cairo);

        // Color
        diva_cairo_graphics_set_color (this, color);

        // Draw
        cairo_fill (this->Cairo);
        
        // Cleanup
        cairo_restore (this->Cairo);
}
