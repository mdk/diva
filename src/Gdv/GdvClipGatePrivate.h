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

struct                          _GdvClipGatePrivate
{
        GdvTime SourceIn;
        GdvTime TimelineIn;                        
        gboolean Killer;

        GstPadEventFunction SinkOrigEventFunction; // The original event function of the sink pad
        
        GMutex *Mutex;                             // Stream mutex
        GdvFlavor Flavor;                          // Flavour (audio/video)
};

#define                         GATE_LOCK(obj) g_mutex_lock (this->Private->Mutex)

#define                         GATE_UNLOCK(obj) g_mutex_unlock (this->Private->Mutex)

static void                     gdv_clipgate_base_init (gpointer klass);

static void                     gdv_clipgate_class_init (GdvClipGateClass *klass);

static void                     gdv_clipgate_init (GdvClipGate *this);

static void                     gdv_clipgate_dispose (GdvClipGate *this);

static void                     gdv_clipgate_finalize (GdvClipGate *this);

static GstFlowReturn            transform_ip (GdvClipGate *this, GstBuffer *buf);

static GstFlowReturn            prepare_output_buffer (GdvClipGate *this, GstBuffer *input,
                                                       gint size, GstCaps *caps, GstBuffer **buf);

static gboolean                 sink_pad_event_func (GstPad *pad, GstEvent *event);

static gboolean                 private_init (GdvClipGate *this, GdvClipGatePrivate *private);

static void                     private_dispose (GdvClipGatePrivate *private);

static GstStateChangeReturn     change_state (GdvClipGate *this, GstStateChange transition);

