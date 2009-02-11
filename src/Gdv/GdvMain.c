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

#include "GdvMain.h"

static                  GdvEngineCapab Mad                      = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Wavparse                 = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Dvdec                    = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Dvdemux                  = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Oggdemux                 = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Vorbisdec                = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Dv1394src                = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Pngdec                   = GDV_ENGINECAPAB_UNKNOWN;

static                  GdvEngineCapab Jpegdec                  = GDV_ENGINECAPAB_UNKNOWN;

/* Check if a given gst element is availible */
gboolean                gdv_main_check_element (const gchar *elementname, GError **error)
{
        GstElement *Element = NULL;
        Element = gst_element_factory_make (elementname, "testobject");

        if (Element != NULL) {
                gst_object_unref (Element);
                return TRUE;
        } else {
                const gchar *errstring = gettext ("Element '%s' is not availible in your GStreamer installation. "
                                                  "Please check that necessary plugins are installed.");

                if (error != NULL) 
                        g_set_error (error, GDV_ERROR_DOMAIN, -1, errstring, elementname);
                
                return FALSE;
        }
}

/* Initialize GStreamer */
gboolean                gdv_main_gst_init (int *argc, char **argv[], GError **error)
{
        return gst_init_check (argc, argv, error);
}

/* Initialize the i18n. LOCALE_DIR env variable sets the dir with the availible 
 * localizations */
void                    gdv_main_i18n_init ()
{
        // The locale dir is supposed to be set by the startup scripts
        // for diva
        
        gchar *localedir = (char *) g_getenv ("LOCALE_DIR");
        if (localedir == NULL)
                localedir = "./";

        bindtextdomain ("diva", "localedir");
        textdomain ("diva");
}

/* A quick and handy initializer that does all the cruft */
gboolean                gdv_init (int *argc, char **argv[], GError **error)
{
        // This is a shortcut function to initialize everything
        // Don't use that in appilcation code. This is mostly
        // for "tests".
        g_assert_not_reached ();

        gdv_main_i18n_init ();
        // Check availibility of the basic elements 
        g_return_val_if_fail (gdv_main_gst_init (argc, argv, error), FALSE);
        //g_return_val_if_fail (gdv_main_gdv_init (argc, argv, error), FALSE);
        g_return_val_if_fail (gdv_main_check_elements (error), FALSE);
        
        return TRUE;
}

/* Check some basic elements. That should fail for borked GStreamer instalations */
gboolean                gdv_main_check_elements (GError **error)
{
        g_return_val_if_fail(gdv_main_check_element ("fakesink", error), FALSE);
        g_return_val_if_fail(gdv_main_check_element ("decodebin", error), FALSE);
        g_return_val_if_fail(gdv_main_check_element ("queue", error), FALSE);
        g_return_val_if_fail(gdv_main_check_element ("filesrc", error), FALSE);
        g_return_val_if_fail(gdv_main_check_element ("xvimagesink", error), FALSE);
        g_return_val_if_fail(gdv_main_check_element ("alsasink", error), FALSE);
        return TRUE;
} 

/* Check if given element is availible for us. Check is based on capab. If GDV_ENGINECAPAB_UNKNOWN
 * we do the check and fill the value */
gboolean                gdv_main_check_capab            (const gchar *element, GdvEngineCapab *capab)
{
        if (*capab == GDV_ENGINECAPAB_UNKNOWN) {
                gboolean ret = gdv_main_check_element (element, NULL);
                if (ret == TRUE) 
                        *capab = GDV_ENGINECAPAB_AVAILIBLE;
                else
                        *capab = GDV_ENGINECAPAB_NOTAVAILIBLE;
        }
                
        if (*capab == GDV_ENGINECAPAB_AVAILIBLE)
                return TRUE;
        else
                return FALSE;
}
        
/* Check if we have mp3-decoding capabilities */
gboolean                gdv_main_has_mp3 ()
{
        return gdv_main_check_capab ("mad", &Mad);
}

/* Check if we have wav capabilities */
gboolean                gdv_main_has_wav ()
{
        return gdv_main_check_capab ("wavparse", &Wavparse);
}


/* Check if we have dv-decoding capabilities */
gboolean                gdv_main_has_dv ()
{
        return (gdv_main_check_capab ("dvdec", &Dvdec) &&
                gdv_main_check_capab ("dvdemux", &Dvdemux));
}

/* Check if we have ogg-decoding capabilities */
gboolean                gdv_main_has_ogg ()
{
        return (gdv_main_check_capab ("vorbisdec", &Vorbisdec) &&
                gdv_main_check_capab ("oggdemux", &Oggdemux));
}

/* Check if we have dv-capturing (firewire) capabilities */
gboolean                gdv_main_has_dvcapture ()
{
        return gdv_main_check_capab ("ie1394dvsrc", &Dv1394src);
}

/* Check if we have image (Jpeg & Png) decoding capabilities */
gboolean                gdv_main_has_images ()
{
        return (gdv_main_check_capab ("jpegdec", &Jpegdec) &&
                gdv_main_check_capab ("pngdec", &Pngdec));
}

/* Get the current version of GStreamer */
void                    gdv_main_gstreamer_version (guint *major, guint *minor, guint *micro, guint *nano)
{
        gst_version (major, minor, micro, nano);
}
