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

#define                         GDV_TRACK_GET_CLASS(obj) ((GdvTrackClass *) G_OBJECT_GET_CLASS(obj))

struct                          _ClipData
{
        GdvClip *Clip;
        guint ChangedSignal;
        
} typedef ClipData;

struct                          _GdvTrackPrivate
{
        GdvClipStore *ClipStore; // Clips
        GSList *Clips;           // A list of ClipData
        
        GstBusSyncHandler ParentSyncHandler;
        gpointer ParentSyncHandlerData;

        GstElement *Blackness;
        GstElement *Silence;

        GstElement *CurrentVElement;
        GstElement *CurrentAElement;
        
        GstPad *VSrc;
        GstPad *ASrc;

        GstElement *VCorrector;
        GstElement *ACorrector;

        GstElement *AQueue;
        GstElement *VQueue;
                
        GdvProjectFormat *ProjectFormat;
        gint32 Layer; 

        GMutex *SegmentMutex;
        gboolean FirstDone;

        GstElement *AudioConvert; 
        GstElement *AudioResample; 

        GdvTime NextCutAt;
        GdvTime SavedSeek;

        GdvIdleTower *Tower;
};

struct                          _SegmentDoneAction
{
        GdvTrack *Track;
        GdvTime Time;
} typedef SegmentDoneAction;

struct                          _SeekAction
{
        GdvTrack *Track;
        GdvTime Time;
} typedef SeekAction;

struct                          _PostRemoveAction
{
        GdvTrack *Track;
        GdvClip *Clip;
} typedef PostRemoveAction;

enum                            _GdvTrackPlayback
{
        GDV_TRACK_PLAYBACK_UNKNOWN,
        GDV_TRACK_PLAYBACK_BLACK_SILENCE,
        GDV_TRACK_PLAYBACK_CLIP_SILENCE,
        GDV_TRACK_PLAYBACK_BLACK_CLIP,
        GDV_TRACK_PLAYBACK_CLIP_CLIP
} typedef GdvTrackPlayback;

enum
{
        ARG_0,
        ARG_CLIPSTORE,        
        ARG_PROJECTFORMAT,
        ARG_CLIPS,
        ARG_LAYER,
        ARG_OPACITY,
};

#define                         SEEK_ACTION_GID 1

#define                         SEGMENT_DONE_ACTION_GID 2

#define                         POST_REMOVE_ACTION_GID 3

static gboolean                 private_init (GdvTrack *this, GdvTrackPrivate *private);

static void                     private_dispose (GdvTrack *this, GdvTrackPrivate *private);

static void                     gdv_track_class_init (GdvTrackClass *klass);

static void                     gdv_track_get_property (GdvTrack *this, guint propid,
                                                        GValue *value, GParamSpec *pspec);

static void                     gdv_track_set_property (GdvTrack *this, guint propid, 
                                                        GValue *value, GParamSpec *pspec);

static void                     gdv_track_init (GdvTrack *this);

static void                     gdv_track_dispose (GdvTrack *this);

static void                     gdv_track_finalize (GdvTrack *this);

static GstBusSyncReply          sync_handler (GstBus *bus, GstMessage *message,
                                              GdvTrack *track);

static GstStateChangeReturn     change_state (GdvTrack *this, GstStateChange transition);

gboolean                        send_event (GdvTrack *this, GstEvent *event);

static gboolean                 do_relink (GdvTrack *this, GdvTrackPlayback mode, GdvClip *clip,
                                           gboolean *vrelinked, gboolean *arelinked);

static gboolean                 check_relink (GdvTrack *this, GdvTrackPlayback mode, GdvClip *clip,
                                              gboolean *vrelinked, gboolean *arelinked);

static GdvTrackPlayback         get_mode (GdvTrack *this, GdvClip **clip, GdvTime time);

static gboolean                 get_seek_span (GdvTrack *this, GdvTrackPlayback mode, GdvClip *curclip,
                                               gint64 *start, gint64 *stop, GdvTime time);

static gchar*                   playback_mode_to_string (GdvTrackPlayback mode);

static void                     on_clip_changed (gpointer a, GdvTrack *this);

static void                     spawn_sheeps (GdvTrack *this, GdvProjectFormat *format);

static int                      clip_find_func (ClipData *a, GdvClip *clip);

static gboolean                 do_initial_setup (GdvTrack *this);

static void                     segment_done_action (SegmentDoneAction *action);

static gboolean                 segment_done_action_dispose (SegmentDoneAction *action);

static void                     seek_action (SeekAction *action);

static gboolean                 seek_action_dispose (SeekAction *action);

static gboolean                 src_event_probe (GstPad *pad, GstEvent *event, GdvTrack *this);

static void                     post_remove_action (PostRemoveAction *action);

static gboolean                 post_remove_action_dispose (PostRemoveAction *action);

