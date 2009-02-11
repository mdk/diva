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

/* Contains funcions to draw "ready mades" -- higher-level graphics elements */

#include "DivaCairoReadyMade.h"

#define                         CLIP_RADIUS 8

#define                         CLIP_BORDER 5

void                            diva_cairo_readymade_checker_frame (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                                    gdouble width, gdouble height, gint checker)
{
        g_return_if_fail (this != NULL);
        g_return_if_fail (width > 0 && height > 0);

        GdkPixbuf *checkers = diva_cairo_util_get_checkers (width + 2, height + 2, checker);
        g_assert (checkers != NULL);
        
        diva_cairo_draw_rounded_pixbuf (this, x0, y0, width, height, 
                                        12, checkers, DIVA_CAIRO_CUT_NONE);
        
        // Colors
        GdvColor *color1 = gdv_color_new_rgba_f (1.0, 1.0, 1.0, 1.0);
        GdvColor *color2 = gdv_color_new_rgba_f (1.0, 1.0, 1.0, 0.0);
        
        diva_cairo_draw_rounded_gradient (this, x0, y0, width, height, 
                                          12, color1, color2, DIVA_CAIRO_CUT_NONE);

        diva_cairo_draw_rounded_outline (this, x0, y0, width, height, 12,
                                         DIVA_CAIRO_OUTLINE_BLACK,
                                         DIVA_CAIRO_CUT_NONE);

        gdv_color_free (color1);
        gdv_color_free (color2);
        gdk_pixbuf_unref (checkers);
}

/* Draw a gradient used as the timeline widget background 
 * FIXME: We should use some generic functions here! */
void                            diva_cairo_readymade_timeline_rect (DivaCairoGraphics *this, const GdkRectangle *rect, 
                                                                    const GdkRectangle *totalrect)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (rect != NULL);
        g_return_if_fail (totalrect != NULL);
        
        // Draw the back using gdk
        gdk_draw_rectangle (GDK_DRAWABLE (this->Window), this->Style->dark_gc [0], TRUE,
                            rect->x, rect->y, 
                            rect->width, rect->height);
        
        // Save state
        cairo_save (this->Cairo);
        
        // Shape
        cairo_move_to (this->Cairo, rect->x, rect->y);
        cairo_line_to (this->Cairo, rect->x + rect->width, rect->y);
        cairo_line_to (this->Cairo, rect->x + rect->width, rect->y + rect->height);
        cairo_line_to (this->Cairo, rect->x, rect->y + rect->height);
        cairo_line_to (this->Cairo, rect->x, rect->y);
        cairo_close_path (this->Cairo);

        // Pattern
        cairo_pattern_t *pattern = cairo_pattern_create_linear (totalrect->x, totalrect->y,
                                                                totalrect->x, totalrect->y + totalrect->height);
        g_assert (pattern != NULL);
        cairo_pattern_add_color_stop_rgba (pattern, 0, 1, 1, 1, 0.75);
        cairo_pattern_add_color_stop_rgba (pattern, 1, 1, 1, 1, 0);
        cairo_set_source (this->Cairo, pattern);
        
        // Draw
        cairo_fill (this->Cairo);
        
        // Cleanup
        cairo_pattern_destroy (pattern);
        cairo_restore (this->Cairo);
}

void                            diva_cairo_readymade_lock_rect (DivaCairoGraphics *this, const GdkRectangle *rect)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (rect != NULL);
        
        // Save state
        cairo_save (this->Cairo);
        
        // Shape
        cairo_move_to (this->Cairo, rect->x, rect->y);
        cairo_line_to (this->Cairo, rect->x + rect->width, rect->y);
        cairo_line_to (this->Cairo, rect->x + rect->width, rect->y + rect->height);
        cairo_line_to (this->Cairo, rect->x, rect->y + rect->height);
        cairo_line_to (this->Cairo, rect->x, rect->y);
        cairo_close_path (this->Cairo);

        // Color
        GdvColor *color = gdv_color_new_rgba_f (0.9, 0.9, 0.9, 0.7);
        diva_cairo_graphics_set_color (this, color);

        // Draw
        cairo_fill (this->Cairo);
        
        // Cleanup
        cairo_restore (this->Cairo);
        gdv_color_free (color);
}

void                            diva_cairo_readymade_clip (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                           gdouble width, 
                                                           GdvColor *color,
                                                           gchar *markup, GdkPixbuf *pixbuf, 
                                                           DivaCairoCut cut, DivaCairoState state)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (markup != NULL);
        
        GdvColor *solid = NULL;
        GdvColor *grad = NULL;
        
        if (state == DIVA_CAIRO_STATE_ACTIVE)
                solid = gdv_color_highlight (color);
        else if (state == DIVA_CAIRO_STATE_DRAG)
                solid = gdv_color_new_rgb_f (1, 1, 1);
        else {
                solid = color;
                grad = gdv_color_dim (color);
        }
                
        if (state == DIVA_CAIRO_STATE_NORMAL)
                diva_cairo_draw_rounded_gradient (this, x0, y0, width, 32, CLIP_RADIUS, solid, grad, cut);
        else
                diva_cairo_draw_rounded_solid (this, x0, y0, width, 32, CLIP_RADIUS, solid, cut);
        
        gint pixelsLeft = width - (CLIP_BORDER * 2); // Two for start, two for end
        gint currentPixel = CLIP_BORDER;
        
        if (pixbuf != NULL && pixelsLeft >= gdk_pixbuf_get_width (pixbuf) &&
            state != DIVA_CAIRO_STATE_DRAG) {
                gint pxwidth = gdk_pixbuf_get_width (pixbuf);
                gint pxheight = gdk_pixbuf_get_height (pixbuf);
                diva_cairo_draw_rounded_pixbuf (this, x0, y0, (gdouble) pxwidth, (gdouble) pxheight, 
                                                CLIP_RADIUS, pixbuf, (cut | DIVA_CAIRO_CUT_RIGHT));
                currentPixel += (pxwidth + 2);
                pixelsLeft -= (pxwidth + 2);
        }
        
        if (pixelsLeft >= 10 && state != DIVA_CAIRO_STATE_DRAG) {
                // Text
                PangoLayout *layout = pango_layout_new (this->Context);
                pango_layout_set_width (layout, pixelsLeft * PANGO_SCALE);
                pango_layout_set_alignment (layout, PANGO_ALIGN_RIGHT);
                pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_START);
                pango_layout_set_markup (layout, markup, -1);
                pango_layout_set_spacing (layout, 0);
                
                gint height;
                pango_layout_get_pixel_size (layout, NULL, &height);
                                
                gdk_draw_layout (this->Window, this->Style->black_gc, 
                                 x0 + currentPixel, y0 + 16 - (height / 2),
                                 layout);
                
                g_object_unref (G_OBJECT (layout));
        }
        
        diva_cairo_draw_rounded_outline (this, x0, y0, width, 32, CLIP_RADIUS, 
                                         DIVA_CAIRO_OUTLINE_WHITE | DIVA_CAIRO_OUTLINE_BLACK,
                                         cut);
        
        if (solid != color)
                gdv_color_free (solid);
}

/* Draw a shield */
void                            diva_cairo_readymade_shield (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                             DivaCairoState state)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));

        cairo_save (this->Cairo);
        
        GdvColor *col1 = gdv_color_new_tango (GDV_COLOR_SCARLETRED1);
        GdvColor *col2 = gdv_color_new_tango (GDV_COLOR_SCARLETRED3);
        
        if (state == DIVA_CAIRO_STATE_ACTIVE) {
                gdv_color_highlight_this (col1);
                gdv_color_highlight_this (col2);
        }
                
        diva_cairo_sketch_shield (this, x0, y0, 12.0);
        
        if (state != DIVA_CAIRO_STATE_DRAG) {
                cairo_pattern_t *gradient_pattern = diva_cairo_sketch_get_linear_pattern2c (this, x0 - 6, y0 - 6,
                                                                                            x0 + 6, y0 + 6,
                                                                                            col1, col2);
                cairo_set_source (this->Cairo, gradient_pattern);
                cairo_fill (this->Cairo);
                cairo_pattern_destroy (gradient_pattern);
        } else {
                cairo_set_source_rgb (this->Cairo, 1, 1, 1);
                cairo_fill (this->Cairo);
        }
        
        // Black Outline
        diva_cairo_sketch_shield (this, x0 + 0.5, y0 + 0.5, 12.0);
        cairo_set_source_rgb (this->Cairo, 0, 0, 0);
        cairo_set_line_width (this->Cairo, 1.0);
        cairo_stroke (this->Cairo);
        
        // White Outline
        diva_cairo_sketch_shield (this, x0 + 0.5, y0 + 0.5, 10.0);
        cairo_set_source_rgba (this->Cairo, 1, 1, 1, 0.5);
        cairo_set_line_width (this->Cairo, 1.0);
        cairo_stroke (this->Cairo);
        
        // Free
        gdv_color_free (col1);
        gdv_color_free (col2);
        cairo_restore (this->Cairo);
}

void                            diva_cairo_readymade_tag (DivaCairoGraphics *this, gdouble x0, gdouble y0,
                                                          gchar *tag)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));

        // Check how much the text will take
        PangoLayout *layout = pango_layout_new (this->Context);
        gchar *markup = g_markup_printf_escaped ("<small>%s</small>", tag);
        pango_layout_set_markup (layout, markup, -1);
                
        gint height, width;
        pango_layout_get_pixel_size (layout, &width, &height);
        
        GdvColor *col = gdv_color_new_tango (GDV_COLOR_ALUMINIUM2);
        diva_cairo_draw_rounded_solid (this, x0, y0, width + 6, height + 4, 5, col,
                                       DIVA_CAIRO_CUT_NONE);

        gdk_draw_layout (this->Window, this->Style->black_gc, 
                         x0 + 3, y0 + 2,
                         layout);
        
        g_object_unref (G_OBJECT (layout));
        g_free (markup);
        
        gdv_color_free (col);
}

void                            diva_cairo_readymade_tag_get_size (DivaCairoGraphics *this, 
                                                                   gchar *tag, gint32 *tagwidth, gint32 *tagheight)
{
        g_return_if_fail (tagwidth != NULL && tagheight != NULL);

        PangoLayout *layout = pango_layout_new (this->Context);
        gchar *markup = g_markup_printf_escaped ("<small>%s</small>", tag);
        pango_layout_set_markup (layout, markup, -1);
                        
        gint height, width;
        pango_layout_get_pixel_size (layout, &width, &height);
        g_object_unref (G_OBJECT (layout));
        g_free (markup);
        
        *tagwidth = width + 6;
        *tagheight = height + 4;
}

void                            diva_cairo_readymade_time_stripe (DivaCairoGraphics *this,
                                                                  int startx, int width, GdvColor *color, 
                                                                  const GdkRectangle *exposerect,
                                                                  const GdkRectangle *totalrect)
{
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        g_return_if_fail (color != NULL);
        g_return_if_fail (exposerect != NULL);

        // Rect of the stripe
        GdkRectangle *drawrect = g_new (GdkRectangle, 1);
        drawrect->x = startx;
        drawrect->y = totalrect->y;
        drawrect->width = width;
        drawrect->height = totalrect->height;

        // Rect to actually draw
        GdkRectangle *intersected = g_new (GdkRectangle, 1);
        gdk_rectangle_intersect (drawrect, (GdkRectangle *) exposerect, intersected);

        // Empty...
        if (intersected->width <= 0 ||
            intersected->height <= 0)
                goto done;

        // Save state
        cairo_save (this->Cairo);
        
        // Shape
        cairo_move_to (this->Cairo, intersected->x, intersected->y);
        cairo_line_to (this->Cairo, intersected->x + intersected->width, intersected->y);
        cairo_line_to (this->Cairo, intersected->x + intersected->width, intersected->y + intersected->height);
        cairo_line_to (this->Cairo, intersected->x, intersected->y + intersected->height);
        cairo_line_to (this->Cairo, intersected->x, intersected->y);
        cairo_close_path (this->Cairo);

        // Color
        diva_cairo_graphics_set_color (this, color);

        // Draw
        cairo_fill (this->Cairo);
        
        // Cleanup
        cairo_restore (this->Cairo);
        
 done:
        g_free (drawrect);
        g_free (intersected);
}
                                                                   
