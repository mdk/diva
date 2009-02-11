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

#ifndef __GDVAUDIOBUFFER_H__
#define __GDVAUDIOBUFFER_H__

#include <glib.h>
#include <gst/gst.h>

#include "GdvTypes.h"
#include "GdvConst.h"

GdvAudioBuffer*                 gdv_audiobuffer_new_from_gst (GstBuffer *buffer);

gboolean                        gdv_audiobuffer_is (gpointer this);

void                            gdv_audiobuffer_meta_set_null (GdvAudioBuffer *this, gboolean null);

gboolean                        gdv_audiobuffer_meta_get_null (GdvAudioBuffer *this);

gboolean                        gdv_audiobuffer_meta_has_null (GdvAudioBuffer *this);

GstBuffer*                      gdv_audiobuffer_resample_16_2 (GstBuffer *buffer, gint32 targetrate);

void                            gdv_audiobuffer_start_editing (GdvAudioBuffer *this);

void                            gdv_audiobuffer_mix_16_signed (gpointer a, gpointer b, gpointer c,
                                                               gint32 size, gdouble volume);

void                            gdv_audiobuffer_mix_16_unsigned (gpointer a, gpointer b, gpointer c,
                                                                 gint32 size, gdouble volume);

void                            gdv_audiobuffer_mix_8_signed (gpointer a, gpointer b, gpointer c,
                                                              gint32 size, gdouble volume);

void                            gdv_audiobuffer_mix_8_unsigned (gpointer a, gpointer b, gpointer c,
                                                                gint32 size, gdouble volume);

#endif


