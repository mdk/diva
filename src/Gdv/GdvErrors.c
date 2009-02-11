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

#include "GdvErrors.h"

/* A handy function */
void                    gdv_errors_set_error (GdvError code, GError **error)
{
        if (error != NULL)
                g_set_error (error, GDV_ERROR_DOMAIN, code, gdv_errors_get_msg (code));
}

/* Return a pointer to the GdvError message */
gchar                   *gdv_errors_get_msg (GdvError code)
{
        switch (code) {
                
                case GDV_ERROR_NO_STREAM:
                return gettext ("No valid audio/video streams were found in "
                                "the media file.");
                break;
                
                case GDV_ERROR_NO_VIDEO_NO_AUDIO:
                return gettext ("This media doesn't contain video nor audio.");
                break;
                
                case GDV_ERROR_GOBJECT:
                return gettext ("Internal GObject error. Please file a bug.");
                break;
                
                case GDV_ERROR_GST_ELEMENTS:
                return gettext ("Internal GStreamer error - one of the stock elements "
                                "could not be created. Check your GStreamer instalation.");
                break;
                
                case GDV_ERROR_GST_LINKING:
                return gettext ("Internal GStreamer error during linking. Please file bug.");
                break;
                
                case GDV_ERROR_BAD_LENGTH:
                return gettext ("The duration of the media stream could not be determined.");
                break;
                
                case GDV_ERROR_STATE:
                return gettext ("The element is in an unexpected state.");
                break;
                
                default:
                return gettext ("Unknown internal error. Please file a bug");
                break;
        }
}
