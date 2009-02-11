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

#include "DivaWidgetsTaggyRenderer.h"
#include "DivaWidgetsTaggyRendererPrivate.h"

static GtkCellRendererClass*    ParentClass = NULL;

/* GObject type */
GType                           diva_widgets_taggy_renderer_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {
                
                static const GTypeInfo objectinfo = {
                        sizeof (DivaWidgetsTaggyRendererClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) diva_widgets_taggy_renderer_class_init,
                        NULL,
                        NULL,
                        sizeof (DivaWidgetsTaggyRenderer),
                        4,
                        (GInstanceInitFunc) diva_widgets_taggy_renderer_init,
                };
                objecttype = g_type_register_static
                        (GTK_TYPE_CELL_RENDERER, "DivaWidgetsTaggyRenderer", &objectinfo, 0);
        }
        return objecttype;        
}

/* GObject class init */
void                            diva_widgets_taggy_renderer_class_init (DivaWidgetsTaggyRendererClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GtkCellRendererClass *cellrendererclass = (GtkCellRendererClass *) klass;
        
        ParentClass = g_type_class_ref (GTK_TYPE_CELL_RENDERER);
        
        cellrendererclass->get_size = (gpointer) diva_widgets_taggy_renderer_get_size;
        cellrendererclass->render= (gpointer) diva_widgets_taggy_renderer_render;

        gobjectclass->get_property = (gpointer) diva_widgets_taggy_renderer_get_property;
        gobjectclass->set_property = (gpointer) diva_widgets_taggy_renderer_set_property;

        g_object_class_install_property (gobjectclass, ARG_TAGS,
					 g_param_spec_string
					 ("tags", "Tags",
					  "The name of this item",
					  "Noname", G_PARAM_READWRITE));

         g_object_class_install_property (gobjectclass, ARG_MAJOR,
					 g_param_spec_string
					 ("major", "Major",
					  "The major string to display",
					  "Noname", G_PARAM_READWRITE));

          g_object_class_install_property (gobjectclass, ARG_MINOR,
					 g_param_spec_string
					 ("minor", "Minor",
					  "The minor string to display",
					  "Noname", G_PARAM_READWRITE));
        
        gobjectclass->dispose = (gpointer) diva_widgets_taggy_renderer_dispose;
        gobjectclass->finalize = (gpointer) diva_widgets_taggy_renderer_finalize;        
}

/* GObject init */
void                            diva_widgets_taggy_renderer_init (DivaWidgetsTaggyRenderer *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));
        this->Private = NULL;

        DivaWidgetsTaggyRendererPrivate *priv = g_new0 (DivaWidgetsTaggyRendererPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
        }
}

/* Create new instance. For the managed constructor */
DivaWidgetsTaggyRenderer*       diva_widgets_taggy_renderer_new ()
{
        DivaWidgetsTaggyRenderer *this = g_object_new (DIVA_WIDGETS_TYPE_TAGGY_RENDERER, NULL);

        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        } else
                return this;
}

/* GObject dispose */
void                            diva_widgets_taggy_renderer_dispose (DivaWidgetsTaggyRenderer *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));

        if (this->Private != NULL)
                private_dispose (this->Private);

        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
void                            diva_widgets_taggy_renderer_finalize (DivaWidgetsTaggyRenderer *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));

        if (this->Private != NULL)
                g_free (this->Private);

        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject get property */
void                            diva_widgets_taggy_renderer_get_property (DivaWidgetsTaggyRenderer *this, guint propid,
                                                                          GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));
        
        switch (propid) {

                case ARG_TAGS:
                        g_value_set_string (value, this->Private->TagString);
                        break;

                case ARG_MAJOR:
                        g_value_set_string (value, this->Private->MajorString);
                        break;

                case ARG_MINOR:
                        g_value_set_string (value, this->Private->MinorString);
                        break;
                        
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                        break;
        }
}

/* GObject set property */
void                            diva_widgets_taggy_renderer_set_property (DivaWidgetsTaggyRenderer *this, guint propid,
                                                                          const GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));
        
        switch (propid) {

                case ARG_MAJOR:
                        if (this->Private->MajorString != NULL)
                                g_free (this->Private->MajorString);
                        this->Private->MajorString = g_value_dup_string (value);
                        break;

                case ARG_MINOR:
                        if (this->Private->MinorString != NULL)
                                g_free (this->Private->MinorString);
                        this->Private->MinorString = g_value_dup_string (value);
                        break;
                        
                case ARG_TAGS: {
                        // The string
                        if (this->Private->TagString != NULL)
                                g_free (this->Private->TagString);
                        this->Private->TagString = g_value_dup_string (value);
                        
                        // Now let's rebuild our list
                        if (this->Private->TagList != NULL)
                                dispose_tag_list (this->Private->TagList);

                        this->Private->TagList = NULL;

                        // Check for an empty string
                        if (strlen (this->Private->TagString) == 0)
                                return;

                        // Split it
                        gchar **strs = g_strsplit (this->Private->TagString, " ", -1);
                        if (strs == NULL)
                                return;

                        // Now, for each string let's copy it into our list
                        int i = 0;
                        while (strs [i] != NULL) {
                                this->Private->TagList = g_slist_append (this->Private->TagList, g_strdup (strs [i]));
                                i++;
                        }

                        // Free the temporary string array
                        g_strfreev (strs);
                        
                } break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GtkCellRenderer get size */
void                            diva_widgets_taggy_renderer_get_size (DivaWidgetsTaggyRenderer *this, GtkWidget *widget,
                                                                      GdkRectangle *cell_area, gint *x_offset,
                                                                      gint *y_offset, gint *width, gint *height)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));

        gint calc_width; 
        gint calc_height;
        gint wx = 0;
        gint wy = 0;
        DivaCairoGraphics *cairo = NULL;

        if (width && *width != 0)
                wx = *width;
        else
                wx = 150;

        cairo = diva_cairo_graphics_new_limited_context (gtk_widget_get_pango_context (widget));
        g_assert (cairo != NULL);

        // Major string 
        if (this->Private->MajorString != NULL && *this->Private->MajorString != 0) {
                PangoLayout *layout = pango_layout_new (cairo->Context);
                gchar *markup = g_markup_printf_escaped ("<b>%s</b>", this->Private->MajorString);
                pango_layout_set_markup (layout, markup, -1);
                gint panheight, panwidth;
                pango_layout_get_pixel_size (layout, &panwidth, &panheight);
                g_object_unref (G_OBJECT (layout));

                if (this->Private->MinorString != NULL && *this->Private->MinorString != 0)
                        panheight += 2;
                
                wy += panheight;
                g_free (markup);
        }

        // Minor string 
        if (this->Private->MinorString != NULL && *this->Private->MinorString != 0) {
                PangoLayout *layout = pango_layout_new (cairo->Context);
                pango_layout_set_text (layout, this->Private->MinorString, -1);
                gint panheight, panwidth;
                pango_layout_get_pixel_size (layout, &panwidth, &panheight);
                g_object_unref (G_OBJECT (layout));

                wy += panheight;
        }

        // Skip all this if no tags set
        if (this->Private->TagList == NULL)
                goto calculate;

        if ((this->Private->MajorString != NULL && *this->Private->MajorString != 0) ||
            (this->Private->MinorString != NULL && *this->Private->MinorString != 0))
                wy += 8;

        // Let's copy the list for the calculator to work on
        GSList *calccopy = g_slist_copy (this->Private->TagList);
        g_assert (calccopy != NULL);

        if (cell_area)
                wx = MAX (wx, cell_area->width - 5);

        gint tagh = 0; 
        gint32 pixelsleft = wx;
        while (g_slist_length (calccopy) != 0) {
                gint currentw = 0;

                gchar *tag = get_next_tag (cairo, &calccopy, pixelsleft, &currentw, &tagh);

                if (tag == NULL) {
                        pixelsleft = wx;
                        wy += (tagh + 2);
                } else 
                        pixelsleft -= (currentw + 2);
        }
        
        wy += (tagh + 2);

        // No need to free the calccopy list, it's already freed

 calculate:
        // Cell renderer calculations
        
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

        diva_cairo_graphics_dispose (cairo);
        diva_cairo_graphics_finalize (cairo);
}

/* GtkCellRenderer render function */
void                            diva_widgets_taggy_renderer_render (DivaWidgetsTaggyRenderer *this, GdkWindow *window,
                                                                    GtkWidget *widget, GdkRectangle *background_area,
                                                                    GdkRectangle *cell_area, GdkRectangle *expose_area,
                                                                    guint flags)
{
        g_return_if_fail (DIVA_WIDGETS_IS_TAGGY_RENDERER (this));
        
        gint width, height;
        gint x_offset, y_offset;
        DivaCairoGraphics *cairo = NULL;

        diva_widgets_taggy_renderer_get_size (this, widget, cell_area,
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

        gdouble x = cell_area->x;
        gdouble y = cell_area->y;

        // Major string 
        if (this->Private->MajorString != NULL && *this->Private->MajorString != 0) {
                PangoLayout *layout = pango_layout_new (cairo->Context);
                gchar *markup = g_markup_printf_escaped ("<b>%s</b>", this->Private->MajorString);
                pango_layout_set_markup (layout, markup, -1);
                gint panheight, panwidth;
                pango_layout_set_width (layout, cell_area->width * PANGO_SCALE);
                pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
                pango_layout_get_pixel_size (layout, &panwidth, &panheight);
                gdk_draw_layout (cairo->Window, cairo->Style->black_gc, 
                                 x, y,
                                 layout);
                
                g_object_unref (G_OBJECT (layout));

                if (this->Private->MinorString != NULL && *this->Private->MinorString != 0)
                        panheight += 2;
                
                y += panheight;
                g_free (markup);
        }

        // Minor string 
        if (this->Private->MinorString != NULL && *this->Private->MinorString != 0) {
                PangoLayout *layout = pango_layout_new (cairo->Context);
                pango_layout_set_width (layout, cell_area->width * PANGO_SCALE);
                pango_layout_set_ellipsize (layout, PANGO_ELLIPSIZE_END);
                pango_layout_set_text (layout, this->Private->MinorString, -1);
                gint panheight, panwidth;
                pango_layout_get_pixel_size (layout, &panwidth, &panheight);

                gdk_draw_layout (cairo->Window, cairo->Style->black_gc, 
                                 x, y,
                                 layout);
                
                g_object_unref (G_OBJECT (layout));

                y += panheight;
        }

        // Tags
        if (this->Private->TagList == NULL)
                goto done;

        if ((this->Private->MajorString != NULL && *this->Private->MajorString != 0) ||
            (this->Private->MinorString != NULL && *this->Private->MinorString != 0))
                y += 8;
        
        GSList *calccopy = g_slist_copy (this->Private->TagList);
        g_assert (calccopy != NULL);

        gint32 pixelsleft = cell_area->width;
        gint32 hhh = 0;
                
        while (g_slist_length (calccopy) != 0) {
                gint currentw = 0;
                gchar *tag = get_next_tag (cairo, &calccopy, pixelsleft, &currentw, &hhh);
                
                if (tag == NULL) {
                        pixelsleft = cell_area->width;
                        y += (hhh + 2);
                        x = cell_area->x;
                } else {
                        diva_cairo_readymade_tag (cairo, x, y, tag);
                        pixelsleft -= (currentw  + 2);
                        x += (currentw + 2);
                }
        }

        // No need to free the calccopy list, it's already freed

 done:
        diva_cairo_graphics_dispose (cairo);
        diva_cairo_graphics_finalize (cairo);

}

/* Private init */
static gboolean                 private_init (DivaWidgetsTaggyRendererPrivate *private)
{
        private->TagString = NULL;
        private->TagList = NULL;
        private->MajorString = NULL;
        private->MinorString = NULL;
        
        return TRUE;
}

/* Private dispose */
static void                     private_dispose (DivaWidgetsTaggyRendererPrivate *private)
{
        if (private->TagString != NULL) {
                g_free (private->TagString);
                private->TagString = NULL;
        }

        if (private->MajorString != NULL) {
                g_free (private->MajorString);
                private->MajorString = NULL;
        }

        if (private->MinorString != NULL) {
                g_free (private->MinorString);
                private->MinorString = NULL;
        }

        if (private->TagList != NULL) {
                dispose_tag_list (private->TagList);
                private->TagList = NULL;
        }
}

/* Private dispose */
static void                     dispose_tag_list (GSList *list)
{
        GSList *iterator;
        for (iterator = list; iterator; iterator = g_slist_next (iterator))
                if (iterator->data != NULL)
                        g_free (iterator->data);
        
        g_slist_free (list);
}

/* Find the next string that can fit the current pixels left */
static gchar*                   get_next_tag (DivaCairoGraphics *cairo, GSList **list,
                                              gint32 pixelsleft, gint32 *outwidth, gint32 *outheight)
{
        g_return_val_if_fail (list != NULL, NULL);
        g_return_val_if_fail (outwidth != NULL, NULL);

        if (pixelsleft <= 0)
                return NULL;

        GSList *iterator;
        for (iterator = *list; iterator; iterator = g_slist_next (iterator)) {
                gchar *tag = iterator->data;
                g_assert (tag != NULL);

                // We're trying to find the first one that fits
                gint32 ww, hh;
                diva_cairo_readymade_tag_get_size (cairo, tag, &ww, &hh);

                if (ww <= pixelsleft) {
                        // That's the one
                        *outwidth = ww;
                        *outheight = hh;    
                        *list = g_slist_remove (*list, tag);
                        return tag;
                }
        }

        return NULL;
}
