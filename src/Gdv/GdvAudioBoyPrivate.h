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

enum                         _ChannelStatus
{
        CHANNEL_STATUS_EMPTY,
        CHANNEL_STATUS_FILLED
} typedef ChannelStatus;

struct                          _GdvAudioBoyChannel
{
        GstBuffer *Buffer;
        gint32 Position;
        ChannelStatus Status;
        gboolean Nullish;
};

struct                          _GdvAudioBoyPrivate
{
        GMutex *Mutex;         // Op mutex
        GList *ChannelList;    // List of input channels
        gint32 ValidBuffers;   // Numbers of accumulated (added) valid buffers

        GstCaps *MasterCaps;   // The master caps we're rendering
        gint32 MasterRate;     // The master samplerate
        gint32 MasterDepth;    // Depth in bpp
        gint32 MasterChannels; // Number of channels
        gboolean MasterSigned; // Signed or unsigned audio

        gdouble Volume;        // Volume. 0.0 - 1.0
};

#define BOY_LOCK(obj)           (g_mutex_lock(obj->Private->Mutex))

#define BOY_UNLOCK(obj)         (g_mutex_unlock(obj->Private->Mutex))

/* If you uncomment this define, be sure to uncomment NOISY_SILENCE in GdvSilentSheep.
   Otherwise you'll get reallt nasty random data being played */
#define OPTIMIZE_NULL

static void                     gdv_audioboy_class_init (GdvAudioBoyClass *klass);

static void                     gdv_audioboy_init (GdvAudioBoy *this);

static void                     gdv_audioboy_dispose (GdvAudioBoy *this);

static void                     gdv_audioboy_finalize (GdvAudioBoy *this);

static gboolean                 private_init (GdvAudioBoyPrivate *private);

static void                     private_dispose (GdvAudioBoy *this, 
                                                 GdvAudioBoyPrivate *private);

static gboolean                 private_set (GdvAudioBoyPrivate *private, GstCaps *caps);

static void                     get_sizes (GdvAudioBoy *this, gint32 *min,
                                           gint32 *max);

static gpointer                 read_input (GdvAudioBoy *this, GdvAudioBoyChannel *channel,
                                            gint32 bytes);

static gint32                   flush (GdvAudioBoy *this);

static GstBuffer*               get_master (GdvAudioBoy *this, gint32 size);
