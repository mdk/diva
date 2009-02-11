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

// FIXME: Add support for frameable

#ifndef __GDVPHOTOITEM_H__
#define __GDVPHOTOITEM_H__
 
#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvMediaItem.h"
#include "GdvSource.h"
#include "GdvThumbnailableInterface.h"
#include "GdvFileBasedInterface.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include "GdvProjectFormat.h"
#include "GdvImageSheep.h"
#include "GdvSheepSource.h"

struct _GdvPhotoItem {
        GdvMediaItem Parent;
        
        GdkPixbuf *_Thumbnail;            /* Thumbnail of this item */
        GdkPixbuf *_SmallThumbnail;       /* Small thumbnail of this item */        
        gchar *_Filename;                 //
        GdvProjectFormat *_ProjectFormat; //
};

struct _GdvPhotoItemClass {
        GdvMediaItemClass ParentClass;
};

GType                   gdv_photoitem_get_type                  (void);

void                    gdv_photoitem_class_init                (GdvPhotoItemClass *klass);

void                    gdv_photoitem_init                      (GdvPhotoItem *this);

void                    gdv_photoitem_get_property              (GdvPhotoItem *this, guint propid, 
                                                                 GValue *value, GParamSpec *pspec);

void                    gdv_photoitem_dispose                   (GdvPhotoItem *this);

GdvPhotoItem            *gdv_photoitem_new                      (GdvProjectFormat *format, const gchar *filename);

GdvSource               *gdv_photoitem_get_fresh_source         (GdvPhotoItem *this);

#endif
