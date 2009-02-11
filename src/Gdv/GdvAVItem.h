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

#ifndef __GDVAVITEM_H__
#define __GDVAVITEM_H__
 
#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvMediaItem.h"
#include "GdvSource.h"
#include "GdvFrameRunner.h"
#include "GdvFrameableInterface.h"
#include "GdvThumbnailableInterface.h"
#include "GdvTimeableInterface.h"

struct _GdvAVItem {
        GdvMediaItem Parent;
        
        GdvFrameRunner *_Runner;       /* Runner to generate frame previews */
        GdkPixbuf *_Thumbnail;         /* Thumbnail of this item */
        GdkPixbuf *_SmallThumbnail;    /* Small thumbnail of this item */        
};

struct _GdvAVItemClass {
        GdvMediaItemClass ParentClass;
};

GType                   gdv_avitem_get_type                     (void);

void                    gdv_avitem_class_init                   (GdvAVItemClass *klass);

void                    gdv_avitem_init                         (GdvAVItem *this);

void                    gdv_avitem_get_property                 (GdvAVItem *this, guint propid, 
                                                                 GValue *value, GParamSpec *pspec);

void                    gdv_avitem_set_property                 (GdvAVItem *this, guint propid, 
                                                                 const GValue *value, GParamSpec *pspec);

void                    gdv_avitem_dispose                      (GdvAVItem *this);

void                    gdv_avitem_frameable_init               (GdvFrameableInterface *interface, gpointer data);

GdvToken                gdv_avitem_frameable_get_frame          (GdvAVItem *this, GdvTime attime,
                                                                 GdvFrameDimensions *dim, GdvToken *settoken,
                                                                 GSourceFunc callback);

GdvAVItem               *gdv_avitem_new                         (GdvSource *source);

#endif
