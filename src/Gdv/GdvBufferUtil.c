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

/* Functions for fast converting / scaling video buffers.
 *
 * We use macros heavily here. There are macros for getting/setting
 * pixels for every supported format, and for conversion between
 * RGB and YUV colorspaces.
 *
 */

#include "GdvBufferUtil.h"
#include <math.h>

/*
 * FIXME - downsampling for YUV creates artifacts
 * due to rounding errors for artificial content
 * (linear strip)
 */

/* Implicit: values y, u, v, a, r, g, b
 * width: w, height: h
 * array: array
 * coordinates: x_c, y_c
 */

#define IMPLICIT_VARIABLES unsigned char r, g, b, a, y, u, v

/* Mummy, make GCC not cry... */
#define TOUCH_IMPLICIT {  if (0) { r = g = b = a = y = u = v = 0;  } }


/* Macros for getting and setting one pixel in various formats.
 * We use macros here instead of inline functions because functions
 * can't return multiple parameters (and we typically have 3 values).
 * And we're *certain* the macros will be inlined.
 */

/* I420 is most tested, and should work flawlessly */
/* Note: there might be rounding issues for high-valiue pixels which then get clipped to very low */
#define GET_I420(array, w, h, stride, x_c, y_c) { \
        y = array[y_c * w + x_c]; \
        u = array[w * h + (y_c / 2) * (w / 2) + x_c / 2 ]; \
        v = array[w * h + w * h / 4 + (y_c / 2) * (w / 2) + x_c / 2 ]; \
}
#define SET_I420(array, w, h, stride, x_c, y_c) { \
        array[y_c * w + x_c] = y; \
        array[w * h + (y_c / 2) * w / 2 + x_c / 2 ] += (u + 2) / 4; \
        array[w * h + w * h / 4 + (y_c / 2) * w / 2 + x_c / 2 ] += (v + 2) / 4; \
}

/* YV12 is I420 with reversed V and U plane order, formulae are the same - untested */
#define GET_YV12(array, w, h, stride, x_c, y_c) { \
        y = array[y_c * w + x_c]; \
        v = array[w * h + (y_c / 2) * (w / 2) + x_c / 2 ]; \
        u = array[w * h + w * h / 4 + (y_c / 2) * (w / 2) + x_c / 2 ]; \
}
#define SET_YV12(array, w, h, stride, x_c, y_c) { \
        array[y_c * w + x_c] = y; \
        array[w * h + (y_c / 2) * w / 2 + x_c / 2 ] += (v + 2) / 4; \
        array[w * h + w * h / 4 + (y_c / 2) * w / 2 + x_c / 2 ] += (u + 2) / 4; \
}

/* This was also tested against a previous implementation and seems to work */
#define GET_YUY2(array, w, h, stride, x_c, y_c) { \
        y = array[y_c * w * 2 + x_c * 2]; \
        u = array[y_c * w * 2 + (x_c / 2) * 4 + 1 ]; \
        v = array[y_c * w * 2 + (x_c / 2) * 4 + 3 ]; \
}
#define SET_YUY2(array, w, h, stride, x_c, y_c) { \
        array[y_c * w * 2 + x_c * 2] = y; \
        array[y_c * w * 2 + (x_c / 2) * 4 + 1 ] += (u + 1) / 2; \
        array[y_c * w * 2 + (x_c / 2) * 4 + 3 ] += (v + 1) / 2; \
}
// FIXME - do we get bad picture because of this?
#define SET_YUY2_ONCE(array, w, h, stride, x_c, y_c) { \
        array[y_c * w * 2 + x_c * 2] = y; \
        array[y_c * w * 2 + (x_c / 2) * 4 + 1 ] = u; \
        array[y_c * w * 2 + (x_c / 2) * 4 + 3 ] = v; \
}


/* Trivial ;-) */
#define GET_RGB(array, w, h, stride, x_c, y_c) { \
        r = array[y_c * stride + x_c * 3]; \
        g = array[y_c * stride + x_c * 3 + 1]; \
        b = array[y_c * stride + x_c * 3 + 2]; \
}
#define SET_RGB(array, w, h, stride, x_c, y_c) { \
        array[y_c * stride + x_c * 3] = r; \
        array[y_c * stride + x_c * 3 + 1] = g; \
        array[y_c * stride + x_c * 3 + 2] = b; \
}

/* Transparent PNGs, here we come! */
#define GET_RGBA(array, w, h, stride, x_c, y_c) { \
        r = array[y_c * stride + x_c * 4]; \
        g = array[y_c * stride + x_c * 4 + 1]; \
        b = array[y_c * stride + x_c * 4 + 2]; \
        a = array[y_c * stride + x_c * 4 + 2]; \
}
#define SET_RGBA(array, w, h, stride, x_c, y_c) { \
        array[y_c * stride + x_c * 4] = r; \
        array[y_c * stride + x_c * 4 + 1] = g; \
        array[y_c * stride + x_c * 4 + 2] = b; \
        array[y_c * stride + x_c * 4 + 3] = a; \
}

/* Colorspace conversion - this is tested fairly well */
#define CONV_YUV_TO_RGB { \
        double dr, dg, db; \
        dr = 1.164 * (y - 16) + 1.596 * (v - 128); \
        dg = 1.164 * (y - 16) - 0.813 * (v - 128) - 0.391 * (u - 128); \
        db = 1.164 * (y - 16) + 2.018 * (u - 128); \
        dr = MAX (dr, 0); \
        db = MAX (db, 0); \
        dg = MAX (dg, 0); \
        r = MIN (dr,255); \
        b = MIN (db,255); \
        g = MIN (dg,255); \
}
#define CONV_RGB_TO_YUV { \
        double dy, dv, du; \
        dy = 0.5 + 0.257 * r + 0.504 * g + 0.098 * b + 16; \
        dv = 0.5 + 0.439 * r - 0.368 * g - 0.071 * b + 128; \
        du = 0.5 + -0.148 * r - 0.291 * g + 0.439 * b + 128; \
        dy = MAX (dy, 0); \
        dv = MAX (dv, 0); \
        du = MAX (du, 0); \
        y = MIN (dy,255); \
        u = MIN (du,255); \
        v = MIN (dv,255); \
}

/* In theory, these should be lot faster, but ... */
#define CONV_RGB_TO_YUV_INTEGER { \
        y = MIN(ABS(r * 2104 + g * 4130 + b * 802 + 4096 + 131072) >> 13, 235); \
        u = MIN(ABS(r * -1214 + g * -2384 + b * 3598 + 4096 + 1048576) >> 13, 240); \
        v = MIN(ABS(r * 3598 + g * -3013 + b * -585 + 4096 + 1048576) >> 13, 240); \
}
#define CONV_YUV_TO_RGB_INTEGER { \
        r = MIN((9535 * (y - 16) + 13074 * (v - 128)) >> 13, 255); \
        g = MIN((9535 * (y - 16) - 6660 * (v - 128) - 3203 * (u - 128)) >> 13, 255); \
        b = MIN((9535 * (y - 16) + 16531 * (u - 128)) >> 13, 255); \
}

#define LINEAR_INTERPOLATE(f1, f2, dx) f1 = f1 + ((dx * (f2 - f1)) >> 8)

#define CONVERT(name, get_fn, conv_fn, set_fn) \
static inline void name ## _unscaled (gpointer in , int w, int h, int stride, gpointer out) \
{ \
        IMPLICIT_VARIABLES; \
        int x_c, y_c; \
        unsigned char *array_in = in; \
        unsigned char *array_out = out; \
        for (y_c = 0; y_c < h; y_c++) { \
                for (x_c = 0; x_c < w; x_c++) { \
                        get_fn (array_in, w, h, stride, x_c, y_c); \
                        conv_fn; \
                        set_fn (array_out, w, h, stride, x_c, y_c); \
                } \
        } \
        TOUCH_IMPLICIT; \
} \
void name (gpointer in , int w, int h, int stride, gpointer out, int ow, int oh, int ostride) \
{ \
        IMPLICIT_VARIABLES; \
        int x_c, y_c; \
        unsigned char *array_in = in; \
        unsigned char *array_out = out; \
        if ((ow == w) && (oh == h)) { \
                name ## _unscaled (in, w, h, stride, out); \
                return; \
        } \
        double xscale = (w * 1.0) / ow; \
        double yscale = (h * 1.0) / oh; \
        int ay[4], av[4], au[4]; \
        int x1, x2, y1, y2; \
        for (y_c = 0; y_c < oh; y_c++) { \
                y1 = (int) (y_c * yscale); \
                y2 = y1 + 1; y2 = MIN(y2, (h-1)); \
                double Ady = (y_c * yscale) - y1; \
                int dy = (int) (Ady * 256); \
                for (x_c = 0; x_c < ow; x_c++) { \
                        x1 = (int) (x_c * xscale);  \
                        x2 = x1 + 1; x2 = MIN(x2, (w-1)); \
                        double Adx = (x_c * xscale) - x1; \
                        int dx = (int) (Adx * 256); \
                        get_fn (array_in, w, h, stride, x1, y1); av[0] = v; au[0] = u; ay[0] = y; \
                        get_fn (array_in, w, h, stride, x1, y2); av[1] = v; au[1] = u; ay[1] = y; \
                        get_fn (array_in, w, h, stride, x2, y1); av[2] = v; au[2] = u; ay[2] = y; \
                        get_fn (array_in, w, h, stride, x2, y2); av[3] = v; au[3] = u; ay[3] = y; \
                        LINEAR_INTERPOLATE(ay[0], ay[1], dy); \
                        LINEAR_INTERPOLATE(au[0], au[1], dy); \
                        LINEAR_INTERPOLATE(av[0], av[1], dy); \
                        LINEAR_INTERPOLATE(ay[2], ay[3], dy); \
                        LINEAR_INTERPOLATE(au[2], au[3], dy); \
                        LINEAR_INTERPOLATE(av[2], av[3], dy); \
                        LINEAR_INTERPOLATE(ay[0], ay[2], dx); \
                        LINEAR_INTERPOLATE(au[0], au[2], dx); \
                        LINEAR_INTERPOLATE(av[0], av[2], dx); \
                        v = (unsigned char) MIN(av[0], 255); \
                        y = (unsigned char) MIN(ay[0], 255); \
                        u = (unsigned char) MIN(au[0], 255); \
                        conv_fn; \
                        set_fn (array_out, ow, oh, ostride, x_c, y_c); \
                } \
        } \
        TOUCH_IMPLICIT; \
} 


/* I420 <-> RGB */
CONVERT(gdv_i420_to_rgb, GET_I420, CONV_YUV_TO_RGB, SET_RGB);
CONVERT(gdv_rgb_to_i420, GET_RGB, CONV_RGB_TO_YUV, SET_I420);

/* YUY2 <-> RGB */
CONVERT(gdv_yuy2_to_rgb, GET_YUY2, CONV_YUV_TO_RGB, SET_RGB);
CONVERT(gdv_rgb_to_yuy2, GET_RGB, CONV_RGB_TO_YUV, SET_YUY2);

/* YUY2 <-> RGBA */
CONVERT(gdv_yuy2_to_rgba, GET_YUY2, CONV_YUV_TO_RGB, SET_RGBA);
CONVERT(gdv_rgba_to_yuy2, GET_RGBA, CONV_RGB_TO_YUV, SET_YUY2);


/* YV12 <-> RGB */
CONVERT(gdv_yv12_to_rgb, GET_YV12, CONV_YUV_TO_RGB, SET_RGB);
CONVERT(gdv_rgb_to_yv12, GET_RGB, CONV_RGB_TO_YUV, SET_YV12);

/* YUY2 <-> I420 */
CONVERT(gdv_yuy2_to_i420, GET_YUY2, , SET_I420);
CONVERT(gdv_i420_to_yuy2, GET_I420, , SET_YUY2);

/* YV12 <-> I420 */
/* FIXME - this should just swap the planes! */
CONVERT(gdv_yv12_to_i420, GET_YV12, , SET_I420);
CONVERT(gdv_i420_to_yv12, GET_I420, , SET_YV12);

/* YUY2 <-> YV12 */
CONVERT(gdv_yuy2_to_yv12, GET_YUY2, , SET_YV12);
CONVERT(gdv_yv12_to_yuy2, GET_YV12, , SET_YUY2);

/* Scaling */
CONVERT(gdv_rgb_copy, GET_RGB, , SET_RGB);
CONVERT(gdv_rgba_copy, GET_RGBA, , SET_RGBA);
CONVERT(gdv_i420_copy, GET_I420, , SET_I420);
CONVERT(gdv_yuy2_copy, GET_YUY2, , SET_YUY2);
CONVERT(gdv_yv12_copy, GET_YV12, , SET_YV12);

/* got these from fourcc.org */
#define FOURCC_RGB 0x32424752
#define FOURCC_RGBA 0x41424752
#define FOURCC_I420 0x30323449
#define FOURCC_YUY2 0x32595559
#define FOURCC_YV12 0x32315659

// FIXME this could be a bit clearer
static struct {
        guint32 from;
        guint32 to;
        gdv_videobuffer_converter_t handler;
} converters[] = {
        { FOURCC_I420, FOURCC_RGB, gdv_i420_to_rgb },
        { FOURCC_RGB, FOURCC_I420, gdv_rgb_to_i420 },

        { FOURCC_YUY2, FOURCC_RGB, gdv_yuy2_to_rgb },
        { FOURCC_RGB, FOURCC_YUY2, gdv_rgb_to_yuy2 },

        { FOURCC_YUY2, FOURCC_RGBA, gdv_yuy2_to_rgba },
        { FOURCC_RGBA, FOURCC_YUY2, gdv_rgba_to_yuy2 },

        { FOURCC_YV12, FOURCC_RGB, gdv_yv12_to_rgb },
        { FOURCC_RGB, FOURCC_YV12, gdv_rgb_to_yv12 },

        { FOURCC_YUY2, FOURCC_I420, gdv_yuy2_to_i420 },
        { FOURCC_I420, FOURCC_YUY2, gdv_i420_to_yuy2 },

        { FOURCC_YV12, FOURCC_I420, gdv_yv12_to_i420 },
        { FOURCC_I420, FOURCC_YV12, gdv_i420_to_yv12 },

        { FOURCC_YV12, FOURCC_YUY2, gdv_yv12_to_yuy2 },
        { FOURCC_YUY2, FOURCC_YV12, gdv_yuy2_to_yv12 },

        { FOURCC_RGB, FOURCC_RGB, gdv_rgb_copy },
        { FOURCC_RGBA, FOURCC_RGBA, gdv_rgba_copy },
        { FOURCC_I420, FOURCC_I420, gdv_i420_copy },
        { FOURCC_YUY2, FOURCC_YUY2, gdv_yuy2_copy },
        { FOURCC_YV12, FOURCC_YV12, gdv_yv12_copy },

        { 0, 0, NULL }
};

gdv_videobuffer_converter_t gdv_videobuffer_get_converter(guint32 fourcc_from, guint32 fourcc_to)
{
        int i;
        for (i = 0; converters[i].handler != NULL; i++) {
                if ((converters[i].from == fourcc_from) && (converters[i].to == fourcc_to)) {
                        return converters[i].handler;
                }
        }
        return NULL;
}


// Composer stuff - this should be lay out a bit better, but it's ok for now
void gdv_videobuffer_blitter_yuy2(gpointer front, int fx, int fy, int fw, int fh, gpointer back, int w, int h, double ratio)
{
        IMPLICIT_VARIABLES;
        int x_f, y_f, x_b, y_b;
        unsigned char t1, t2, t3;

        unsigned char *array_front = front;
        unsigned char *array_back = back;

        // 256 == a constant factor so we avoid floating point math
        int fac = (int) (256.0 * ratio);

        int stride = w * h; stride += 0; // make gcc not cry
        int fstride = fw * fh; fstride += 0; // make gcc not cry

        for (x_f = 0; x_f < fw; x_f++) {
                x_b = fx + x_f;
                if (x_b < 0) continue;
                if (x_b >= w) break;
                
                for (y_f = 0; y_f < fh; y_f++) {
                        y_b = fy + y_f;
                        if (y_b < 0) continue;
                        if (y_b >= h) break;

                        GET_YUY2(array_back, w, h, stride, x_b, y_b);
                        t1 = y; t2 = u; t3 = v;
                        GET_YUY2(array_front, fw, fh, fstride, x_f, y_f);

                        // the formula is: result = ratio * front + (1 - ratio) * back
                        // end cases:
                        //   1 * front + (1 - 1) * back = front + 0 = front
                        //   0 * front + (1 - 0) * back = 0 + back = back
                        //   0.5 * front + (1 - 0.5) * back = 0.5 * front + 0.5 * back = (front + back) / 2

                        y = ((fac * y) + ((256 - fac) * t1)) / 256;
                        u = ((fac * u) + ((256 - fac) * t2)) / 256;
                        v = ((fac * v) + ((256 - fac) * t3)) / 256;

                        SET_YUY2_ONCE(array_back, w, h, stride, x_b, y_b);
                }
        }
        TOUCH_IMPLICIT;
}

void gdv_videobuffer_blitter_bitwise(gpointer front, gpointer back, size_t size, double ratio)
{
        unsigned char *array_front = front;
        unsigned char *array_back = back;

        // 256 == a constant factor so we avoid floating point math
        int fac = (int) (256.0 * ratio);

        size_t i;
        for (i = 0; i < size; i++) {
            array_back[i] = (unsigned char) (((fac * array_front[i]) + ((256 - fac) * array_back[i])) / 256);
        }
}

void gdv_videobuffer_scale_compose_yuy2(gpointer front , int w, int h, gpointer back, int ow, int oh, double ratio) 
{ 
        IMPLICIT_VARIABLES; 
        int x_c, y_c; 
        unsigned char *array_front = front; 
        unsigned char *array_back = back; 
        if ((ow == w) && (oh == h)) { 
                gdv_videobuffer_blitter_bitwise(front, back, ow * oh * 2, ratio);
                return;
        } 
        // 256 == a constant factor so we avoid floating point math
        int fac = (int) (256.0 * ratio);

        double xscale = (w * 1.0) / ow; 
        double yscale = (h * 1.0) / oh; 
        int ay[4], av[4], au[4]; 
        int x1, x2, y1, y2; 
        int t1, t2, t3;

        int stride = w * h; stride += 0; // make gcc not cry
        int ostride = ow * oh; ostride += 0; // make gcc not cry

        for (y_c = 0; y_c < oh; y_c++) { 
                y1 = (int) (y_c * yscale); 
                y2 = y1 + 1; y2 = MIN(y2, (h-1)); 

                double Ady = (y_c * yscale) - y1; 
                int dy = (int) (Ady * 256); 
                for (x_c = 0; x_c < ow; x_c++) { 
                        x1 = (int) (x_c * xscale);  
                        x2 = x1 + 1; x2 = MIN(x2, (w-1)); 
                        double Adx = (x_c * xscale) - x1; 
                        int dx = (int) (Adx * 256); 
                        GET_YUY2 (array_front, w, h, stride, x1, y1); av[0] = v; au[0] = u; ay[0] = y; 
                        GET_YUY2 (array_front, w, h, stride, x1, y2); av[1] = v; au[1] = u; ay[1] = y; 
                        GET_YUY2 (array_front, w, h, stride, x2, y1); av[2] = v; au[2] = u; ay[2] = y; 
                        GET_YUY2 (array_front, w, h, stride, x2, y2); av[3] = v; au[3] = u; ay[3] = y; 
                        LINEAR_INTERPOLATE(ay[0], ay[1], dy); 
                        LINEAR_INTERPOLATE(au[0], au[1], dy); 
                        LINEAR_INTERPOLATE(av[0], av[1], dy); 
                        LINEAR_INTERPOLATE(ay[2], ay[3], dy); 
                        LINEAR_INTERPOLATE(au[2], au[3], dy); 
                        LINEAR_INTERPOLATE(av[2], av[3], dy); 
                        LINEAR_INTERPOLATE(ay[0], ay[2], dx); 
                        LINEAR_INTERPOLATE(au[0], au[2], dx); 
                        LINEAR_INTERPOLATE(av[0], av[2], dx); 

                        t1 = MIN(ay[0], 255);
                        t2 = MIN(au[0], 255);
                        t3 = MIN(av[0], 255);

                        GET_YUY2(array_back, ow, oh, ostride, x_c, y_c);

                        y = ((fac * t1) + ((256 - fac) * y)) / 256;
                        u = ((fac * t2) + ((256 - fac) * u)) / 256;
                        v = ((fac * t3) + ((256 - fac) * v)) / 256;

                        SET_YUY2_ONCE (array_back, ow, oh, ostride, x_c, y_c); 
                } 
        } 
        TOUCH_IMPLICIT; 
} 


