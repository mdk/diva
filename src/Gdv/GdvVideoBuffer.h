///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Senko Rasic <senko@senko.net>                                        //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//                                                                           //
// Permission is hereby granted, free of charge, to any person obtaining a   //
// copy of this software and associated documentation files (the             //
// "Software"), to deal in the Software without restriction, including       //
// without limitation the rights to use, copy, modify, merge, publish,       //
// distribute, sublicense, and/or sell copies of the Software, and to permit //
// persons to whom the Software is furnished to do so, subject to the        //
// following conditions:                                                     //
//                                                                           //
// The above copyright notice and this permission notice shall be included   //
// in all copies or substantial portions of the Software.                    //
//                                                                           //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#ifndef __GDVVIDEOBUFFER_H__
#define __GDVVIDEOBUFFER_H__

#include <glib.h>
#include <gst/gst.h>

#include "GdvTypes.h"
#include "GdvConst.h"

// FIXME: this should be gdv_videobuffer_new_converted or something
GdvVideoBuffer*                 gdv_videobuffer_new (GdvVideoBuffer *buffer, guint32 fourcc_to,
                                                     int ow, int oh);

GdvVideoBuffer*                 gdv_videobuffer_new_with_caps (GdvVideoBuffer *buffer, GstCaps *caps);

GdvVideoBuffer*                 gdv_videobuffer_new_from_gst (GstBuffer *buffer);

void                            gdv_videobuffer_meta_set_opacity (GdvVideoBuffer *this, gdouble opacity);

gdouble                         gdv_videobuffer_meta_get_opacity (GdvVideoBuffer *this);

gboolean                        gdv_videobuffer_meta_has_opacity (GdvVideoBuffer *this);

void                            gdv_videobuffer_meta_set_null (GdvVideoBuffer *this, gboolean null);

gboolean                        gdv_videobuffer_meta_get_null (GdvVideoBuffer *this);

gboolean                        gdv_videobuffer_meta_has_null (GdvVideoBuffer *this);

void                            gdv_videobuffer_meta_set_layer (GdvVideoBuffer *this, gint32 layer);

gint32                          gdv_videobuffer_meta_get_layer (GdvVideoBuffer *this);

gboolean                        gdv_videobuffer_meta_has_layer (GdvVideoBuffer *this);

gboolean                        gdv_videobuffer_is (gpointer this);

void                            gdv_videobuffer_start_editing (GdvVideoBuffer *this);

#endif


