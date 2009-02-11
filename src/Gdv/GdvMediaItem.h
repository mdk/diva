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

#ifndef __GDVMEDIAITEM_H__
#define __GDVMEDIAITEM_H__
 
#include <gst/gst.h>
#include <libintl.h>
#include "GdvConst.h"
#include "GdvVideoFormat.h"
#include "GdvAudioFormat.h"
#include "GdvSource.h"
#include "GdvTypes.h"

struct _GdvMediaItem {
        GObject Parent;
        
        gchar *_Name;                  /* Name of this media item */
        gboolean _HasAudio;            /* If it has audio */
        gboolean _HasVideo;            /* If it has video */
        GdvVideoFormat *_VideoFormat;  /* The video format */
        GdvAudioFormat *_AudioFormat;  /* The audio format */
        
        GdvSource *_BaseSource;        /* For stock implementations of get_fresh_source and
                                        * some querying */
};

struct _GdvMediaItemClass {
        GObjectClass ParentClass;
        GdvMediaItemSourceFunc GetFreshSourceFunc;
};

GType                   gdv_mediaitem_get_type                  (void);

void                    gdv_mediaitem_class_init                (GdvMediaItemClass *klass);

void                    gdv_mediaitem_init                      (GdvMediaItem *this);

void                    *gdv_mediaitem_randomize_thumbnail      (GdvMediaItem *this);

void                    gdv_mediaitem_set_property              (GdvMediaItem *this, guint propid, 
                                                                 GValue *value, GParamSpec *pspec);

void                    gdv_mediaitem_get_property              (GdvMediaItem *this, guint propid,
                                                                 GValue *value, GParamSpec *pspec);

void                    gdv_mediaitem_dispose                   (GdvMediaItem *this);

GdvSource               *gdv_mediaitem_get_fresh_source         (GdvMediaItem *this);

GdvSource               *gdv_mediaitem_get_fresh_source_stock   (GdvMediaItem *this);

#endif
