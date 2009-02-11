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

#define INPUT_LOCK(obj)         (g_mutex_lock(obj->Private->InputMutex))

#define INPUT_UNLOCK(obj)       (g_mutex_unlock(obj->Private->InputMutex))

struct                          _GdvCompSinkPrivate
{
        gint32 PadCookie;          // The ID of the pad. Not really number of pads
        gint32 PadCount;           // The number of sink pads
        GstCaps *MasterCaps;       // Caps for output
        GstPad *SrcPad;            // Our output pad

        GstClockTime Now;          // Next buffer's time
                
        GSList *Inputs;            // Our inputs
        GCond *Cond;               // Condition
        GMutex *InputMutex;        // Protects us

        gboolean Started;          // If we're to process data
        gboolean NeedsSegment;     // If we need to send a new segment event

};

static gboolean                 private_init (GdvCompSink *this, GdvCompSinkPrivate *priv);

static void                     private_dispose (GdvCompSink *this, GdvCompSinkPrivate *priv);

static GstPad*                  request_new_pad (GdvCompSink *this, GstPadTemplate *template,
                                                 const gchar *name);

static GstStateChangeReturn     change_state (GdvCompSink *this, GstStateChange transition);

static GstFlowReturn            sinkpad_chain_func (GstPad *pad, GstBuffer *buffer);

static gboolean                 sinkpad_event_func (GstPad *pad, GstEvent *event);

static GstCaps*                 sinkpad_get_caps_function (GstPad *pad);

static GstCaps*                 srcpad_get_caps_function (GstPad *pad);

static gboolean                 srcpad_event_func (GstPad *pad, GstEvent *event);

static void                     gdv_compsink_class_init (GdvCompSinkClass *klass);

static void                     gdv_compsink_init (GdvCompSink *this);

static void                     start_pads (GdvCompSink *this);

static void                     stop_pads (GdvCompSink *this);

static gboolean                 push_new_segment (GdvCompSink *this, GstPad *pad, GstBuffer *buffer);

static gboolean                 block_on_flush_cookie (GdvCompSink *this);

static void                     reset_flush_cookies (GdvCompSink *this);

static GstBuffer*               call_compose_func (GdvCompSink *this);

static void                     call_flush_func (GdvCompSink *this);

static gboolean                 call_new_pad_func (GdvCompSink *this, GdvCompSinkInput *input);

static gboolean                 call_chain_enter_func (GdvCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer);

static gboolean                 call_chain_leave_func (GdvCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer);

static void                     gdv_compsink_finalize (GdvCompSink *this);

static void                     gdv_compsink_dispose (GdvCompSink *this);

static void                     stock_flush_func (GdvCompSink *this, GSList *inputs);

static gboolean                 all_stacked (GdvCompSink *this);

