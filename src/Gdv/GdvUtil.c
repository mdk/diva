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

/* FIXME: Description
 */

#include "GdvUtil.h"

inline gboolean         gdv_util_filetest (const gchar *filename, GError **error)
{
        if (g_file_test (filename, G_FILE_TEST_EXISTS))
                return TRUE;
        else {
                const gchar *errstring = gettext ("File %s could not be found");
                g_set_error (error, GDV_ERROR_DOMAIN, -1, errstring, filename);
                return FALSE;
        }
}

void                    gdv_util_pixbuf_destroyfunc (guchar *data, gpointer userdata)
{
        GDV_DEBUG_E ("Freeing pixbuf data", data);
        g_free (data);
}


gpointer                gdv_util_get_checkers (gint width, gint height)
{
        // Last time I was doing that was in the ms-dos QuickBasic times... 
        // But let's see if I still remember
        // The objective is to create a rgb buffer with a nice checkered pattern
        
        int x,y,ch;
        int iterator = 0;
        int mod;
        
        unsigned char *data = g_malloc (width * height * 3);
        unsigned char col;
        
        for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                        
                        mod = (y / 4) % 2; 
                        if (((x / 4) + mod) % 2 == 0) 
                                col = 230;
                        else 
                                col = 255;
                        
                        for (ch = 0; ch < 3; ch++) {
                                                        
                                data[iterator] = col;        
                                iterator++;
                        }

                }
        }                

        // Create the pixbuf
        gpointer buf = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB, FALSE,
                                                 8, width, height, 
                                                 width * 3, gdv_util_pixbuf_destroyfunc, NULL);
        
        return buf;
}

// FIXME: Unify that and above into ONE function
gpointer                gdv_util_get_checkers_gros (gint width, gint height)
{
        int x,y,ch;
        int iterator = 0;
        int mod;
        
        unsigned char *data = g_malloc (width * height * 3);
        unsigned char col;
        
        for (y = 0; y < height; y++) {
                for (x = 0; x < width; x++) {
                        
                        mod = (y / 16) % 2; 
                        if (((x / 16) + mod) % 2 == 0) 
                                col = 230;
                        else 
                                col = 255;
                        
                        for (ch = 0; ch < 3; ch++) {
                                                        
                                data[iterator] = col;        
                                iterator++;
                        }

                }
        }                

        // Create the pixbuf
        gpointer buf = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB, FALSE,
                                                 8, width, height, 
                                                 width * 3, gdv_util_pixbuf_destroyfunc, NULL);
        
        return buf;
}


/* Get a in-ratio width value for a given height */
int                     gdv_util_scale_width (int inputw, int inputh, int outputh)
{
        double ratio = (double) inputw / (double) inputh;
        return ratio * outputh;
}

