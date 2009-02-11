/* Copyright (c) 2005 MichalDominik@gmail.com ; MIT X11 license
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
 * OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR
 * THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "DivaWidgetsPaintRenderer.h"

/* Global */

static GtkCellRendererClass*    ParentClass;

#define                         PAINT_WIDTH 16

/* Public */

enum {
        ARG_0,
        ARG_COLOR,
        ARG_CUSTOMIZE
};

/* GObject type */
GType                           diva_widgets_paint_renderer_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {
                
                static const GTypeInfo objectinfo = {
                        sizeof (DivaWidgetsPaintRendererClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) diva_widgets_paint_renderer_class_init,
                        NULL, 
                        NULL,
                        sizeof (DivaWidgetsPaintRenderer),
                        4,
                        (GInstanceInitFunc) diva_widgets_paint_renderer_init,
                };
                objecttype = g_type_register_static
                        (GTK_TYPE_CELL_RENDERER, "DivaWidgetsPaintRenderer", &objectinfo, 0);
        }
        return objecttype;        
}

/* GObject class init */
void                            diva_widgets_paint_renderer_class_init (DivaWidgetsPaintRendererClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GtkCellRendererClass *cellrendererclass = (GtkCellRendererClass *) klass;
        
        ParentClass = g_type_class_ref (GTK_TYPE_CELL_RENDERER);
        
        cellrendererclass->get_size = (gpointer) diva_widgets_paint_renderer_get_size;
        cellrendererclass->render= (gpointer) diva_widgets_paint_renderer_render;

        gobjectclass->get_property = (gpointer) diva_widgets_paint_renderer_get_property;
        gobjectclass->set_property = (gpointer) diva_widgets_paint_renderer_set_property;
        
        g_object_class_install_property (gobjectclass, ARG_COLOR, 
                                         g_param_spec_boxed 
                                         ("color", "Color",
                                          "The color to showcase",
                                          GDV_TYPE_COLOR,
                                          G_PARAM_READWRITE));
        
        g_object_class_install_property (gobjectclass, ARG_CUSTOMIZE, 
                                         g_param_spec_boolean 
                                         ("customize", "Customize",
                                          "If the current color is drawn as customizable",
                                          FALSE,
                                          G_PARAM_READWRITE));

        gobjectclass->finalize = (gpointer) diva_widgets_paint_renderer_finalize;
}

/* GObject init */
void                            diva_widgets_paint_renderer_init (DivaWidgetsPaintRenderer *this)
{
        // Allocate blackness
        this->_Color = gdv_color_new_rgb_8 (0, 0, 0);
        this->_Customize = FALSE;
}

/* Create new instance. For the managed constructor */
DivaWidgetsPaintRenderer*       diva_widgets_paint_renderer_new ()
{
        DivaWidgetsPaintRenderer *this = g_object_new (DIVA_WIDGETS_TYPE_PAINT_RENDERER, NULL);
        return this;
}

/* GObject finalize */
void                            diva_widgets_paint_renderer_finalize (DivaWidgetsPaintRenderer *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_PAINT_RENDERER (this));
        
        if (this->_Color != NULL) {
                gdv_color_free (this->_Color);
                this->_Color = NULL;
        }

        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject get property */
void                            diva_widgets_paint_renderer_get_property (DivaWidgetsPaintRenderer *this, guint propid,
                                                                          GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (DIVA_WIDGETS_IS_PAINT_RENDERER (this));
        
        switch (propid) {

                case ARG_COLOR:
                g_value_set_boxed (value, this->_Color);
                break;

                case ARG_CUSTOMIZE:
                g_value_set_boolean (value, this->_Customize);
                break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GObject set property */
void                            diva_widgets_paint_renderer_set_property (DivaWidgetsPaintRenderer *this, guint propid,
                                                                          const GValue *value, GParamSpec *pspec) \
{
        g_return_if_fail (DIVA_WIDGETS_IS_PAINT_RENDERER (this));
        
        switch (propid) {

                case ARG_COLOR:
                if (this->_Color != NULL)
                        gdv_color_free (this->_Color);
                this->_Color = g_value_dup_boxed (value);
                break;

                case ARG_CUSTOMIZE:
                this->_Customize = g_value_get_boolean (value);
                break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GtkCellRenderer get size */
void                            diva_widgets_paint_renderer_get_size (DivaWidgetsPaintRenderer *this, GtkWidget *widget,
                                                                      GdkRectangle *cell_area, gint *x_offset,
                                                                      gint *y_offset, gint *width, gint *height)
{
        g_return_if_fail (DIVA_WIDGETS_IS_PAINT_RENDERER (this));
    
        gint calc_width, calc_height;
        calc_width = (gint) GTK_CELL_RENDERER (this)->xpad * 2 + (PAINT_WIDTH * 1.5) + 3;
        calc_height = (gint) GTK_CELL_RENDERER (this)->ypad * 2 + PAINT_WIDTH + 2;
    
        if (width)
                *width = calc_width;        
        if (height)
                *height = calc_height;
        
        if (cell_area)
        {
                if (x_offset)
                {
                        *x_offset = GTK_CELL_RENDERER (this)->xalign * (cell_area->width - calc_width);
                        *x_offset = MAX (*x_offset, 0);
                }
                
                if (y_offset)
                {
                        *y_offset = GTK_CELL_RENDERER (this)->yalign * (cell_area->height - calc_height);
                        *y_offset = MAX (*y_offset, 0);
                }
        }
}

/* GtkCellRenderer render function */
void                            diva_widgets_paint_renderer_render (DivaWidgetsPaintRenderer *this, GdkWindow *window,
                                                                   GtkWidget *widget, GdkRectangle *background_area,
                                                                   GdkRectangle *cell_area, GdkRectangle *expose_area,
                                                                   guint flags)
{
        g_return_if_fail (DIVA_WIDGETS_IS_PAINT_RENDERER (this));
        g_return_if_fail (this->_Color != NULL);
        
        // Silently exit if empty color
        if (gdv_color_is_empty (this->_Color))
                return;
        
        gint width = -1; 
        gint height = -1;
        gint x_offset = -1; 
        gint y_offset = -1;
        DivaCairoGraphics *cairo = NULL;
        
        diva_widgets_paint_renderer_get_size (this, widget, cell_area,
                                              &x_offset, &y_offset,
                                              &width, &height);

        g_assert (width != -1 && height != -1 &&
                  x_offset != -1 && y_offset != -1);
        
        // Get cairo graphics
        cairo = diva_cairo_graphics_new (window, widget->style,
                                         gtk_widget_get_pango_context (widget));
        g_assert (cairo != NULL);
        
        width  -= GTK_CELL_RENDERER (this)->xpad * 2 + 3;
        height -= GTK_CELL_RENDERER (this)->ypad * 2 + 2;

        diva_cairo_draw_rounded_solid (cairo, 
                                       cell_area->x + x_offset + GTK_CELL_RENDERER (this)->xpad + 1, 
                                       cell_area->y + y_offset + GTK_CELL_RENDERER (this)->ypad + 1,
                                       width - 1, height - 1,
                                       5, this->_Color, DIVA_CAIRO_CUT_NONE);
        
        diva_cairo_draw_rounded_outline (cairo, 
                                         cell_area->x + x_offset + GTK_CELL_RENDERER (this)->xpad + 1, 
                                         cell_area->y + y_offset + GTK_CELL_RENDERER (this)->ypad + 1,
                                         width - 1, height - 1, 5,
                                         DIVA_CAIRO_OUTLINE_WHITE | DIVA_CAIRO_OUTLINE_BLACK,
                                         DIVA_CAIRO_CUT_NONE);

        if (this->_Customize) 
                diva_cairo_draw_centered_text (cairo, 
                                               cell_area->x + x_offset + GTK_CELL_RENDERER (this)->xpad + 1 + ((width - 1) / 2),
                                               cell_area->y + y_offset + GTK_CELL_RENDERER (this)->ypad + 1 + ((height - 1) / 2),
                                               10, TRUE, "Sans", "?");


        diva_cairo_graphics_dispose (cairo);
        diva_cairo_graphics_finalize (cairo);
}
