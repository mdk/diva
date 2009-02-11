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

#include "GdvColor.h"

/* GBoxed type */
GType                           gdv_color_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {
                objecttype = g_boxed_type_register_static 
                        ("GdvColor", 
                         (GBoxedCopyFunc) gdv_color_copy,
                         (GBoxedFreeFunc) gdv_color_free);
        }
        
        return objecttype;       
}

/* GBoxed copy */
GdvColor*                       gdv_color_copy (const GdvColor *this)
{
        g_return_val_if_fail (this != NULL, NULL);

        GdvColor *copy = g_new (GdvColor, 1);
        g_assert (copy != NULL);
        
        copy->Red = this->Red;
        copy->Green = this->Green;
        copy->Blue = this->Blue;
        copy->Alpha = this->Alpha;
        return copy;
}

/* GBoxed free */
void                            gdv_color_free (GdvColor *this)
{
        g_return_if_fail (this != NULL);
        g_free (this);
}

/* If color is empty */
gboolean                        gdv_color_is_empty (GdvColor *this)
{
        g_return_val_if_fail (this != NULL, TRUE);
        
        if (this->Red == -1.0f &&
            this->Green == -1.0f &&
            this->Blue == -1.0 &&
            this->Alpha == -1.0)
                return TRUE;
        else
                return FALSE;
}

/* Create an empty color */
GdvColor*                       gdv_color_empty (void)
{
        return gdv_color_new_rgba_f (-1.0f, -1.0f, -1.0f, -1.0f);
}

/* Clamp the color values to a 0.0 - 1.0 range */
void                            gdv_color_clamp (GdvColor *this)
{
        g_assert (this != NULL);
        
        this->Red = MIN (this->Red, 1.0); this->Red = MAX (this->Red, 0.0);
        this->Green = MIN (this->Green, 1.0); this->Green = MAX (this->Green, 0.0);
        this->Blue = MIN (this->Blue, 1.0); this->Blue = MAX (this->Blue, 0.0);
}

/* Create new instance (RGB floats) */
GdvColor*                       gdv_color_new_rgb_f (gfloat red, gfloat green, gfloat blue)
{
        return gdv_color_new_rgba_f (red, green, blue, 1.0);
}

/* Create new instance (RGBA floats) */
GdvColor*                       gdv_color_new_rgba_f (gfloat red, gfloat green, gfloat blue, gfloat alpha)
{
        GdvColor *this = g_new (GdvColor, 1);

        this->Red = red;
        this->Blue = blue;
        this->Green = green;
        this->Alpha = alpha;
        
        gdv_color_clamp (this);
        return this;
}

/* Create new instance (RGBA bytes) */
GdvColor*                       gdv_color_new_rgba_8 (guchar red, guchar green, guchar blue, guchar alpha)
{
        GdvColor *this = g_new (GdvColor, 1);

        this->Red = (gfloat) red / 255.0;
        this->Blue = (gfloat) blue / 255.0;
        this->Green = (gfloat) green / 255.0;
        this->Alpha = (gfloat) alpha / 255.0;
        
        gdv_color_clamp (this);
        return this;
}

/* Create new instance (RGB bytes) */
GdvColor*                       gdv_color_new_rgb_8 (guchar red, guchar green, guchar blue)
{
        return gdv_color_new_rgba_8 (red, green, blue, 255);
}

/* Check if two colors are equal */
gboolean                        gdv_color_equals (const GdvColor *this, const GdvColor *other)
{
        if (this->Red == other->Red &&
            this->Green == other->Green &&
            this->Blue == other->Blue &&
            this->Alpha == other->Alpha)
                return TRUE;
        else
                return FALSE;
}

/* Convert to a valid gdk color */
GdkColor*                       gdv_color_to_gdkcolor (const GdvColor *this)
{
        GdkColor *gdkcolor = g_new (GdkColor, 1);
        g_return_val_if_fail (gdkcolor != NULL, NULL);
        
        gdkcolor->red = this->Red * 65536;
        gdkcolor->green = this->Green * 65536;
        gdkcolor->blue = this->Blue * 65536;
        
        return gdkcolor;
}

/* Create a GdvColor from a GdkColor */
GdvColor*                       gdv_color_from_gdkcolor (const GdkColor *color)
{
        g_return_val_if_fail (color != NULL, NULL);
        
        return gdv_color_new_rgb_8 (color->red / 256, color->green / 256,
                                    color->blue / 256);
}

/* Create a color from a stock color const */
GdvColor*                       gdv_color_new_const (GdvColorConst val)
{
        switch (val) {
                
                case GDV_COLOR_BLACK:
                return gdv_color_new_rgb_f (0, 0, 0);
                break;
                
                case GDV_COLOR_WHITE:
                return gdv_color_new_rgb_f (1, 1, 1);
                break;
                
                case GDV_COLOR_RED:
                return gdv_color_new_rgb_f (1.0, 0, 0);
                break;
                
                case GDV_COLOR_GREEN:
                return gdv_color_new_rgb_f (0, 1.0, 0);
                break;
                
                case GDV_COLOR_BLUE:
                return gdv_color_new_rgb_f (0, 0, 1.0);
                break;
                
                default:
                g_assert_not_reached ();
                break;
                
        }
}

/* Create a color from a Tango colors palette */
GdvColor*                       gdv_color_new_tango (GdvColorTango val)
{
        // This stuff is directly C&P'ed from the Tango gimp palette.
        // More info: http://tango-project.org/Tango_Icon_Theme_Guidelines
        
        switch (val) {
                
                // Butter
                
                case GDV_COLOR_BUTTER1:
                return gdv_color_new_rgb_8 (252, 233, 79);
                break;
                
                case GDV_COLOR_BUTTER2:
                return gdv_color_new_rgb_8 (237, 212, 0);
                break;
                
                case GDV_COLOR_BUTTER3:
                return gdv_color_new_rgb_8 (196, 160, 0);
                break;
                
                // Chameleon
                
                case GDV_COLOR_CHAMELEON1:
                return gdv_color_new_rgb_8 (138, 226, 52);
                break;

                case GDV_COLOR_CHAMELEON2:
                return gdv_color_new_rgb_8 (115, 210, 22);
                break;
                
                case GDV_COLOR_CHAMELEON3:
                return gdv_color_new_rgb_8 (78, 154, 6);
                break;
                
                // Orange
                
                case GDV_COLOR_ORANGE1:
                return gdv_color_new_rgb_8 (252, 175, 62);
                break;

                case GDV_COLOR_ORANGE2:
                return gdv_color_new_rgb_8 (245, 121, 0);                        
                break;
                
                case GDV_COLOR_ORANGE3:
                return gdv_color_new_rgb_8 (206, 92, 0);
                break;
                
                // Sky Blue
                 
                case GDV_COLOR_SKYBLUE1:
                return gdv_color_new_rgb_8 (114, 159, 207);
                break;
                
                case GDV_COLOR_SKYBLUE2:
                return gdv_color_new_rgb_8 (52, 101, 164);
                break;
                
                case GDV_COLOR_SKYBLUE3:
                return gdv_color_new_rgb_8 (32, 74, 135);
                break;
                
                // Plum
                
                case GDV_COLOR_PLUM1:
                return gdv_color_new_rgb_8 (173, 127, 168);
                break;
                
                case GDV_COLOR_PLUM2:
                return gdv_color_new_rgb_8 (117, 80, 123);
                break;
                
                case GDV_COLOR_PLUM3:
                return gdv_color_new_rgb_8 (92, 53, 102);
                break;
                
                // Chocolate
                
                case GDV_COLOR_CHOCOLATE1:
                return gdv_color_new_rgb_8 (233, 185, 110);
                break;
                
                case GDV_COLOR_CHOCOLATE2:
                return gdv_color_new_rgb_8 (193, 125, 17);
                break;
                
                case GDV_COLOR_CHOCOLATE3:
                return gdv_color_new_rgb_8 (143, 89, 2);
                break;
                
                // Scarlett Red
                
                case GDV_COLOR_SCARLETRED1:
                return gdv_color_new_rgb_8 (239, 41, 41);
                break;
                
                case GDV_COLOR_SCARLETRED2:
                return gdv_color_new_rgb_8 (204, 0, 0);
                break;
                
                case GDV_COLOR_SCARLETRED3:
                return gdv_color_new_rgb_8 (164, 0, 0);
                break;
                
                // Aluminium
               
                case GDV_COLOR_ALUMINIUM1:
                return gdv_color_new_rgb_8 (238, 238, 236);
                break;
                
                case GDV_COLOR_ALUMINIUM2:
                return gdv_color_new_rgb_8 (211, 215, 207);
                break;
                
                case GDV_COLOR_ALUMINIUM3:
                return gdv_color_new_rgb_8 (186, 189, 182);
                break;
                
                case GDV_COLOR_ALUMINIUM4:
                return gdv_color_new_rgb_8 (136, 138, 133);
                break;
                
                case GDV_COLOR_ALUMINIUM5:
                return gdv_color_new_rgb_8 (85, 87, 83);
                break;
                
                case GDV_COLOR_ALUMINIUM6:
                return gdv_color_new_rgb_8 (46, 52, 54);
                break;
                                                
                default:
                g_assert_not_reached ();
                break;
        }
}

/* Convert to a HTML-like hex notation (ie. ff00ff). Note that this ignores
 * alpha */
gchar*                          gdv_color_to_html (const GdvColor *this)
{
        return g_strdup_printf ("%.2X%.2X%.2X", (gint) (this->Red * 255.0) , (gint) (this->Green * 255.0), 
                                (gint) (this->Blue * 255.0));
}

GdvColor*                       gdv_color_highlight (const GdvColor *this)
{
        g_return_val_if_fail (this != NULL, NULL);
        GdvColor *copy = gdv_color_copy (this);
        copy->Red *= 1.2;
        copy->Green *= 1.2;
        copy->Blue *= 1.2;
        
        gdv_color_clamp (copy);
        return copy;
}

GdvColor*                       gdv_color_dim (const GdvColor *this)
{
        g_return_val_if_fail (this != NULL, NULL);
        GdvColor *copy = gdv_color_copy (this);
        copy->Red *= 0.8;
        copy->Green *= 0.8;
        copy->Blue *= 0.8;
        
        gdv_color_clamp (copy);
        return copy;
}

void                            gdv_color_highlight_this (GdvColor *this)
{
        g_return_if_fail (this != NULL);
        this->Red *= 1.2;
        this->Green *= 1.2;
        this->Blue *= 1.2;
        
        gdv_color_clamp (this);
}

void                            gdv_color_dim_this (GdvColor *this)
{
        g_return_if_fail (this != NULL);
        this->Red *= 0.8;
        this->Green *= 0.8;
        this->Blue *= 0.8;
        
        gdv_color_clamp (this);
}
