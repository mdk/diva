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

#include "DivaCairoUtil.h"

gpointer                       diva_cairo_util_get_checkers (gint width, gint height, gint checker)
{
        g_return_val_if_fail (width > 0, NULL);
        g_return_val_if_fail (height > 0, NULL);
        g_return_val_if_fail (checker > 1, NULL);
                
        int x, y, ch;
        int iterator = 0;
        int mod;
        
        unsigned char *data = g_malloc (width * height * 3);
        unsigned char col;
        
        for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                        
                        mod = (y / checker) % 2; 
                        if (((x / checker) + mod) % 2 == 0) 
                                col = 230;
                        else 
                                col = 255;
                        
                        for (ch = 0; ch < 3; ch++) {
                                                        
                                data [iterator] = col;        
                                iterator++;
                        }

                }
        }                

        // Create the pixbuf
        gpointer buf = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB, FALSE,
                                                 8, width, height, 
                                                 width * 3, (gpointer) g_free, NULL);
        
        return buf;
        
}
