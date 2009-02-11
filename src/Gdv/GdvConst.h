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

#ifndef __GDVCONST_H__
#define __GDVCONST_H__

#include <glib.h>

// Some constant values

#define                 GDV_DEBUG_LEVEL                         5
#define                 GDV_ZEROTIME                            0
#define                 GDV_INFINITY                            G_MAXINT64

// Some common debugging macros

#define                 GDV_ELEMENT_NAME                        gst_element_get_name ((GstElement *) this)

// Quarks

#define                 GDV_ERROR_DOMAIN                        g_quark_from_static_string ("GdvError")

// Debug

static inline void      GDV_MSG                                 (const gchar *element, const gchar *func, 
                                                                 GLogLevelFlags flags, const gchar *fmt, ...)
{
        #ifdef GDV_ENABLE_MESSAGES

        if (GDV_ENABLE_MESSAGES == 0 && flags == G_LOG_LEVEL_DEBUG)
                return;

        // FIXME: What a deep shit here...
        // FIXME: I have a feeling that this needs a small review
        
        va_list ap;
        va_start (ap, fmt);

        if (element == NULL && func == NULL)
                g_logv (G_LOG_DOMAIN, flags, fmt, ap);
        else if (func == NULL && element != NULL) {
                gchar *str = g_strdup_vprintf (fmt, ap);
                g_log (G_LOG_DOMAIN, flags, "[%s] %s", element, str);
                g_free (str);
        } else if (func != NULL && element == NULL) {
                gchar *str = g_strdup_vprintf (fmt, ap);
                g_log (G_LOG_DOMAIN, flags, "[%s] %s", func, str);
                g_free (str);
        } else if (func != NULL && element != NULL) {
                gchar *str = g_strdup_vprintf (fmt, ap);
                g_log (G_LOG_DOMAIN, flags, "[%s : %s] %s", element, func, str);
                g_free (str);
        } else
                g_assert_not_reached ();
        
        va_end (ap);
        #endif
}

#define                 GDV_DEBUG(str,...)                      GDV_MSG (GDV_ELEMENT_NAME, __FUNCTION__, \
                                                                         G_LOG_LEVEL_DEBUG, str, __VA_ARGS__)

#define                 GDV_DEBUG_E(str,...)                    GDV_MSG (NULL, __FUNCTION__, \
                                                                         G_LOG_LEVEL_DEBUG, str, __VA_ARGS__)

#define                 GDV_WARNING(str,...)                    GDV_MSG (GDV_ELEMENT_NAME, __FUNCTION__, \
                                                                         G_LOG_LEVEL_WARNING, str, __VA_ARGS__)

#define                 GDV_WARNING_E(str,...)                  GDV_MSG (NULL, __FUNCTION__, \
                                                                         G_LOG_LEVEL_WARNING, str, __VA_ARGS__)

#define                 GDV_DEBUG_ELEMENT                       GDV_DEBUG

#define                 GDV_DEBUG_EXT                           GDV_DEBUG_E

#define                 GDV_WARNING_ELEMENT                     GDV_WARNING

#define                 GDV_WARNING_EXT                         GDV_WARNING_E

#endif
