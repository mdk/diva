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

/* Renders a frame (cairo) representing the given aspect ratio */

#include "DivaWidgetsAspectFrame.h"

/* Private */

static gboolean                 on_expose_event (DivaWidgetsAspectFrame *this, GdkEventExpose *event);

static void                     on_size_request (DivaWidgetsAspectFrame *this, GtkRequisition *req);

/* Global */

static GtkDrawingAreaClass*      ParentClass;

/* Public */

enum {
        ARG_0,
        ARG_PIXEL_ASPECT,
        ARG_FRAME_DIMENSIONS,
        ARG_FPS
};

/* GObject type */
GType                           diva_widgets_aspect_frame_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {
                
                static const GTypeInfo objectinfo = {
                        sizeof (DivaWidgetsAspectFrameClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) diva_widgets_aspect_frame_class_init,
                        NULL,
                        NULL,
                        sizeof (DivaWidgetsAspectFrame),
                        1,
                        (GInstanceInitFunc) diva_widgets_aspect_frame_init,
                };
                objecttype = g_type_register_static
                        (GTK_TYPE_DRAWING_AREA, "DivaWidgetsAspectFrame", &objectinfo, 0);
        }
        return objecttype;        
}

/* GObject finalize */
void                            diva_widgets_aspect_frame_finalize (DivaWidgetsAspectFrame *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_ASPECT_FRAME (this));

        if (this->_PixelAspect != NULL) {
                gdv_fraction_free (this->_PixelAspect);
                this->_PixelAspect = NULL;
        }

        if (this->_Dimensions != NULL) {
                gdv_framedimensions_free (this->_Dimensions);
                this->_Dimensions = NULL;
        }

        if (this->_Fps != NULL) {
                gdv_fraction_free (this->_Fps);
                this->_Fps = NULL;
        }

        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject class init */
void                            diva_widgets_aspect_frame_class_init (DivaWidgetsAspectFrameClass *klass)
{
        // FIXME: Finalize functions?
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        ParentClass = g_type_class_ref (GTK_TYPE_DRAWING_AREA);
        
        gobjectclass->get_property = (gpointer) diva_widgets_aspect_frame_get_property;
        gobjectclass->set_property = (gpointer) diva_widgets_aspect_frame_set_property;
        gobjectclass->finalize = (gpointer) diva_widgets_aspect_frame_finalize;

        g_object_class_install_property (gobjectclass, ARG_PIXEL_ASPECT,
                                         g_param_spec_boxed
					 ("pixelaspect", "PixelAspect",
					  "The pixel aspect ration",
					  GDV_TYPE_FRACTION, G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_FPS,
                                         g_param_spec_boxed
					 ("fps", "Fps",
					  "Frames per second",
					  GDV_TYPE_FRACTION, G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_FRAME_DIMENSIONS,
                                         g_param_spec_boxed
					 ("framedimensions", "FrameDimensions",
					  "The frame dimensions (resolution)",
					  GDV_TYPE_FRAMEDIMENSIONS, G_PARAM_READWRITE));
}

/* GObject init */
void                            diva_widgets_aspect_frame_init (DivaWidgetsAspectFrame *this)
{
        g_return_if_fail (DIVA_WIDGETS_IS_ASPECT_FRAME (this));
        
        // Connect signals
        g_signal_connect (G_OBJECT (this), "expose-event", G_CALLBACK (on_expose_event), NULL);
        g_signal_connect (G_OBJECT (this), "size-request", G_CALLBACK (on_size_request), NULL);
        
        // Initialize handler variables
        this->_PixelAspect = gdv_fraction_new (1, 1);
        this->_Dimensions = gdv_framedimensions_new (1, 1);
        this->_Fps = gdv_fraction_new (1, 1);
}

/* Create a new instance */
DivaWidgetsAspectFrame*         diva_widgets_aspect_frame_new ()
{
        return g_object_new (DIVA_WIDGETS_TYPE_ASPECT_FRAME, NULL);
}

/* GObject get property */
void                            diva_widgets_aspect_frame_get_property (DivaWidgetsAspectFrame *this, guint propid,
                                                                        GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (DIVA_WIDGETS_IS_ASPECT_FRAME (this));
        
        switch (propid) {

                case ARG_FRAME_DIMENSIONS:
                        g_return_if_fail (this->_Dimensions != NULL);
                        g_value_set_boxed (value, this->_Dimensions);
                        break;

                case ARG_FPS:
                        g_return_if_fail (this->_Fps != NULL);
                        g_value_set_boxed (value, this->_Fps);
                        break;                        
                
                case ARG_PIXEL_ASPECT:
                        g_return_if_fail (this->_PixelAspect != NULL);
                        g_value_set_boxed (value, this->_PixelAspect);
                        break;
                
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                        break;
        }

}

/* GObject set property */
void                            diva_widgets_aspect_frame_set_property (DivaWidgetsAspectFrame *this, guint propid,
                                                                        const GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (DIVA_WIDGETS_IS_ASPECT_FRAME (this));

        switch (propid) {

                case ARG_FRAME_DIMENSIONS:
                        if (this->_Dimensions != NULL)
                                gdv_framedimensions_free (this->_Dimensions);
                        this->_Dimensions = g_value_dup_boxed (value);
                        gtk_widget_queue_draw (GTK_WIDGET (this));
                        break;

                case ARG_PIXEL_ASPECT:
                        if (this->_PixelAspect != NULL)
                                gdv_fraction_free (this->_PixelAspect);
                        this->_PixelAspect = g_value_dup_boxed (value);
                        gtk_widget_queue_draw (GTK_WIDGET (this));
                        break;

                case ARG_FPS:
                        if (this->_Fps != NULL)
                                gdv_fraction_free (this->_Fps);
                        this->_Fps = g_value_dup_boxed (value);
                        gtk_widget_queue_draw (GTK_WIDGET (this));
                        break;
                
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* The expose event implementation. Here the drawing happens */
static gboolean                 on_expose_event (DivaWidgetsAspectFrame *this, GdkEventExpose *event)
{
        g_return_val_if_fail (DIVA_WIDGETS_IS_ASPECT_FRAME (this), FALSE);
        g_return_val_if_fail (this->_Dimensions != NULL, FALSE);
        g_return_val_if_fail (this->_PixelAspect != NULL, FALSE);

        int width = -1;                        // Width of the drawing widget
        int height = -1;                       // Height of the drawing widget
        DivaCairoGraphics *cairo = NULL;       // Cairo graphics to draw
        GdvFrameDimensions *dimensions = NULL; // Calculated (via pixel) frame dimensions
        GdvFraction *frameaspect = NULL;       // Aspect ratio of the final frame
        GdvFrameDimensions *drawframe = NULL;  // A frame we need to draw
        gchar *dimensionstext = NULL;          // String with dimensions
        gchar *extratext = NULL;               // Strign with fps etc.
        GdvFraction *squarepixel = NULL;       // 1/1 fraction for multiplying
        GdvFraction *widgetaspect = NULL;      // Widget aspect ratio

        // Get width & height
        width = GTK_WIDGET (this)->allocation.width - 4;
        height = GTK_WIDGET (this)->allocation.height - 4;
        g_assert (width != -1 && height != -1);

        // If we're really small we fall back
        if (width < 10 || height < 10)
                goto MrFree;

        // Create the cairo graphics structure
        cairo = diva_cairo_graphics_new (GTK_WIDGET (this)->window, GTK_WIDGET (this)->style, 
                                         gtk_widget_get_pango_context (GTK_WIDGET (this)));
        if (cairo == NULL)
                goto MrFree;

        // Calculate the pixel aspect ratio and everything
        squarepixel = gdv_fraction_new (1, 1);
        dimensions = gdv_fraction_aspect_pixelize (squarepixel, this->_PixelAspect, this->_Dimensions);
        g_assert (dimensions != NULL);

        // Calculate the aspect ratio
        frameaspect = gdv_fraction_aspect_new_from_frame (dimensions);

        // Get widget aspect ratio
        widgetaspect = gdv_fraction_new (width, height);

        drawframe = gdv_framedimensions_new (1,1);
        if (gdv_fraction_compare (widgetaspect, frameaspect) == -1)
                gdv_framedimensions_fit_w (dimensions, width, drawframe);
        else
                gdv_framedimensions_fit_h (dimensions, height, drawframe);

        // Draw it finally!
        diva_cairo_readymade_checker_frame (cairo,
                                            (width - drawframe->Width) / 2 + 2,
                                            (height - drawframe->Height) / 2 + 2,
                                            drawframe->Width , drawframe->Height, 8);

        dimensionstext = g_strdup_printf ("%dx%d",
                                          this->_Dimensions->Width,
                                          this->_Dimensions->Height);

        extratext = g_strdup_printf ("%.2f fps", gdv_fraction_to_double (this->_Fps));

        diva_cairo_draw_centered_text (cairo, (width + 4) / 2, (height + 4) / 2 - 13,
                                       15, TRUE, "Sans", dimensionstext);

        diva_cairo_draw_centered_text (cairo, (width + 4) / 2, (height + 4) / 2 + 2,
                                       12, FALSE, "Sans", extratext);

        MrFree:
        if (cairo != NULL) {
                diva_cairo_graphics_dispose (cairo);
                diva_cairo_graphics_finalize (cairo);
        }
        
        if (dimensions != NULL)
                gdv_framedimensions_free (dimensions);

        if (frameaspect != NULL)
                gdv_fraction_free (frameaspect);

        if (drawframe != NULL)
                gdv_framedimensions_free (drawframe);

        if (dimensionstext != NULL)
                g_free (dimensionstext);

        if (extratext != NULL)
                g_free (extratext);

        if (widgetaspect != NULL)
                g_free (widgetaspect);

        if (squarepixel != NULL)
                g_free (squarepixel);
        
        // We don't want to keep the event
        return FALSE;
}

/* Here we force 150px height */
static void                     on_size_request (DivaWidgetsAspectFrame *this, GtkRequisition *req)
{
        g_return_if_fail (DIVA_WIDGETS_IS_ASPECT_FRAME (this));
        req->height = 150;
}
