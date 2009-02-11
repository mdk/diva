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

#ifndef __DIVA_WIDGETS_THMB_RENDERER_H__
#define __DIVA_WIDGETS_THMB_RENDERER_H__

#include <gtk/gtk.h>
#include <cairo.h>
#include <divacairo.h>
#include "DivaWidgetsTypes.h"

struct _DivaWidgetsThmbRenderer {
        GtkCellRenderer Parent;

        GdkPixbuf *_Pixbuf; // Thumbnail pixbuf to render
        gboolean _Border;   // Should we render a border
};

struct _DivaWidgetsThmbRendererClass {
        GtkCellRendererClass ParentClass;
};

GType                           diva_widgets_thmb_renderer_get_type (void);

void                            diva_widgets_thmb_renderer_class_init (DivaWidgetsThmbRendererClass *klass);

void                            diva_widgets_thmb_renderer_init (DivaWidgetsThmbRenderer *this);

DivaWidgetsThmbRenderer*        diva_widgets_thmb_renderer_new ();

void                            diva_widgets_thmb_renderer_dispose (DivaWidgetsThmbRenderer *this);

void                            diva_widgets_thmb_renderer_get_property (DivaWidgetsThmbRenderer *this, guint propid,
                                                                         GValue *value, GParamSpec *pspec);

void                            diva_widgets_thmb_renderer_set_property (DivaWidgetsThmbRenderer *this, guint propid,
                                                                         const GValue *value, GParamSpec *pspec);

void                            diva_widgets_thmb_renderer_get_size (DivaWidgetsThmbRenderer *this, GtkWidget *widget,
                                                                     GdkRectangle *cell_area, gint *x_offset,
                                                                     gint *y_offset, gint *width, gint *height);

void                            diva_widgets_thmb_renderer_render (DivaWidgetsThmbRenderer *this, GdkWindow *window,
                                                                   GtkWidget *widget, GdkRectangle *background_area,
                                                                   GdkRectangle *cell_area, GdkRectangle *expose_area,
                                                                   guint flags);

#endif
