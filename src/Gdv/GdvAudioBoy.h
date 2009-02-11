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

#ifndef __GDVAUDIOBOY_H__
#define __GDVAUDIOBOY_H__

#include <gst/gst.h>

#include "GdvTypes.h"
#include "GdvConst.h"
#include "GdvFraction.h"
#include "GdvFrameDimensions.h"
#include "GdvAudioBuffer.h"
#include "GdvBufferUtil.h"
#include <string.h>

typedef struct                  _GdvAudioBoyPrivate GdvAudioBoyPrivate;

typedef struct                  _GdvAudioBoyChannel GdvAudioBoyChannel;

struct                          _GdvAudioBoy
{
        GObject Parent;
        GdvAudioBoyPrivate *Private;
};

struct                          _GdvAudioBoyClass
{
        GObjectClass ParentClass;
};

GType                           gdv_audioboy_get_type (void);

GdvAudioBoy*                    gdv_audioboy_new (GstCaps *caps);

gboolean                        gdv_audioboy_channel_is_empty (GdvAudioBoy *this, GdvAudioBoyChannel *channel);

gboolean                        gdv_audioboy_channel_flush (GdvAudioBoy *this, GdvAudioBoyChannel *channel);

GdvAudioBoyChannel*             gdv_audioboy_add_channel (GdvAudioBoy *this);

void                            gdv_audioboy_channel_add_buffer (GdvAudioBoy *this, GdvAudioBoyChannel *channel,
                                                                 GstBuffer *buffer);

GstBuffer*                      gdv_audioboy_get_buffer (GdvAudioBoy *this);

gboolean                        gdv_audioboy_all_filled (GdvAudioBoy *this);

void                            gdv_audioboy_set_volume (GdvAudioBoy *this, gdouble volume);

gdouble                         gdv_audioboy_get_volume (GdvAudioBoy *this);

#endif
