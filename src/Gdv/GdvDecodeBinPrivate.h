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
        ARG_BASEFILE
};

struct                          _GdvDecodeBinPrivate
{
        GstElement *FileSrc;               // Filesrc GST element
        GstElement *DecodeBin;             // DecodeBin element
        gchar *FileName;                   // Filename of the underlying file
        GdvIdleTower *Tower;               // Tower for idle ops
        gboolean Running;                  // If we're >= PAUSED

        GstEvent *CachedEvent;             // The (seek) event we have chached
        gboolean SeekScheduled;            // If a seek was already scheduled for this startup
        
        GMutex *StartMutex;
        GCond *StartCond;
        gboolean PadsFinished;             // Protected by the mutex/cond
        gboolean SegmentsDone;             // If all the initial (to be discarded) new-segments were received
        gint32 SegmentsReceived;           // How many new-segment events were received

        GSList *QHandles;                  // QueueHandles to queue's we're watching
        GMutex *QMutex;                    // Queue's handling mutex

        GstBusSyncHandler OrigSyncHandler; // The original/parent's sync handler
        gpointer OrigSyncHandlerData;      // The original/parent's sync handler data

        GstMessage *CachedSegmentDone;     // The cached segment-done message we've got
        gboolean WaitingForExhaust;        // If we're waiting for a queue-exhaust
};

struct                          _BlockAction
{
        GdvDecodeBin *DecodeBin; // The parent decodebin
        GstPad *Pad;             // Pad this action applies to
        gboolean KillSwitch;     // Kill switch to prevent double-execution
        gboolean Seek;           // If we should perform a seek ops
        GMutex *Mutex;           // Protection mutex. Could be it's not needed
        
};

struct                          _QueueHandle
{
        GstElement *Queue;
        gboolean Exhausted;
} typedef QueueHandle;

#define ACTION_LOCK(obj)        (g_mutex_lock (obj->Mutex))

#define ACTION_UNLOCK(obj)      (g_mutex_unlock (obj->Mutex))

#define Q_LOCK(obj)             (g_mutex_lock (this->Private->QMutex))

#define Q_UNLOCK(obj)           (g_mutex_unlock (this->Private->QMutex))

static void                     gdv_decodebin_class_init (GdvDecodeBinClass *klass);

static void                     gdv_decodebin_init (GdvDecodeBin *this);

static void                     gdv_decodebin_get_property (GdvDecodeBin *this, guint propid,
                                                            GValue *value, GParamSpec *pspec);

static gboolean                 private_build (GdvDecodeBin *this, GdvDecodeBinPrivate *private, const gchar *filename);

static void                     gdv_decodebin_dispose (GdvDecodeBin *this);

static void                     gdv_decodebin_finalize (GdvDecodeBin *this);

static void                     new_pad (GstElement *element, GstPad *pad, GdvDecodeBin *this);

static void                     no_more_pads (GstElement *element, GdvDecodeBin *this);

static GdvDecodeBin*            clone (GdvDecodeBin *this);

static gboolean                 private_init (GdvDecodeBin *this, GdvDecodeBinPrivate *private);

static void                     private_dispose (GdvDecodeBin *this, GdvDecodeBinPrivate *private);

static GstStateChangeReturn     change_state (GdvDecodeBin *this, GstStateChange transition);

static gboolean                 send_event (GdvDecodeBin *this, GstEvent *event);

static BlockAction*             new_action (GdvDecodeBin *this, GstPad *pad);

static void                     pad_block (GstPad *pad, gboolean blocked, BlockAction *action);

static gboolean                 schedule_action_for_tower (GdvDecodeBin *this, BlockAction *action,
                                                           GdvIdleTowerActionFunc action_func);

static gboolean                 action_dispose_func (BlockAction *action);

static void                     action_finalize_func (BlockAction *action);

static void                     perform_seek_action_func (BlockAction *action);

static gboolean                 pad_event_probe (GstPad *pad, GstEvent *event, GdvDecodeBin *this);

static void                     scan_queues (GdvDecodeBin *this, GstBin *scantarget);

static gint                     queue_find_func (const QueueHandle *data, const GstElement *element);

static GstBusSyncReply          sync_handler (GstBus *bus, GstMessage *message,
                                              GdvDecodeBin *this);

static gboolean                 check_if_queues_exhausted (GdvDecodeBin *this, gboolean probe);

static void                     queue_underrun (GstElement *queue, GdvDecodeBin *this);
