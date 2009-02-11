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

/* GdvTime is a basic time unit expressed in nanoseconds. It's meant to be
 * compatible with GstClockTime by GStreamer 
 */

#include "GdvTime.h"

/* c99 extension - this is an ugly hack to prevent declaration warnings (--senko) */
extern float roundf(float x);

/* Get a number of seconds */
gdouble                         gdv_time_to_seconds (GdvTime time)
{
        return (gdouble) time / (gdouble) GDV_SECOND;
}

/* New nanosecond time from seconds */
GdvTime                         gdv_time_from_seconds (gdouble seconds)
{
        return seconds * (gdouble) GDV_SECOND;
}

/* Convert a time relative to reloffset into absolute time */
GdvTime                         gdv_time_relative_to_absolute (GdvTime reloffset, GdvTime time)
{
        return reloffset + time;
}

/* Convert an absolute time to a relative value to relativeto  */
GdvTime                         gdv_time_absolute_to_relative (GdvTime relativeto, GdvTime time)
{
        if (time > relativeto)
                return time - relativeto;
        else
                return GDV_ZEROTIME;
}

/* Check if the time period (in-out) contains the given time */
gboolean                        gdv_time_inspan (GdvTime time, GdvTime in, GdvTime out)
{
        if (in == out)
                return FALSE;
                
        g_assert (in < out);
        
        if (time >= in && time < out)
                return TRUE;
        else 
                return FALSE;
}

/* Round this time to the nearest value defined by precision */
GdvTime                         gdv_time_round (GdvTime time, GdvTime precision)
{
        gfloat multi = roundf ((gfloat) time / (gfloat) precision);
        
        return (GdvTime) ((gint32) multi * (gfloat) precision);
}

/* Return a nicely formatted string with this time. This is for debugging purposes only.
 * On the managed side we use custom function for this */
gchar*                          gdv_time_to_string (GdvTime time)
{
        if (time != GDV_INFINITY)
                return g_strdup_printf ("%.2f", gdv_time_to_seconds (time));
        else
                return g_strdup_printf ("infinity");
}

/* Get a gint64 value in a safe manner. That values are used for seeking in GStreamer, etc. */
gint64                          gdv_time_get_int64 (GdvTime time)
{
        if (time == GDV_INFINITY)
                return -1;
        
        if (time < G_MAXINT64)
                return (gint64) time;
        else {
                GDV_WARNING_EXT ("Time value clipped to gint64", NULL);
                return G_MAXINT64;
        }
}
