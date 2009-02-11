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

#ifndef __GDVCLIP_H__
#define __GDVCLIP_H__
 
#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvTime.h"
#include "GdvTimeSpan.h"
#include "GdvSource.h"
#include "GdvMediaItem.h"
#include "GdvTimeableInterface.h"
#include "GdvClipGate.h"
#include "GdvTrack.h"

typedef struct                  _GdvClipPrivate GdvClipPrivate;

struct                          _GdvClip
{
        GstBin Parent;
        GdvClipPrivate *Private;
};

struct                          _GdvClipClass
{
        GstBinClass ParentClass;
        guint ChangedSignalId;
};

GType                           gdv_clip_get_type (void);

gboolean                        gdv_clip_place_at_timespan (GdvClip *this, GdvTimeSpan *timespan);

gboolean                        gdv_clip_place_at_time (GdvClip *this, GdvTime time);

GdvClip*                        gdv_clip_new (GdvMediaItem *item);

GdvClip*                        gdv_clip_new_explicit (GdvSource *source, GdvMediaItem *item);

GdvTime                         gdv_clip_time_to_source_time (GdvClip *this, GdvTime cliptime);

GdvTime                         gdv_clip_source_time_to_time (GdvClip *this, GdvTime sourcetime);

gboolean                        gdv_clip_move_to (GdvClip *this, GdvTime time);

gboolean                        gdv_clip_left_adjust_to (GdvClip *this, GdvTime newtime);

gboolean                        gdv_clip_right_adjust_to (GdvClip *this, GdvTime newtime);

void                            gdv_clip_force_changed_signal (GdvClip *this);

#endif
