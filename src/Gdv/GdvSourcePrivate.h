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
        ARG_HASVIDEO,
        ARG_VIDEOFORMAT,
        ARG_AUDIOFORMAT,
        ARG_HASAUDIO,
        ARG_LENGTH,
        ARG_CONSISTENT,
};

struct                          _GdvSourcePrivate
{
        GdvSourcePad *VSrc; // The out video pad (Ghost)
        GdvSourcePad *ASrc; // The out audio pad (Ghost)
};

static void                     gdv_source_class_init (GdvSourceClass *klass);

static void                     gdv_source_get_property (GdvSource *this, guint propid,
                                                         GValue *value, GParamSpec *pspec);

static void                     gdv_source_init (GdvSource *this);

static void                     gdv_source_dispose (GdvSource *this);

static void                     gdv_source_finalize (GdvSource *this);

static gboolean                 private_init (GdvSource *this, GdvSourcePrivate *private);

static gboolean                 protected_init (GdvSource *this, GdvSourceProtected *private);

static void                     private_dispose (GdvSource *this, GdvSourcePrivate *private);

static void                     protected_dispose (GdvSource *this, GdvSourceProtected *private);



