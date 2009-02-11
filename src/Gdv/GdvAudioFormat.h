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

#ifndef __GDVAUDIOFORMAT_H__
#define __GDVAUDIOFORMAT_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"

typedef struct                  _GdvAudioFormatPrivate GdvAudioFormatPrivate;

struct                          _GdvAudioFormat
{
        GObject Parent;
        GdvAudioFormatPrivate *Private;
};

struct _GdvAudioFormatClass {
        GObjectClass ParentClass;
};

GType                           gdv_audioformat_get_type (void);

GdvAudioFormat*                 gdv_audioformat_new (GstCaps *caps);

GdvAudioFormat*                 gdv_audioformat_new_from_string (const gchar *string);

GdvAudioFormat*                 gdv_audioformat_new_default (void);

gchar*                          gdv_audioformat_serialize_to_string (GdvAudioFormat *this);

gboolean                        gdv_audioformat_caps_ok (GstCaps *caps);

GdvAudioFormat*                 gdv_audioformat_clone (GdvAudioFormat *this);

#endif
