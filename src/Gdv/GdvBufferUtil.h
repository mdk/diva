///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//      Senko Rasic <senko@senko.net>                                        //
//                                                                           //
// Permission is hereby granted, free of charge, to any person obtaining a   //
// copy of this software and associated documentation files (the             //
// "Software"), to deal in the Software without restriction, including       //
// without limitation the rights to use, copy, modify, merge, publish,       //
// distribute, sublicense, and/or sell copies of the Software, and to permit //
// persons to whom the Software is furnished to do so, subject to the        //
// following conditions:                                                     //
//                                                                           //
// The above copyright notice and this permission notice shall be included   //
// in all copies or substantial portions of the Software.                    //
//                                                                           //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
 
#ifndef __GDVBUFFERUTIL_H__
#define __GDVBUFFERUTIL_H__

#include <glib.h>

typedef void (*gdv_videobuffer_converter_t)(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
gdv_videobuffer_converter_t gdv_videobuffer_get_converter(guint32 fourcc_from, guint32 fourcc_to);

/* I420 <-> RGB */
extern void gdv_i420_to_rgb(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_rgb_to_i420(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* YUY2 <-> RGB */
extern void gdv_yuy2_to_rgb(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_rgb_to_yuy2(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* YUY2 <-> RGBA */
extern void gdv_yuy2_to_rgba(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_rgba_to_yuy2(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* YV12 <-> RGB */
extern void gdv_yv12_to_rgb(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_rgb_to_yv12(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* YUY2 <-> I420 */
extern void gdv_yuy2_to_i420(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_i420_to_yuy2(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* YV12 <-> I420 */
extern void gdv_yv12_to_i420(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_i420_to_yv12(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* YUY2 <-> YV12 */
extern void gdv_yuy2_to_yv12(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_yv12_to_yuy2(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* Scaling */
extern void gdv_rgb_copy(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_i420_copy(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_yuy2_copy(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);
extern void gdv_yv12_copy(gpointer in, int w, int h, int stride, gpointer out, int ow, int oh, int ostride);

/* These are all YUY2-to-YUY2 functions only */
/* Blit front frame onto back frame with opacity 'ratio'. Frame dimensions may differ, and you can specify a displacement */
void gdv_videobuffer_blitter_yuy2(gpointer front, int fx, int fy, int fw, int fh, gpointer back, int w, int h, double ratio);
/* Bitwise merge frames. They must have the same dimensions, and no displacement. */
void gdv_videobuffer_blitter_bitwise(gpointer front, gpointer back, size_t size, double ratio);
/* Scale the front frame to the dimensions of the back frame, and combine them with front frame opacity == ratio */
void gdv_videobuffer_scale_compose_yuy2(gpointer front , int w, int h, gpointer back, int ow, int oh, double ratio);

#endif

