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
 
#ifndef __GDVPIXBUFSINK_H__
#define __GDVPIXBUFSINK_H__
 
#include <gst/gst.h>
#include <gst/base/gstbasesink.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvFrameDimensions.h"
#include "GdvUtil.h"
#include "GdvBufferUtil.h"
#include <gdk-pixbuf/gdk-pixbuf.h>


struct _GdvPixbufSink {
        GstBaseSink Parent;
        
        GMutex *_PropMutex;                      /* Mutex locked when properties are accessed/used */
        GMutex *_CondMutex;                      /* Mutex for wait conditions */
        GCond *_Cond;                            /* Condition for signalling */
        GdkPixbuf *_Pixbuf;                      /* Last pixbuf rendered */
        GdvFrameDimensions *_FrameDimensions;    /* Requested output size */
};

struct _GdvPixbufSinkClass {
        GstBaseSinkClass ParentClass;
};

GType                   gdv_pixbufsink_get_type         (void);

void                    gdv_pixbufsink_class_init       (GdvPixbufSinkClass *klass);

void                    gdv_pixbufsink_init             (GdvPixbufSink *this);

GdvPixbufSink           *gdv_pixbufsink_new             (void);

GstFlowReturn           gdv_pixbufsink_preroll          (GdvPixbufSink *this, GstBuffer *buffer);

void                    gdv_pixbufsink_set_property     (GdvPixbufSink *this, guint propid, 
                                                         const GValue *value, GParamSpec *pspec);

void                    gdv_pixbufsink_get_property     (GdvPixbufSink *this, guint propid, 
                                                         GValue *value, GParamSpec *pspec);

void                    gdv_pixbufsink_dispose          (GdvPixbufSink *this);

void                    gdv_pixbufsink_lock             (GdvPixbufSink *this);

void                    gdv_pixbufsink_wait             (GdvPixbufSink *this);

GstStateChangeReturn    gdv_pixbufsink_change_state     (GdvPixbufSink *this, 
                                                         GstStateChange transition);

#endif
