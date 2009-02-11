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

#ifndef __DIVA_WIDGETS_TYPES_H__
#define __DIVA_WIDGETS_TYPES_H__

/* Structs */

typedef struct                  _DivaWidgetsAspectFrame DivaWidgetsAspectFrame;

typedef struct                  _DivaWidgetsThmbRenderer DivaWidgetsThmbRenderer;

typedef struct                  _DivaWidgetsTaggyRenderer DivaWidgetsTaggyRenderer;

typedef struct                  _DivaWidgetsPaintRenderer DivaWidgetsPaintRenderer;

/* Classes */

typedef struct                  _DivaWidgetsAspectFrameClass DivaWidgetsAspectFrameClass;

typedef struct                  _DivaWidgetsThmbRendererClass DivaWidgetsThmbRendererClass;

typedef struct                  _DivaWidgetsTaggyRendererClass DivaWidgetsTaggyRendererClass;

typedef struct                  _DivaWidgetsPaintRendererClass DivaWidgetsPaintRendererClass;

/* GTypes */

#define                         DIVA_WIDGETS_TYPE_ASPECT_FRAME (diva_widgets_aspect_frame_get_type ())

#define                         DIVA_WIDGETS_TYPE_THMB_RENDERER (diva_widgets_thmb_renderer_get_type ())

#define                         DIVA_WIDGETS_TYPE_TAGGY_RENDERER (diva_widgets_taggy_renderer_get_type ())

#define                         DIVA_WIDGETS_TYPE_PAINT_RENDERER (diva_widgets_paint_renderer_get_type ())

/* Is */

#define                         DIVA_WIDGETS_IS_ASPECT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                                                               DIVA_WIDGETS_TYPE_ASPECT_FRAME))

#define                         DIVA_WIDGETS_IS_THMB_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                                                                DIVA_WIDGETS_TYPE_THMB_RENDERER))

#define                         DIVA_WIDGETS_IS_TAGGY_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                                                                 DIVA_WIDGETS_TYPE_TAGGY_RENDERER))

#define                         DIVA_WIDGETS_IS_PAINT_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                                                                 DIVA_WIDGETS_TYPE_PAINT_RENDERER))

/* Type casts */

#define                         DIVA_WIDGETS_ASPECT_FRAME(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                                                            DIVA_WIDGETS_TYPE_ASPECT_FRAME, \
                                                                                            DivaWidgetsAspectFrame))

#define                         DIVA_WIDGETS_THMB_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                                                             DIVA_WIDGETS_TYPE_THMB_RENDERER, \
                                                                                             DivaWidgetsThmbRenderer))

#define                         DIVA_WIDGETS_TAGGY_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                                                              DIVA_WIDGETS_TYPE_TAGGY_RENDERER, \
                                                                                              DivaWidgetsTaggyRenderer))

#define                         DIVA_WIDGETS_PAINT_RENDERER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                                                              GDV_TYPE_SOURCE, \
                                                                                              DivaWidgetsThmbRenderer))

#endif
