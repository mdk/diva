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
 
#ifndef __GDVCOMPSINK__
#define __GDVCOMPSINK__
 
#include <gst/gst.h>

#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvTime.h"
#include "GdvTimeSpan.h"
#include "GdvFraction.h"
#include "GdvProjectFormat.h"

typedef struct                  _GdvCompSinkPrivate GdvCompSinkPrivate;

typedef struct                  _GdvCompSinkInput GdvCompSinkInput;

typedef void                    (*GdvCompSinkFlushFunc) (GdvCompSink *this, GSList *inputs);

typedef gboolean                (*GdvCompSinkNewPadFunc) (GdvCompSink *this, GdvCompSinkInput *input);

typedef GstBuffer*              (*GdvCompSinkComposeFunc) (GdvCompSink *this, GSList *inputs, GdvTime now);

typedef gboolean                (*GdvCompSinkChainEnterFunc) (GdvCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer);

typedef gboolean                (*GdvCompSinkChainLeaveFunc) (GdvCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer);

struct                          _GdvCompSink 
{
        GstElement Parent;
        GdvCompSinkPrivate *Private;
};

struct                          _GdvCompSinkClass 
{
        GstElementClass ParentClass;
        GdvCompSinkFlushFunc FlushFunc;
        GdvCompSinkComposeFunc ComposeFunc;
        GdvCompSinkNewPadFunc NewPadFunc;
        GdvCompSinkChainEnterFunc ChainEnterFunc;
        GdvCompSinkChainLeaveFunc ChainLeaveFunc;        
};

struct                          _GdvCompSinkInput
{
        GstPad *Pad;        // The pad of the input channel
        GstBuffer *Buffer;  // Current buffer on the stack
        gboolean Flushing;  // If we're flushing
        gint32 FlushCookie; // Number of flushes got
        gpointer Private;   // Private pointer
        GstCaps *LastCaps;  // Last caps seen on this buffer       
};

GType                           gdv_compsink_get_type (void);

void                            gdv_compsink_input_set_private (GdvCompSinkInput *input, gpointer private);

gpointer                        gdv_compsink_input_get_private (GdvCompSinkInput *input);

gboolean                        gdv_compsink_set_master_caps (GdvCompSink *this, GstCaps *caps);

#endif
