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

#define                         STUB_WIDTH 16

#define                         STUB_HEIGHT 16

struct                          _GdvBlackSheepPrivate
{
        GstClockTime BufferLength;
        GdvFraction *Fps;
};

static void                     gdv_blacksheep_class_init (GdvBlackSheepClass *klass);

static void                     gdv_blacksheep_init (GdvBlackSheep *this);

static void                     gdv_blacksheep_dispose (GdvBlackSheep *this);

static void                     gdv_blacksheep_finalize (GdvBlackSheep *this);

static gboolean                 private_init (GdvBlackSheep *this, GdvBlackSheepPrivate *private);

static GstCaps*                 get_caps_function (GdvBlackSheep *this, GstPad *pad);

static GstBuffer*               buffer_func (GdvBlackSheep *this, GstPad *pad, 
                                             GdvTime time, GdvTime endtime);
        
static void                     private_set (GdvBlackSheepPrivate *private, GdvFraction *fps);        
