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

#define VIDEO_CAPS              GST_STATIC_CAPS ("video/x-raw-yuv;" \
                                                 "video/x-raw-rgb")

#define USE_PAD_ALLOC           FALSE

struct                          _GdvTwinCompSinkPrivate
{
        GdvVideoBoy *Boy;     // The video boy we're using for rendering
        GdvTime BufferLength; // Length of buffers we're pushing out
        GstPad *LeftPad;      // The left input pad
        GstPad *RightPad;     // The right input pad
};

static gboolean                 private_init (GdvTwinCompSink *this, GdvTwinCompSinkPrivate *priv, gpointer klass);

static gboolean                 private_dispose (GdvTwinCompSink *this, GdvTwinCompSinkPrivate *priv);

static void                     gdv_twincompsink_dispose (GdvTwinCompSink *this);

static void                     gdv_twincompsink_finalize (GdvTwinCompSink *this);

static void                     gdv_twincompsink_base_init (gpointer klass);

static void                     gdv_twincompsink_class_init (GdvTwinCompSinkClass *klass);

static void                     gdv_twincompsink_init (GdvTwinCompSink *this, gpointer klass);

static GstBuffer*               compose_func (GdvTwinCompSink *this, GSList *inputs, GdvTime now);
