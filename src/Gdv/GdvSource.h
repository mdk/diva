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

#ifndef __GDVSOURCE_H__
#define __GDVSOURCE_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvFrameDimensions.h"
#include "GdvSourcePad.h"
#include "GdvTimeSpan.h"

typedef struct                  _GdvSourcePrivate GdvSourcePrivate;

typedef struct                  _GdvSourceProtected GdvSourceProtected;

struct                          _GdvSource
{
        GstBin Parent;

        GdvSourcePrivate *Private;
        GdvSourceProtected *Protected;
};

struct                          _GdvSourceClass
{
        GstBinClass ParentClass;
        GdvSourceCloneFunc CloneFunc;
};

GType                           gdv_source_get_type (void);

gboolean                        gdv_source_link_to_sinks (GdvSource *this, GstElement *vsink, 
                                                          GstElement *asink);

gboolean                        gdv_source_unlink (GdvSource *this);

GdvSource*                      gdv_source_clone (GdvSource *this);

void                            gdv_source_get_thumb_dimensions (GdvSource *this, GdvFrameDimensions *dimensions);

void                            gdv_source_get_small_thumb_dimensions (GdvSource *this, GdvFrameDimensions *dimensions);

#endif
