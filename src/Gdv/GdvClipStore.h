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

#ifndef __GDVCLIPSTORE_H__
#define __GDVCLIPSTORE_H__

#include <gst/gst.h>

#include "GdvTypes.h"
#include "GdvTimeSpan.h"
#include "GdvTime.h"
#include "GdvClip.h"

typedef struct                  _GdvClipStorePrivate GdvClipStorePrivate;

struct                          _GdvClipStore
{
        GObject Parent;
        GdvClipStorePrivate *Private;
};

struct                          _GdvClipStoreClass
{
        GObjectClass ParentClass;
};

GType                           gdv_clipstore_get_type (void);

GdvClipStore*                   gdv_clipstore_new (void);

gboolean                        gdv_clipstore_is_span_empty (GdvClipStore *this,
                                                             GdvTimeSpan *span);

gboolean                        gdv_clipstore_is_span_empty_excluding (GdvClipStore *this,
                                                                       GdvTimeSpan *span, GdvClip *clip);

gboolean                        gdv_clipstore_add_clip (GdvClipStore *this, 
                                                        GdvClip *clip);

gboolean                        gdv_clipstore_remove_clip (GdvClipStore *this, 
                                                           GdvClip *clip);

GdvClip*                        gdv_clipstore_get_clip_at_time (GdvClipStore *this, 
                                                                GdvTime time);

GdvClip*                        gdv_clipstore_get_clip_after_time (GdvClipStore *this, 
                                                                   GdvTime time);

GdvClip*                        gdv_clipstore_get_clip_before_time (GdvClipStore *this, 
                                                                    GdvTime time);

GdvClip*                        gdv_clipstore_get_clip_before_clip (GdvClipStore *this, 
                                                                    GdvClip *clip);

GdvClip*                        gdv_clipstore_get_clip_after_clip (GdvClipStore *this, 
                                                                   GdvClip *clip);

gboolean                        gdv_clipstore_suggest_position (GdvClipStore *this,
                                                                GdvTimeSpan *span, GdvTimeSpan *out,
                                                                GdvTime cursor);

gboolean                        gdv_clipstore_suggest_position_excluding (GdvClipStore *this,
                                                                          GdvTimeSpan *span, GdvTimeSpan *out,
                                                                          GdvTime cursor, GdvClip *exclude);

#endif
