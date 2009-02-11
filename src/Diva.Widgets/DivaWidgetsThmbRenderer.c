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

/* This is a GtkCellRenderer that renders thumbnails from Pixbufs. Thumbnails
 * have rounded-rectangles. Drawing occures using Cairo.
 * In future it would be actually nice to write such a thing directly in C#, but
 * currently that's not really possible. 
 */

#include "DivaWidgetsThmbRenderer.h"

#define                         THMB_RADIUS 7

/* Global */

static GtkCellRendererClass*    ParentClass;

/* Public */

enum {
        ARG_0,
        ARG_PIXBUF,
        ARG_BORDER,
        
};

/* GObject type */
GType                           diva_widgets_thmb_renderer_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {
                
                static const GTypeInfo objectinfo = {
                        sizeof (DivaWidgetsThmbRendererClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) diva_widgets_thmb_renderer_class_init,
                        NULL,
                        NULL,
                        sizeof (DivaWidgetsThmbRenderer),
                        4,
                        (GInstanceInitFunc) diva_widgets_thmb_renderer_init,
                };
                objecttype = g_type_register_static
                        (GTK_TYPE_CELL_RENDERER, "DivaWidgetsThmbRenderer", &objectinfo, 0);
        }
        return objecttype;        
}

/* GObject class init */
void                            diva_widgets_thmb_renderer_class_init (DivaWidgetsThmbRendererClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GtkCellRendererClass *cellrendererclass = (GtkCellRendererClass *) klass;
        
        ParentClass = g_type_class_ref (GTK_TYPE_CELL_RENDERER);
        
        cellrendererclass->get_size = (gpointer) diva_widgets_thmb_renderer_get_size;
        cellrendererclass->render= (gpointer) diva_widgets_thmb_renderer_render;

        gobjectclass->get_property = (gpointer) diva_widgets_thmb_renderer_get_property;
        gobjectclass->set_property = (gpointer) diva_widgets_thmb_renderer_set_property;
        
        g_object_class_install_property (gobjectclass, ARG_PIXBUF, 
                                         g_param_spec_object 
                                         ("pixbuf", "Pixbuf",
                                          "The pixbuf to render",
                                          GDK_TYPE_PIXBUF,
                                          G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_BORDER, 
                                         g_param_spec_boolean 
                                         ("border", "Border",
                                          "If we should render a border",
                                          FALSE,
                                          G_PARAM_READWRITE));
                
        gobjectclass->dispose = (gpointer) diva_widgets_thmb_renderer_dispose;
}

/* GObject init */
void                            diva_widgets_thmb_renderer_init (DivaWidgetsThmbRenderer *this)
{
        this->_Pixbuf = NULL;
        this->_Border = FALSE;
}

/* Create new instance. For the managed constructor */
DivaWidgetsThmbRenderer*        diva_widgets_thmb_renderer_new ()
{
        DivaWidgetsThmbRenderer *this = g_object_new (DIVA_WIDGETS_TYPE_THMB_RENDERER, NULL);
        return this;
}

/* GObject dispose */
void                            diva_widgets_thmb_renderer_dispose (DivaWidgetsThmbRenderer *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_THMB_RENDERER (this));
        
        if (this->_Pixbuf != NULL) {
                g_object_unref (this->_Pixbuf);
                this->_Pixbuf = NULL;
        }

        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject get property */
void                            diva_widgets_thmb_renderer_get_property (DivaWidgetsThmbRenderer *this, guint propid,
                                                                         GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (DIVA_WIDGETS_IS_THMB_RENDERER (this));
        
        switch (propid) {

                case ARG_BORDER:
                        g_value_set_boolean (value, this->_Border);
                break;

	        case ARG_PIXBUF:
                g_value_set_object (value, this->_Pixbuf);
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GObject set property */
void                            diva_widgets_thmb_renderer_set_property (DivaWidgetsThmbRenderer *this, guint propid,
                                                                         const GValue *value, GParamSpec *pspec) \
{
        g_return_if_fail (DIVA_WIDGETS_IS_THMB_RENDERER (this));
        GdkPixbuf *pixbuf = NULL;
        
        switch (propid) {

                case ARG_BORDER:
                        this->_Border = g_value_get_boolean (value);
                        break;

	        case ARG_PIXBUF:
                pixbuf = (GdkPixbuf *) g_value_get_object (value);
                if (pixbuf != NULL)
                        g_object_ref (pixbuf);
                if (this->_Pixbuf != NULL)
                        g_object_unref (this->_Pixbuf);
                this->_Pixbuf = pixbuf;
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GtkCellRenderer get size */
void                            diva_widgets_thmb_renderer_get_size (DivaWidgetsThmbRenderer *this, GtkWidget *widget,
                                                                     GdkRectangle *cell_area, gint *x_offset,
                                                                     gint *y_offset, gint *width, gint *height)
{
        g_return_if_fail (DIVA_WIDGETS_IS_THMB_RENDERER (this));
        
        gint calc_width; 
        gint calc_height;
        gint wx = 0;
        gint wy = 0;
        
        if (this->_Pixbuf != NULL) {
                wx = gdk_pixbuf_get_width (this->_Pixbuf);
                wy = gdk_pixbuf_get_height (this->_Pixbuf);
        } 
        
        calc_width  = (gint) GTK_CELL_RENDERER (this)->xpad * 2 + wx + 2;
        calc_height = (gint) GTK_CELL_RENDERER (this)->ypad * 2 + wy + 2;
        
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
void                            diva_widgets_thmb_renderer_render (DivaWidgetsThmbRenderer *this, GdkWindow *window,
                                                                   GtkWidget *widget, GdkRectangle *background_area,
                                                                   GdkRectangle *cell_area, GdkRectangle *expose_area,
                                                                   guint flags)
{
        g_return_if_fail (DIVA_WIDGETS_IS_THMB_RENDERER (this));

        // Silently return if not pixbuf
        if (this->_Pixbuf == NULL)
                return;
        
        gint width, height;
        gint x_offset, y_offset;
        DivaCairoGraphics *cairo = NULL;
        
        diva_widgets_thmb_renderer_get_size (this, widget, cell_area,
                                             &x_offset, &y_offset,
                                             &width, &height);
        
        // Get cairo graphics
        if (GDK_IS_WINDOW (window) == FALSE)
                return;
        
        cairo = diva_cairo_graphics_new (window, widget->style, 
                                         gtk_widget_get_pango_context (widget));
        g_assert (cairo != NULL);
        
        width  -= GTK_CELL_RENDERER (this)->xpad * 2 + 2;
        height -= GTK_CELL_RENDERER (this)->ypad * 2 + 2;

        // Draw it all
        // If no border is needed, we're using the native Gdk drawing to make
        // it a bit faster
        
        if (this->_Border == TRUE) {
                diva_cairo_draw_rounded_pixbuf (cairo, 
                                                cell_area->x + cell_area->width - 1 - width,
                                                cell_area->y + y_offset + GTK_CELL_RENDERER (this)->ypad + 1,
                                                width - 1, height - 1,
                                                THMB_RADIUS, this->_Pixbuf, DIVA_CAIRO_CUT_NONE);
                
                diva_cairo_draw_rounded_outline (cairo,
                                                 cell_area->x + cell_area->width - 1 - width,
                                                 cell_area->y + y_offset + GTK_CELL_RENDERER (this)->ypad + 1,
                                                 width - 1, height - 1, 
                                                 THMB_RADIUS,
                                                 DIVA_CAIRO_OUTLINE_WHITE | DIVA_CAIRO_OUTLINE_BLACK,
                                                 DIVA_CAIRO_CUT_NONE);
        } else
                gdk_draw_pixbuf (window, NULL, this->_Pixbuf, 0, 0,
                                 cell_area->x + cell_area->width - 1 - width,
                                 cell_area->y + y_offset + GTK_CELL_RENDERER (this)->ypad + 1,
                                 width - 1, height - 1,
                                 GDK_RGB_DITHER_NORMAL,
                                 0, 0);
        
        diva_cairo_graphics_dispose (cairo);
        diva_cairo_graphics_finalize (cairo);
}
