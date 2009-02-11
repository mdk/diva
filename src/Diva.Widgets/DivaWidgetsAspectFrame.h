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

#ifndef __DIVA_WIDGETS_ASPECTFRAME_H__
#define __DIVA_WIDGETS_ASPECTFRAME_H__

#include <gtk/gtk.h>
#include <gdv.h>
#include <divacairo.h>
#include "DivaWidgetsTypes.h"

struct _DivaWidgetsAspectFrame {
        GtkDrawingArea Parent;

        GdvFrameDimensions *_Dimensions; // The frame dimensions (resolution)
        GdvFraction *_PixelAspect;       // Pixel aspect ratio
        GdvFraction *_Fps;               // Frames per second (text display only)
};

struct _DivaWidgetsAspectFrameClass {
        GtkDrawingAreaClass ParentClass;
};

GType                           diva_widgets_aspect_frame_get_type (void);

void                            diva_widgets_aspect_frame_class_init (DivaWidgetsAspectFrameClass *klass);

void                            diva_widgets_aspect_frame_init (DivaWidgetsAspectFrame *this);

void                            diva_widgets_aspect_frame_finalize (DivaWidgetsAspectFrame *this);

DivaWidgetsAspectFrame*         diva_widgets_aspect_frame_new ();

void                            diva_widgets_aspect_frame_get_property (DivaWidgetsAspectFrame *this, guint propid,
                                                                        GValue *value, GParamSpec *pspec);

void                            diva_widgets_aspect_frame_set_property (DivaWidgetsAspectFrame *this, guint propid,
                                                                        const GValue *value, GParamSpec *pspec);

#endif
