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

#ifndef __GDVERRORS_H__
#define __GDVERRORS_H__

// FIXME: GObject is all we need
#include <libintl.h>
#include <gst/gst.h>
#include "GdvTypes.h"
#include "GdvConst.h"

enum _GdvError {
        GDV_ERROR_NULL,
        GDV_ERROR_NO_STREAM,
        GDV_ERROR_NO_VIDEO_NO_AUDIO,
        GDV_ERROR_GOBJECT,
        GDV_ERROR_GST_ELEMENTS,
        GDV_ERROR_GST_LINKING,
        GDV_ERROR_BAD_LENGTH,
        GDV_ERROR_STATE,
};

void                    gdv_errors_set_error            (GdvError code, GError **error);

gchar                   *gdv_errors_get_msg             (GdvError code);

#endif
