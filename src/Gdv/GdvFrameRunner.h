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

#ifndef __GDVFRAMERUNNER_H__
#define __GDVFRAMERUNNER_H__
 
#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvSource.h"
#include "GdvPixbufSink.h"
#include "GdvToken.h"
#include "GdvJob.h"
#include "GdvFrameRunnerJobData.h"
#include "GdvVideoFormat.h"
#include "GdvJobProcessor.h"
#include <gdk-pixbuf/gdk-pixbuf.h>


struct _GdvFrameRunner {
        GstBin Parent;

        GdvSource *_Source;            /* Source used for rendering */
        GdvPixbufSink *_PixbufSink;    /* The sink to generate pixbufs */
};

struct _GdvFrameRunnerClass {
        GstBinClass ParentClass;
};

GType                   gdv_framerunner_get_type        (void);

void                    gdv_framerunner_class_init      (GdvFrameRunnerClass *klass);

void                    gdv_framerunner_init            (GdvFrameRunner *this);

GdvFrameRunner          *gdv_framerunner_new            (GdvSource *source);

gboolean                gdv_framerunner_job_work_func   (GdvFrameRunnerJobData *data);

GdvToken                gdv_framerunner_job_schedule    (GdvFrameRunner *this, GdvTime attime, 
                                                         GdvFrameDimensions *dim, GdvToken *settoken, 
                                                         GSourceFunc callback);

GdkPixbuf               *gdv_framerunner_get_pixbuf     (GdvFrameRunner *this);

#endif
