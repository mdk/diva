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

enum
{
        ARG_0,
        ARG_PARENTITEM,
        ARG_SOURCE,
        ARG_TIMELINESPAN,
        ARG_SOURCESPAN,
        ARG_MINTIMELINEIN,
        ARG_MAXTIMELINEOUT,
        ARG_TIMELINEIN,
        ARG_TIMELINEOUT,
        ARG_HASAUDIO,
        ARG_HASVIDEO,
        ARG_PARENTTRACK,        
};

struct                          _GdvClipPrivate
{
        GdvSource *Source;                 // The source element
        GdvMediaItem *ParentItem;          // The parent item
        
        GstPad *VSrc;                      // The vsrc pad
        GstPad *ASrc;                      // The asrc pad
        
        GdvTimeSpan *TimelineSpan;         // The timespan of this clip on the timeline
        GdvTimeSpan *SourceSpan;           // The source timespan

        GdvClipGate *VGate;                // Video gate
        GdvClipGate *AGate;                // Audio gate
        
        GstBusSyncHandler OrigSyncHandler; // The original/parent's sync handler
        gpointer OrigSyncHandlerData;      // The original/parent's sync handler data

        gboolean Timeable;                 // If the source has length
};

#define                         GDV_CLIP_GET_CLASS(obj) ((GdvClipClass *) G_OBJECT_GET_CLASS(obj))

static void                     gdv_clip_class_init (GdvClipClass *klass);

static void                     gdv_clip_init (GdvClip *this);

static void                     gdv_clip_get_property (GdvClip *this, guint propid,
                                                       GValue *value, GParamSpec *pspec);

static void                     gdv_clip_set_property (GdvClip *this, guint propid, 
                                                       GValue *value, GParamSpec *pspec);

static void                     gdv_clip_dispose (GdvClip *this);

static void                     gdv_clip_finalize (GdvClip *this);

static GstBusSyncReply          sync_handler (GstBus *bus, GstMessage *message,
                                              GdvClip *this);

static GstMessage*              translate_segment_done (GdvClip *this, GstMessage *message);

static gboolean                 send_event (GdvClip *this, GstEvent *event);

static gboolean                 private_init (GdvClip *this, GdvClipPrivate *private);

static GstStateChangeReturn     change_state (GdvClip *this, GstStateChange transition);

static GstEvent*                get_full_seek_event (GdvClip *this);

static void                     update_gates (GdvClip *this);

