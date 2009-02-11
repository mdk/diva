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

#ifndef __GDVVIDEOBOY_H__
#define __GDVVIDEOBOY_H__

#include <gst/gst.h>

#include "GdvTypes.h"
#include "GdvConst.h"
#include "GdvFraction.h"
#include "GdvFrameDimensions.h"
#include "GdvVideoBuffer.h"
#include "GdvBufferUtil.h"

typedef struct                  _GdvVideoBoyPrivate GdvVideoBoyPrivate;

struct                          _GdvVideoBoy
{
        GObject Parent;
        GdvVideoBoyPrivate *Private;
};

struct                          _GdvVideoBoyClass
{
        GObjectClass ParentClass;
};

GType                           gdv_videoboy_get_type (void);

GdvVideoBoy*                    gdv_videoboy_new (GstPad *pad, GstCaps *caps, gboolean padalloc);

void                            gdv_videoboy_start_composing (GdvVideoBoy *this);

void                            gdv_videoboy_abort_composing (GdvVideoBoy *this);

gboolean                        gdv_videoboy_add_buffer (GdvVideoBoy *this, GstBuffer *buffer);

GstBuffer*                      gdv_videoboy_get_rendered_buffer (GdvVideoBoy *this);

#endif
