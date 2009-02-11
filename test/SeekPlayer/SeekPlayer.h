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

/* Structs */

struct _UnblockAction {

        GstElement *Element;
        GstPad *Pad;
        gboolean KillSwitch;
        GMutex *Mutex;
        
} typedef UnblockAction;

#define ACTION_LOCK(obj)        (g_mutex_lock(obj->Mutex))

#define ACTION_UNLOCK(obj)      (g_mutex_unlock(obj->Mutex))

/* Globals */

GstElement*                     FileSrc;

GstElement*                     DecodeBin;

GstElement*                     VideoSink;

GstElement*                     AudioSink;

GstElement*                     AudioConvert;

GstElement*                     AudioResample;

GstElement*                     VideoQueue;

GstElement*                     AudioQueue;

GstElement*                     VIdentity;

GstElement*                     AIdentity;

GstBin*                         Pipeline;

gboolean                        HasVideo;

gboolean                        HasAudio;

GstClockTime                    MinABuffer;

GstClockTime                    MaxABuffer;

GstClockTime                    MinVBuffer;

GstClockTime                    MaxVBuffer;

/* Funcs */

void                            show_banner (void);

void                            show_usage (void);

gboolean                        build_pipeline (gchar *file);

gboolean                        link_pipeline (void);

int                             main (int argc, gchar **argv);

void                            new_pad (GstElement *element, GstPad *pad, gpointer ptr);

gboolean                        block_initial (void);

void                            block (GstPad *pad, gboolean blocked, UnblockAction *action);

gboolean                        idle_unblock_action (UnblockAction *action);

UnblockAction*                  new_action (GstPad *pad, GstElement *element);

void                            no_more_pads (GstElement *element, gpointer ptr);

gboolean                        bus_callback (GstBus *bus, GstMessage *message, gpointer ptr);

gboolean                        timeout (gpointer ptr);

void                            show_buffer_results ();

void                            handoff (GstElement *element, GstBuffer *buffer, gpointer ptr);

