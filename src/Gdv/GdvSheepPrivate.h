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

#define                         GDV_SHEEP_GET_CLASS(obj) ((GdvSheepClass *) G_OBJECT_GET_CLASS(obj))

#define                         GDV_SHEEP_PAD(obj) (obj->Private->SrcPad)

struct                          _GdvSheepPrivate
{
        GstPad *SrcPad;
        
        GdvTimeSpan *Segment;
        GdvTime CurrentTime;
        GstEvent *CachedSeek;
        gboolean Running; 
};

static void                     gdv_sheep_class_init (GdvSheepClass *klass);

static void                     gdv_sheep_init (GdvSheep *this, gpointer klass);

static void                     gdv_sheep_dispose (GdvSheep *this);

static void                     gdv_sheep_finalize (GdvSheep *this);

static gboolean                 private_init (GdvSheep *this, GdvSheepPrivate *private, gpointer klass);

static void                     private_dispose (GdvSheep *this, GdvSheepPrivate *private);

static gboolean                 activate_push (GstPad *pad, gboolean active);

static void                     task_loop (GstPad *pad);

static GstCaps*                 pad_getcaps (GstPad * pad);

static gboolean                 caps_negotiate (GdvSheep *this);

static gboolean                 perform_seek (GdvSheep *this, GstEvent *event);

static gboolean                 event_func (GstPad *pad, GstEvent *event);

static gboolean                 send_event (GdvSheep *this, GstEvent *event);

static gboolean                 pause_task (GdvSheep *this);

static gboolean                 start_task (GdvSheep *this, gpointer loop);
