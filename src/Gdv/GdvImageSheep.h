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

#ifndef __GDVIMAGESHEEP_H__
#define __GDVIMAGESHEEP_H__

#include <gst/gst.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

#include "GdvTypes.h"
#include "GdvTimeSpan.h"
#include "GdvTime.h"
#include "GdvSheep.h"
#include "GdvFraction.h"
#include "GdvProjectFormat.h"
#include "GdvBufferUtil.h"
#include "GdvFileBasedInterface.h"

typedef struct                  _GdvImageSheepPrivate GdvImageSheepPrivate;

struct                          _GdvImageSheep 
{
        GdvSheep Parent;
        GdvImageSheepPrivate* Private;
};

struct                          _GdvImageSheepClass 
{
        GdvSheepClass ParentClass;
};

GType                           gdv_imagesheep_get_type (void);

GdvImageSheep*                  gdv_imagesheep_new (GdvProjectFormat *format, gchar *filename);

#endif

