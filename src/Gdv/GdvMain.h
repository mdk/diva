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

#ifndef __GDVMAIN_H__
#define __GDVMAIN_H__

#include <gst/gst.h>
#include <libintl.h>
#include "GdvConst.h"
#include "GdvTypes.h"

enum _GdvEngineCapab {
        GDV_ENGINECAPAB_AVAILIBLE,
        GDV_ENGINECAPAB_NOTAVAILIBLE,
        GDV_ENGINECAPAB_UNKNOWN
};

gboolean                gdv_main_check_element          (const gchar *elementname, GError **error);

gboolean                gdv_main_gst_init               (int *argc, char **argv[], GError **error);

gboolean                gdv_main_check_elements         (GError **error);

gboolean                gdv_main_gdv_init               (int *argc, char **argv[], GError **error);

gboolean                gdv_init                        (int *argc, char **argv[], GError **error);

void                    gdv_main_i18n_init              ();

gboolean                gdv_main_check_capab            (const gchar *element, GdvEngineCapab *capab);

gboolean                gdv_main_has_mp3                ();

gboolean                gdv_main_has_dv                 ();

gboolean                gdv_main_has_ogg                ();

gboolean                gdv_main_has_dvcapture          ();

gboolean                gdv_main_has_images             ();

void                    gdv_main_gstreamer_version      (guint *major, guint *minor, guint *micro, guint *nano);

#endif
