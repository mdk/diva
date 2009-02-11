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

struct                          _GdvImageSheepPrivate
{
        GstClockTime BufferLength;
        GdvFrameDimensions *FrameDimensions;
        GstCaps *MasterCaps;
        GstBuffer *MasterBuffer;
        gchar *Filename;
        GdvProjectFormat *ProjectFormat;
};

enum
{
        ARG_0,
        ARG_BASE_FILE,
        ARG_PROJECTFORMAT,
};

static void                     gdv_imagesheep_class_init (GdvImageSheepClass *klass);

static void                     gdv_imagesheep_init (GdvImageSheep *this);

static void                     gdv_imagesheep_dispose (GdvImageSheep *this);

static void                     gdv_imagesheep_finalize (GdvImageSheep *this);

static gboolean                 private_set (GdvImageSheepPrivate *private,
                                             GdvProjectFormat *format, gchar *filename);

static GstCaps*                 get_caps_function (GdvImageSheep *this, GstPad *pad);

static GstBuffer*               buffer_func (GdvImageSheep *this, GstPad *pad, 
                                             GdvTime time, GdvTime endtime);

static gboolean                 prepare_buffer_data (GdvImageSheep *this);

static gboolean                 private_init (GdvImageSheep *this, GdvImageSheepPrivate *private);

static void                     private_dispose (GdvImageSheepPrivate *private);

static GstStateChangeReturn     change_state (GdvImageSheep *this, GstStateChange transition);

static void                     gdv_imagesheep_get_property (GdvImageSheep *this, guint propid,
                                                             GValue *value, GParamSpec *pspec);
