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

enum {
        ARG_0, 
        ARG_FRAMEDIMENSIONS,
        ARG_FPS,
        ARG_CAPS,
        ARG_PIXEL_ASPECT,
        ARG_FRAME_ASPECT
};

struct                          _GdvVideoFormatPrivate
{
        GstCaps *Caps;
};

static void                     gdv_videoformat_class_init (GdvVideoFormatClass *klass);

static void                     gdv_videoformat_init (GdvVideoFormat *this);

static void                     gdv_videoformat_dispose (GdvVideoFormat *this);

static void                     gdv_videoformat_finalize (GdvVideoFormat *this);

static void                     gdv_videoformat_get_property (GdvVideoFormat *this, guint propid,
                                                              GValue *value, GParamSpec *pspec);

static void                     gdv_videoformat_set_property (GdvVideoFormat *this, guint propid,
                                                              const GValue *value, GParamSpec *pspec);

static gboolean                 private_init (GdvVideoFormatPrivate *private);

static void                     private_dispose (GdvVideoFormatPrivate *private);


