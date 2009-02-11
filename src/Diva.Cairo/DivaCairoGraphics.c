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

#include "DivaCairoGraphics.h"

/* This creates a LIMITED version of the graphics, without the style/pango context.
 * Some graphics functions will NOT work with this graphics. Be sure you know what 
 * you're doing! */
DivaCairoGraphics*              diva_cairo_graphics_new_limited (GdkWindow *window)
{
        g_return_val_if_fail (GDK_IS_WINDOW (window), NULL);
        
        cairo_t *gr = gdk_cairo_create (window);
        g_return_val_if_fail (gr != NULL, NULL);
        
        DivaCairoGraphics *this = g_new0 (DivaCairoGraphics, 1);
        
        this->Style = NULL;
        this->Context = NULL;
        this->Window = window;
        this->Cairo = gr;
                
        g_object_ref (G_OBJECT (window));

        return this;
}

DivaCairoGraphics*              diva_cairo_graphics_new_limited_context (PangoContext *context)
{
        g_return_val_if_fail (PANGO_IS_CONTEXT (context), NULL);
                
        DivaCairoGraphics *this = g_new0 (DivaCairoGraphics, 1);
        
        this->Style = NULL;
        this->Context = context;
        this->Window = NULL;
        this->Cairo = NULL;
                
        g_object_ref (G_OBJECT (context));

        return this;
}

/* Create a new Cairo graphics */
DivaCairoGraphics*              diva_cairo_graphics_new (GdkWindow *window, GtkStyle *style,
                                                        PangoContext *context)
{
        g_return_val_if_fail (GDK_IS_WINDOW (window), NULL);
        g_return_val_if_fail (GTK_IS_STYLE (style), NULL);
        g_return_val_if_fail (PANGO_IS_CONTEXT (context), NULL);
        
        cairo_t *gr = gdk_cairo_create (window);
        g_return_val_if_fail (gr != NULL, NULL);
                
        DivaCairoGraphics *this = g_new0 (DivaCairoGraphics, 1);
        
        this->Style = style;
        this->Context = context;
        this->Window = window;
        this->Cairo = gr;
                
        g_object_ref (G_OBJECT (style));
        g_object_ref (G_OBJECT (window));
        g_object_ref (G_OBJECT (context));

        return this;
}

/* Dispose (loose reference) to all objects we keep */
void                            diva_cairo_graphics_dispose (DivaCairoGraphics *this)
{
        g_return_if_fail (this != NULL);
        
        if (this->Window != NULL) {
                g_object_unref (G_OBJECT (this->Window));
                this->Window = NULL;
        }
        
        if (this->Context != NULL) {
                g_object_unref (G_OBJECT (this->Context));
                this->Context = NULL;
        }
        
        if (this->Style != NULL) {
                g_object_unref (G_OBJECT (this->Style));
                this->Style = NULL;
        }
        
        if (this->Cairo != NULL) {
                cairo_destroy (this->Cairo);
                this->Cairo = NULL;
        }
}

/* Free the structure itself. If we're not disposed, will dispose (with a warning */
void                            diva_cairo_graphics_finalize (DivaCairoGraphics *this)
{
        g_return_if_fail (this != NULL);
        
        if (this->Window != NULL || 
            this->Style != NULL ||
            this->Cairo != NULL ||
            this->Context != NULL) {
                g_warning ("Trying to finalize a non-disposed struct. Disposing first.");
                diva_cairo_graphics_dispose (this);
        }

        g_free (this);
}

/* Set the current color to the passed Gdv color */
void                            diva_cairo_graphics_set_color (DivaCairoGraphics *this, GdvColor *color)
{
        g_return_if_fail (color != NULL);
        g_return_if_fail (DIVA_CAIRO_IS_GRAPHICS (this));
        
        if (color->Alpha != 1.0) 
                cairo_set_source_rgba (this->Cairo, 
                                       color->Red, 
                                       color->Green,
                                       color->Blue, 
                                       color->Alpha);
        else
                cairo_set_source_rgb (this->Cairo, 
                                      color->Red, 
                                      color->Green,
                                      color->Blue);
}

/* Get the assosciated GtkStyle. Refs it. For managed code */
GtkStyle*                       diva_cairo_graphics_get_style (DivaCairoGraphics *this)
{
        g_return_val_if_fail (DIVA_CAIRO_IS_GRAPHICS (this), NULL);
        g_return_val_if_fail (this->Style != NULL, NULL);
        
        g_object_ref (G_OBJECT (this->Style));
        return this->Style;
}

/* Get the assosciated GdkWindow. Refs it. For managed code */
GdkWindow*                      diva_cairo_graphics_get_window (DivaCairoGraphics *this)
{
        g_return_val_if_fail (DIVA_CAIRO_IS_GRAPHICS (this), NULL);
        g_return_val_if_fail (this->Window != NULL, NULL);
        
        g_object_ref (G_OBJECT (this->Window));
        return this->Window;
}

/* Get the assosciated GdkWindow. Refs it. For managed code */
PangoContext*                   diva_cairo_graphics_get_context (DivaCairoGraphics *this)
{
        g_return_val_if_fail (DIVA_CAIRO_IS_GRAPHICS (this), NULL);
        g_return_val_if_fail (this->Context != NULL, NULL);
        
        g_object_ref (G_OBJECT (this->Context));
        return this->Context;                

}
