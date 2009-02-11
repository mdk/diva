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

#define INPUT_LOCK(obj)         (g_mutex_lock (obj->Private->InputMutex))

#define INPUT_UNLOCK(obj)       (g_mutex_unlock (obj->Private->InputMutex))

#define AUDIO_CAPS              GST_STATIC_CAPS ("audio/x-raw-int;" \
                                                 "video/x-raw-float")
                                                 
#define MIN_BUFFER_SIZE         1024

struct                          _GdvAudioCompSinkPrivate
{
        GdvAudioBoy *AudioBoy; // Composer
        GSList *PartialBuffers;
};

static gboolean                 private_init (GdvAudioCompSink *this, GdvAudioCompSinkPrivate *priv);

static gboolean                 private_dispose (GdvAudioCompSink *this, GdvAudioCompSinkPrivate *priv);

static void                     gdv_audiocompsink_base_init (gpointer klass);

static void                     gdv_audiocompsink_class_init (GdvAudioCompSinkClass *klass);

static void                     gdv_audiocompsink_init (GdvAudioCompSink *this);

static void                     gdv_audiocompsink_dispose (GdvAudioCompSink *this);

static void                     gdv_audiocompsink_finalize (GdvAudioCompSink *this);

static GstBuffer*               compose_func (GdvAudioCompSink *this, GSList *inputs, GdvTime now);

static void                     flush_func (GdvAudioCompSink *this, GSList *inputs);

static gboolean                 new_pad_func (GdvAudioCompSink *this, GdvCompSinkInput *input);

static gboolean                 chain_enter_func (GdvAudioCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer);

static gboolean                 chain_leave_func (GdvAudioCompSink *this, GdvCompSinkInput *input, GstBuffer *buffer);

static void                     add_partial_buffer (GdvAudioCompSink *this, GstBuffer *buffer);

static int                      free_partials (GdvAudioCompSink *this);

static int                      calculate_partials_size (GdvAudioCompSink *this);

static GstBuffer*               handle_new_buffer_with_partials (GdvAudioCompSink *this, GstBuffer *buffer);

static GstBuffer*               get_glued_partials (GdvAudioCompSink *this);
