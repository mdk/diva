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

#include "GdvInspector.h"

// PRIVATE ////////////////////////////////////////////////////////////////////

enum {
        ARG_0,
        ARG_MIME,
        ARG_HASVIDEO,
        ARG_HASAUDIO,
        ARG_VIDEOFORMAT,
        ARG_AUDIOFORMAT,
        ARG_URL,
        ARG_LENGTH,
        ARG_TAGS,
        ARG_ERROR
};

static void                    gdv_inspector_dispose (GdvInspector *this);
  
static void                    gdv_inspector_finalize (GdvInspector *this);

static void                    gdv_inspector_init (GdvInspector *this);

static void                    gdv_inspector_class_init (GdvInspectorClass *klass);

static void                    gdv_inspector_get_property (GdvInspector *this, guint propid,
                                                           GValue *value, GParamSpec *pspec);

static void                    no_more_pads (GstElement *element, GdvInspector *this);

static gboolean                bus_callback (GstBus *bus, GstMessage *message,
                                             GdvInspector *this);

static gboolean                stop_inspecting (GdvInspector *this);

static void                    caps_notify (GstPad *pad, gpointer param,
                                            GdvInspector *this);

static void                    new_pad (GstElement *element, GstPad *pad, gboolean b, GdvInspector *this);

static gboolean                check_if_done (GdvInspector *this);

static gboolean                check_length (GdvInspector *this);

static void                    have_type (GstElement *typefind, gpointer data, GstCaps *caps, GdvInspector  *this);

static void                    unknown_type (GstElement *element, GstPad *pad, GstCaps *caps, GdvInspector *this);

static void                    parse_caps (GdvInspector *this, GstCaps *caps);

// GLOBAL /////////////////////////////////////////////////////////////////////

static GstPipelineClass*       ParentClass = NULL;

// PUBLIC /////////////////////////////////////////////////////////////////////

/* GObject get type */
GType                          gdv_inspector_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvInspectorClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_inspector_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvInspector),
                        8,
                        (GInstanceInitFunc) gdv_inspector_init,
                };
                objecttype = g_type_register_static (GST_TYPE_PIPELINE, "GdvInspector", &objectinfo, 0);
        }

        return objecttype;
}

/* Create a new object */
GdvInspector*                   gdv_inspector_new (const gchar *filename)
{
        g_return_val_if_fail (filename != NULL, NULL);
        GDV_DEBUG_EXT ("Creating a new GdvInspector for %s", filename);
        
        GdvInspector *this = NULL;
        
        this = g_object_new (GDV_TYPE_INSPECTOR, NULL);
        if (this == NULL)
                goto failure;

        // Set the file location
        g_object_set (G_OBJECT (this->_Filesrc), "location", filename, NULL);
        
        // Set the Uri
        this->_Url = g_strdup (filename);
        return this;
                
        // FAILURE //
        failure:
        if (this != NULL) 
                gst_object_unref (GST_OBJECT (this));
        
        return NULL;
}

/* Set the idle callback that will be called once we're done */
void                            gdv_inspector_set_done_func (GdvInspector *this, gpointer func)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));

        this->_DoneFunc = func;
}

/* Set the idle callback that will be called if the error occurs */
void                            gdv_inspector_set_error_func (GdvInspector *this, gpointer func)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));

        this->_ErrorFunc = func;
}

/* Start the process of inspecting */
void                            gdv_inspector_start (GdvInspector *this)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));
        
        // Bus watch
        this->_Watch = gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (this)),
                                          (gpointer) bus_callback, this);
        
        GDV_DEBUG_ELEMENT ("Starting inspecting", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PLAYING);
}

GType                          gdv_inspector_get_tag_type (GdvInspector *this, gchar *tag)
{
        g_assert (this != NULL); g_assert (tag != NULL); g_assert (GDV_IS_INSPECTOR(this));

        switch (gst_tag_get_type (tag)) {
                case G_TYPE_INT:
                case G_TYPE_LONG:
                case G_TYPE_INT64:
                    return G_TYPE_INT64;

                case G_TYPE_UINT:
                case G_TYPE_ULONG:
                case G_TYPE_UINT64:
                    return G_TYPE_UINT64;

                case G_TYPE_STRING:
                    return G_TYPE_STRING;
        } 
        return G_TYPE_INVALID;
}

/* FIXME We don't do it via GObject because we'd have to marshall the entire structure. This is easier. */
gboolean                       gdv_inspector_get_int_tag (GdvInspector *this, gchar *tag, gint64 *value)
{

        g_assert (this != NULL); g_assert (tag != NULL); g_assert (GDV_IS_INSPECTOR(this));

        GDV_DEBUG_ELEMENT ("Requesting integer tag %s", tag, NULL);

        switch (gst_tag_get_type (tag)) {

                case G_TYPE_INT: {
                        gint val;
                        if (!gst_tag_list_get_int (this->_Tags, tag, &val)) return FALSE;
                        *value = (gint64) val;
                }
                break;
                case G_TYPE_LONG: {
                        glong val;
                        if (!gst_tag_list_get_long (this->_Tags, tag, &val)) return FALSE;
                        *value = (gint64) val;
                }
                break;
                case G_TYPE_INT64: {
                        gint64 val;
                        if (!gst_tag_list_get_int64 (this->_Tags, tag, &val)) return FALSE;
                        *value = (gint64) val;
                }
                break;
                default:
                        // that's ok, be graceful
                        // g_warning ("Requesting nonexistent/non-integer tag %s", tag);
                        return FALSE;
        }
        return TRUE;
}

/* Abort the current inspecting operation in progress */
void                            gdv_inspector_abort (GdvInspector *this)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));
        
        if (this->_Executed == TRUE)
                return;
        
        // FIXME: Mutex here!
        this->_DoneFunc = NULL;
        this->_ErrorFunc = NULL;
        
        gst_element_set_state ((GstElement *) this, GST_STATE_NULL);
}

gboolean                       gdv_inspector_get_uint_tag (GdvInspector *this, gchar *tag, guint64 *value)
{

        g_assert (this != NULL); g_assert (tag != NULL); g_assert (GDV_IS_INSPECTOR(this));

        GDV_DEBUG_ELEMENT ("Requesting unsigned integer tag %s", tag, NULL);

        switch (gst_tag_get_type (tag)) {

                case G_TYPE_UINT: {
                        guint val;
                        if (!gst_tag_list_get_uint (this->_Tags, tag, &val)) return FALSE;
                        *value = (guint64) val;
                }
                break;
                case G_TYPE_ULONG: {
                        gulong val;
                        if (!gst_tag_list_get_ulong (this->_Tags, tag, &val)) return FALSE;
                        *value = (guint64) val;
                }
                break;
                case G_TYPE_UINT64: {
                        guint64 val;
                        if (!gst_tag_list_get_uint64 (this->_Tags, tag, &val)) return FALSE;
                        *value = (guint64) val;
                }
                break;
                default:
                        // that's ok, be graceful
                        // g_warning ("Requesting nonexistent/non-unsigned-integer tag %s", tag);
                        return FALSE;
        }
        return TRUE;
}

gboolean                       gdv_inspector_get_string_tag (GdvInspector *this, gchar *tag, gchar **value)
{

        g_assert (this != NULL); g_assert (tag != NULL); g_assert (GDV_IS_INSPECTOR(this));

        GDV_DEBUG_ELEMENT ("Requesting string tag %s", tag, NULL);

        if (gst_tag_get_type (tag) == G_TYPE_STRING) {
                gchar *val;
                if (!gst_tag_list_get_string (this->_Tags, tag, &val)) return FALSE;
                *value = val;
                return TRUE;
                // FIXME the caller should free this afterwards
        }

        // that's ok, be graceful
        // g_warning ("Requesting nonexistent/non-unsigned-integer tag %s", tag);
        return FALSE;
}

// Private ////////////////////////////////////////////////////////////////////

/* GObject dispose */
static void                    gdv_inspector_dispose (GdvInspector *this)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));
        
        GDV_DEBUG_ELEMENT ("Disposing...", NULL);
        
        /*
        if (this->_VideoFormat != NULL) {
                g_object_unref (this->_VideoFormat);
                this->_VideoFormat = NULL;
        }

        if (this->_AudioFormat != NULL) {
                g_object_unref (this->_AudioFormat);
                this->_AudioFormat = NULL;
        }*/
        
        if (this->_Watch != -1) {
                g_source_remove (this->_Watch);
                this->_Watch = -1;
        }

        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}
        
static void                    gdv_inspector_finalize (GdvInspector *this)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));
        
        GDV_DEBUG_ELEMENT ("Finalizing...", NULL);
        
        if (this->_Mime != NULL) {
                g_free (this->_Mime);
                this->_Mime = NULL;
        }
        
        if (this->_Url != NULL) {
                g_free (this->_Url);
                this->_Url = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject init */
static void                    gdv_inspector_init (GdvInspector *this)
{
        GDV_DEBUG_ELEMENT ("Initializing fresh GdvInspector object", NULL);
        
        this->_Mime = NULL;
        this->_Url = NULL;
        this->_Error = NULL;
        this->_HasVideo = FALSE;
        this->_HasAudio = FALSE;
        this->_VideoFormat = NULL;
        this->_AudioFormat = NULL;
        this->_Length = GDV_ZEROTIME;
        this->_Tags = gst_tag_list_new ();
        this->_Watch = -1;
        this->_Executed = FALSE;
        this->_Finished = FALSE;

        GDV_DEBUG_ELEMENT ("Creating all the elements", NULL);
        this->_Filesrc = gst_element_factory_make ("filesrc", NULL);
        g_assert (this->_Filesrc != NULL);
        g_object_set (G_OBJECT (this->_Filesrc), "blocksize", 1000000, NULL);
                
        this->_Decodebin = gst_element_factory_make ("decodebin", NULL);
        g_assert (this->_Decodebin != NULL);

        GDV_DEBUG_ELEMENT ("Connecting signals", NULL);
        
        gulong sigid1 = g_signal_connect (G_OBJECT(this->_Decodebin), "new-decoded-pad",
                                          G_CALLBACK(new_pad), this);
        
        gulong sigid2 = g_signal_connect (G_OBJECT(this->_Decodebin), "no-more-pads",
                                          G_CALLBACK(no_more_pads), this);
        
        g_assert (sigid1 != 0 && sigid2 != 0);
               
        GDV_DEBUG_ELEMENT ("Adding & linking", NULL);
        gst_bin_add_many ((GstBin *) this, this->_Filesrc, this->_Decodebin,
                          NULL);
        gboolean l1 = gst_element_link (this->_Filesrc, this->_Decodebin);
        g_assert (l1 == TRUE);
        
        // Typefinder
        GstElement *Typefind = gst_bin_get_by_name ((GstBin *) this->_Decodebin, "typefind");
        g_assert (Typefind != NULL);
        
        gulong sigid3 = g_signal_connect (G_OBJECT (Typefind), "have-type",
                                          G_CALLBACK(have_type), this);
        g_assert (sigid3 != 0);
        
        gulong sigid4 = g_signal_connect (G_OBJECT (this->_Decodebin), "unknown-type",
                                          G_CALLBACK (unknown_type), this);
        g_assert (sigid4 != 0);
        
}

/* GObject class init */
static void                    gdv_inspector_class_init (GdvInspectorClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        gobjectclass->get_property = (gpointer) gdv_inspector_get_property;

        // Properties
        g_object_class_install_property (gobjectclass, ARG_URL,
					 g_param_spec_string
					 ("url", "Url",
					  "Url of the file",
					  NULL, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_MIME,
					 g_param_spec_string
					 ("mime", "Mime",
					  "Mime type",
					  NULL, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_HASAUDIO,
                                         g_param_spec_boolean
                                         ("hasaudio", "HasAudio",
                                          "If this source has audio",
                                          FALSE, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_HASVIDEO,
                                         g_param_spec_boolean
                                         ("hasvideo", "HasVideo",
                                          "If this source has video",
                                          FALSE, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_VIDEOFORMAT,
                                         g_param_spec_object
                                         ("videoformat", "VideoFormat",
                                          "VideoFormat object of this source",
                                          GDV_TYPE_VIDEOFORMAT,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_AUDIOFORMAT,
                                         g_param_spec_object
                                         ("audioformat", "AudioFormat",
                                          "AudioFormat object of this source",
                                          GDV_TYPE_AUDIOFORMAT,
                                          G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_LENGTH,
                                         g_param_spec_uint64 
                                         ("length", "Length",
                                          "Length of the source in nanoseconds",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_ERROR,
                                         g_param_spec_pointer
                                         ("error", "Error", "The GError that happened",
                                          G_PARAM_READABLE));

        ParentClass = g_type_class_ref (GST_TYPE_PIPELINE);
        gobjectclass->dispose = (gpointer) (gdv_inspector_dispose);
        gobjectclass->finalize = (gpointer) (gdv_inspector_finalize);
}

/* No more pads will be decoded. Called during preroll */
static void                    no_more_pads (GstElement *element, GdvInspector *this)
{
        GDV_DEBUG_ELEMENT ("No more pads", NULL);
        
        // if (check_if_done (this) == TRUE) {
        //        GDV_DEBUG_ELEMENT ("All resolved. Stopping inspecting", NULL);
        //        g_idle_add ((GSourceFunc) stop_inspecting, this);
        // }
}

static gboolean                bus_callback (GstBus *bus, GstMessage *message,
                                             GdvInspector *this)
{
        GDV_DEBUG_ELEMENT ("Bus callback for %s message %s", GST_OBJECT_NAME (this),
                           gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
        
        switch (GST_MESSAGE_TYPE (message)) {
                
                case GST_MESSAGE_ERROR: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_error (message, &err, &debug);
                        this->_Error = err;
                        this->_Executed = TRUE;
                        GDV_DEBUG_ELEMENT ("Error: %s (%s)", this->_Error->message, debug);
                        
                        // Call the callback if we have it
                        if (this->_ErrorFunc != NULL)
                                g_idle_add ((GSourceFunc) this->_ErrorFunc, this);
                        
                        g_source_remove (this->_Watch);
                        this->_Watch = -1;
                        
                        this->_Executed = TRUE;
                        
                }
                break;
                
                case GST_MESSAGE_EOS:
                GDV_DEBUG_ELEMENT ("EOS reached. Stopping inspecting", NULL);
                g_idle_add ((GSourceFunc) stop_inspecting, this);
                break;
                
                case GST_MESSAGE_TAG: {
                        GDV_DEBUG_ELEMENT ("Appending new set of tags", NULL);
                        GstTagList *taglist = gst_tag_list_new ();
                        gst_message_parse_tag (message, &taglist);
                        this->_Tags = gst_tag_list_merge (this->_Tags, taglist, GST_TAG_MERGE_APPEND);
                }
                break;
                
                default:
                break;
        }
                
        return TRUE;
}

/* This is called via gidle_add to prevent race conditions */
static gboolean                stop_inspecting (GdvInspector *this)
{
        GDV_DEBUG_ELEMENT ("Stopping inspecting from idle", NULL);

        if (this->_Finished == TRUE)
                return FALSE;

        this->_Finished = TRUE; 
        this->_Executed = TRUE;
        
        // only pause it if it has content and we recognized it
        // GstState end_state = (this->_HasAudio || this->_HasVideo) ? GST_STATE_PAUSED : GST_STATE_NULL;
        //gst_element_set_state ((GstElement *) this, end_state);
        //gst_element_get_state ((GstElement *) this, NULL, NULL, GST_CLOCK_TIME_NONE);
        
        // If we didn't managed to get a full info about video/audio, 
        // we assume that source has no video/audio (even if we got a pad)
        this->_HasAudio = (this->_AudioFormat != NULL) ? TRUE : FALSE;
        this->_HasVideo = (this->_VideoFormat != NULL) ? TRUE : FALSE;
                
        // FIXME: Remove watch 
        
        if (check_length (this) == TRUE) 
                GDV_DEBUG_ELEMENT ("Length: %lld (%.2fs)", this->_Length, this->_Length / (gfloat) GST_SECOND);
        
        // So we can later on dispose it...
        gst_element_set_state ((GstElement *) this, GST_STATE_NULL);
        gst_element_get_state ((GstElement *) this, NULL, NULL, GST_CLOCK_TIME_NONE);

        if (this->_HasVideo == FALSE && this->_HasAudio == FALSE) {
                gdv_errors_set_error (GDV_ERROR_NO_VIDEO_NO_AUDIO, &this->_Error);
                if (this->_ErrorFunc != NULL)
                        g_idle_add (this->_ErrorFunc, this);
        } else {
                if (this->_DoneFunc != NULL)
                        g_idle_add (this->_DoneFunc, this);
        }
        
        return FALSE;
}

/* This is called when pads are fixated on the sink end. This is our main function 
 * for learning all the nice stuff about source  */
static void                    caps_notify (GstPad *pad, gpointer param,
                                            GdvInspector *this)
{
        GDV_DEBUG_ELEMENT ("Caps notify function for pad", gst_pad_get_name (pad));
        GstCaps *caps = gst_pad_get_negotiated_caps (pad);

        if (caps == NULL) 
                return;
        
        gchar *capsstring = gst_caps_to_string (caps);
        
        GDV_DEBUG_ELEMENT ("Caps: %s", capsstring);
        parse_caps (this, caps);
                
        gst_caps_unref (caps);
        g_free (capsstring);

        // Check if we're done
        if (check_if_done (this) == TRUE) {
                GDV_DEBUG_ELEMENT ("All resolved. Stopping inspecting", NULL);
                g_idle_add ((GSourceFunc) stop_inspecting, this);
        }
}

/* New pad was added */
static void                    new_pad (GstElement *element, GstPad *pad, gboolean b, GdvInspector *this)
{
        if (this->_Executed == TRUE) return; // #FIXME - hotfix for ticket #85

        GDV_DEBUG_ELEMENT ("New pad %s, creating elements", gst_pad_get_name (pad));
        
        GstElement *Queue = gst_element_factory_make ("queue", NULL);
        g_assert (Queue != NULL);
        GstElement *Fakesink = gst_element_factory_make ("fakesink", NULL);
        g_assert (Fakesink != NULL);
        
        GDV_DEBUG_ELEMENT ("Adding & linking elements", NULL);
        gst_bin_add_many ((GstBin *) this, Queue, Fakesink, NULL);
        
        // Check if we might extract some information now
        GstCaps *caps = gst_pad_get_caps (pad);
        parse_caps (this, caps);
        
        gulong sigid = g_signal_connect (gst_element_get_pad (Queue, "sink"), "notify::caps",
                                         G_CALLBACK (caps_notify), this);
        g_assert (sigid != 0);

        gboolean l1 = gst_element_link (Queue, Fakesink);
        g_assert (l1 == TRUE);
                       
        GstPadLinkReturn l2 = gst_pad_link (pad, gst_element_get_pad (Queue, "sink"));
        g_assert (l2 == GST_PAD_LINK_OK);

        gst_element_set_state (Queue, GST_STATE_PAUSED);
        gst_element_set_state (Fakesink, GST_STATE_PAUSED);
}

/* Returns TRUE if we have all the information. False if we should rather continue inspecting */
static gboolean                check_if_done (GdvInspector *this)
{
        gboolean c1, c2;
       
        c1 = FALSE;
        if (this->_HasVideo == FALSE)
                c1 = TRUE;
        if (this->_HasVideo == TRUE && this->_VideoFormat != NULL)
                c1 = TRUE;
        
        c2 = FALSE;
        if (this->_HasAudio == FALSE)
                c2 = TRUE;
        if (this->_HasAudio == TRUE && this->_AudioFormat != NULL)   
                c2 = TRUE;
        
        if (c1 == TRUE && c2 == TRUE) 
                return TRUE;
        else
                return FALSE;
}

/* Analyze all the pads of the decodebin to find the length of the source. Return FALSE on failure. */
static gboolean                check_length (GdvInspector *this)
{
        GSList *iterator = (GSList *) GST_ELEMENT_PADS (this->_Decodebin);
        if (g_slist_length (iterator) <= 1) 
                return FALSE;
        
        GDV_DEBUG_ELEMENT ("Checking the length of the source, analyzing %d pads", g_slist_length (iterator));
         
        while (iterator != NULL)
        {
                GstPad *currentpad = iterator->data;

                if (GST_PAD_DIRECTION (currentpad) == GST_PAD_SINK) {
                        iterator = g_slist_next (iterator);
                        continue;
                }
                
                // Check length
                GstFormat fmt = GST_FORMAT_TIME;
                gint64 len = 0;
                gst_pad_query_duration (currentpad, &fmt, &len);
                
                if (fmt == GST_FORMAT_TIME && len != 0) {
                        this->_Length = len;
                        return TRUE;
                }
                
                iterator = g_slist_next (iterator);
        }
        
        // If we're here, it means we didn't manage to find it. Our last chance is 
        // the element query
        GDV_DEBUG_ELEMENT ("Trying to use the decodebin element for length query", NULL);
        
        gint64 len = 0;
        GstFormat fmt = GST_FORMAT_TIME;
        gst_element_query_duration (this->_Decodebin, &fmt, &len);
        if (fmt == GST_FORMAT_TIME && len != 0) {
                this->_Length = len;
                return TRUE;
        }
        
        return FALSE;
}

/* This is fired by the typefinder. We use it to set the mime */
static void                    have_type (GstElement *typefind, gpointer data, GstCaps *caps, GdvInspector  *this)
{
        this->_Mime = gst_caps_to_string (caps);
        GDV_DEBUG_ELEMENT ("Has type: %s", this->_Mime);
}

/* This is fired by the decodebin. Source could not be decoded */
static void                    unknown_type (GstElement *element, GstPad *pad, GstCaps *caps, GdvInspector *this)
{
        GDV_DEBUG_ELEMENT ("Pad %d has unknown type (caps: %s)", gst_pad_get_name (pad), gst_caps_to_string (caps));
}

/* Here we parse caps (either fixed or negotiated) and create all the format info */
static void                    parse_caps (GdvInspector *this, GstCaps *caps)
{
        g_assert (caps != NULL);
        GDV_DEBUG_ELEMENT ("Parsing caps %s", gst_caps_to_string (caps));
        
        if (gst_caps_is_any (caps) == TRUE) {
                GDV_DEBUG_ELEMENT ("ANY caps, aborting", NULL);
                return;
        }
        
        GstStructure *str;
        str = gst_caps_get_structure (caps, 0);
        if (str == NULL) {
                GDV_WARNING_ELEMENT ("No structure in caps", NULL);
                return;
        }
        
        if (g_strrstr (gst_structure_get_name (str), "video")) {
                GDV_DEBUG_ELEMENT ("Got some video info in caps", NULL);
                this->_HasVideo = TRUE;
                
                if (this->_VideoFormat == NULL && gdv_videoformat_caps_ok (caps))
                        this->_VideoFormat = gdv_videoformat_new (caps);
        }
        
        if (g_strrstr (gst_structure_get_name (str), "audio")) {
                GDV_DEBUG_ELEMENT ("Got some audio info in caps", NULL);
                this->_HasAudio = TRUE;
                
                if (this->_AudioFormat == NULL && gdv_audioformat_caps_ok (caps))
                        this->_AudioFormat = gdv_audioformat_new (caps);
        }
}

/* GObject get property */
static void                    gdv_inspector_get_property (GdvInspector *this, guint propid,
                                                           GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_INSPECTOR (this));
        g_return_if_fail (this->_Executed == TRUE);
        
        switch (propid) {

	        case ARG_URL:
                g_value_set_string (value, this->_Url);
                break;
                
                case ARG_MIME:
                g_value_set_string (value, this->_Mime);
                break;
                
                case ARG_VIDEOFORMAT:
                g_assert (this->_VideoFormat != NULL);
                g_value_set_object (value, this->_VideoFormat);
                break;

                case ARG_AUDIOFORMAT:
                g_assert (this->_AudioFormat != NULL);
                g_value_set_object (value, this->_AudioFormat);
                break;
                
                case ARG_HASVIDEO:
                g_value_set_boolean (value, this->_HasVideo);
                break;
                
                case ARG_HASAUDIO:
                g_value_set_boolean (value, this->_HasAudio);
                break;
                
                case ARG_LENGTH:
                g_value_set_uint64 (value, this->_Length);
                break;
                
                case ARG_ERROR:
                g_return_if_fail (this->_Error != NULL);
                g_value_set_pointer (value, this->_Error);
                break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}
