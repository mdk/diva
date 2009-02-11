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

/* This is an abstract class.
 * Source is a basic provider of video/audio data. Source is built on the top
 * of the GstBin and can be inserted directly into GStreamer pipelines.
 * Each source provides a method to clone itself.
 * Child classes should provide custom gst functions etc.
 * Source consistent if sending event to one pad == sending event to both pads.
 * This is true for all (?) file-based sources
 */

#include "GdvSource.h"
#include "GdvSourcePrivate.h"
#include "GdvSourceProtected.h"

static GstElementClass*         ParentClass = NULL;

/* GObject Type */
GType                           gdv_source_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvSourceClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_source_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvSource),
                        32,
                        (GInstanceInitFunc) gdv_source_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvSource", &objectinfo, 0);
        }

        return objecttype;
}

/* GObject init */
static void                     gdv_source_class_init (GdvSourceClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) klass;

        ParentClass = g_type_class_ref (GST_TYPE_BIN);
        gobjectclass->get_property = (gpointer) (gdv_source_get_property);
        
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
        
        g_object_class_install_property (gobjectclass, ARG_CONSISTENT,
                                         g_param_spec_boolean
                                         ("consistent", "Consistent",
                                          "If this source is consistent",
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
          
        // Some custom element functions
        gobjectclass->dispose = (gpointer) gdv_source_dispose;
        gobjectclass->finalize = (gpointer) gdv_source_finalize;
        elementclass->send_event = (gpointer) gdv_source_send_event_stock;

        // Class methods
        klass->CloneFunc = NULL;
}

static gboolean                 protected_init (GdvSource *this, GdvSourceProtected *protected)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        g_return_val_if_fail (protected != NULL, FALSE);
        
        protected->Length = GDV_ZEROTIME;
        
        protected->HasVideo = FALSE;
        protected->VideoFormat = NULL;
        protected->HasAudio = FALSE;
        protected->AudioFormat = NULL;
        protected->Consistent = TRUE;

        return TRUE;
}

static gboolean                 private_init (GdvSource *this, GdvSourcePrivate *private)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);
        
        // Create ghost pads
        private->VSrc = gdv_sourcepad_new_no_target ("vsrc");
        g_return_val_if_fail (private->VSrc != NULL, FALSE);
        gst_element_add_pad (GST_ELEMENT (this), (GstPad *) private->VSrc);
        
        private->ASrc = gdv_sourcepad_new_no_target ("asrc");
        g_return_val_if_fail (private->ASrc != NULL, FALSE);
        gst_element_add_pad (GST_ELEMENT (this), (GstPad *) private->ASrc);

        return TRUE;
}

/* GObject init */
static void                     gdv_source_init (GdvSource *this)
{
        GDV_DEBUG_ELEMENT ("Initializing fresh GdvSource object", NULL);
        
        g_return_if_fail (GDV_IS_SOURCE (this));
        this->Private = NULL;
        this->Protected = NULL;

        GdvSourcePrivate *priv = g_new0 (GdvSourcePrivate, 1);
        g_return_if_fail (priv != NULL);

        GdvSourceProtected *prot = g_new0 (GdvSourceProtected, 1);
        g_return_if_fail (prot != NULL);

        if (private_init (this, priv) == TRUE && protected_init (this, prot) == TRUE) { 
                this->Private = priv;
                this->Protected = prot;
        } else {
                private_dispose (this, priv);
                protected_dispose (this, prot);

                g_free (priv);
                g_free (prot);
                this->Private = NULL;
                this->Protected = NULL;
        }
}

/* GObject get property */
static void                     gdv_source_get_property (GdvSource *this, guint propid,
                                                         GValue *value, GParamSpec *pspec)
{
        switch (propid) {

                case ARG_HASVIDEO:
                        g_value_set_boolean (value, this->Protected->HasVideo);
                        break;
                
                case ARG_HASAUDIO:
                        g_value_set_boolean (value, this->Protected->HasAudio);
                        break;
                        
                case ARG_CONSISTENT:
                        g_value_set_boolean (value, this->Protected->Consistent);
                        break;
                        
                case ARG_VIDEOFORMAT:
                        g_return_if_fail (this->Protected->VideoFormat != NULL);
                        g_value_set_object (value, this->Protected->VideoFormat);
                        break;
                        
                case ARG_AUDIOFORMAT:
                        g_return_if_fail (this->Protected->AudioFormat != NULL);
                        g_value_set_object (value, this->Protected->AudioFormat);
                        break;
                        
                case ARG_LENGTH:
                        g_value_set_uint64 (value, this->Protected->Length);
                        break;
                        
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                        break;
        }
}

/* GObject dispose */
static void                     gdv_source_dispose (GdvSource *this)
{
        g_return_if_fail (GDV_IS_SOURCE (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this, this->Private);

        if (this->Protected != NULL)
                protected_dispose (this, this->Protected);
        
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_source_finalize (GdvSource *this)
{
        g_return_if_fail (GDV_IS_SOURCE (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }

        if (this->Protected != NULL) {
                g_free (this->Protected);
                this->Protected = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Link this source to given sinks */
gboolean                        gdv_source_link_to_sinks (GdvSource *this, GstElement *vsink, GstElement *asink)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        
        if (vsink == NULL && asink == NULL) {
                GDV_WARNING ("No sinks specified!", NULL);
                return FALSE;
        }
        
        if (this->Private->VSrc == NULL && this->Private->ASrc == NULL) {
                GDV_WARNING ("No VSrc or ASrc pads!", NULL);
                return FALSE;
        }
                        
        if (vsink != NULL && this->Private->VSrc != NULL) {
                GDV_DEBUG_ELEMENT ("Linking video to %s:sink", gst_element_get_name (vsink));
                if (GST_PAD_LINK_FAILED (gst_element_link_pads (GST_ELEMENT (this), "vsrc", vsink, "sink"))) {
                        GDV_WARNING_ELEMENT ("Failed to link vsrc to sink!", NULL);
                        return FALSE;
                }
        }

        if (asink != NULL && this->Private->ASrc != NULL) {
                GDV_DEBUG_ELEMENT ("Linking audio to %s:sink", gst_element_get_name (asink));
                if (GST_PAD_LINK_FAILED (gst_element_link_pads (GST_ELEMENT (this), "asrc", asink, "sink"))) {
                        GDV_WARNING_ELEMENT ("Failed to link asrc to sink!", NULL);
                        return FALSE;
                }
        }
        
        return TRUE;
}

/* Unlink this source */
gboolean                        gdv_source_unlink (GdvSource *this)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        gboolean unlinkres = TRUE;

        if (GST_PAD_IS_LINKED (this->Private->VSrc)) {
                GDV_DEBUG_ELEMENT ("Unlinking vsrc pad", NULL);
                gboolean ul1 = gst_pad_unlink ((GstPad *) this->Private->VSrc, GST_PAD_PEER (this->Private->VSrc));
                unlinkres = ul1;
        }

        if (GST_PAD_IS_LINKED (this->Private->ASrc)) {
                GDV_DEBUG_ELEMENT ("Unlinking asrc pad", NULL);
                gboolean ul2 = gst_pad_unlink ((GstPad *) this->Private->ASrc, GST_PAD_PEER (this->Private->ASrc));
                unlinkres = unlinkres && ul2;
        }

        return unlinkres;
}

/* Clone this source */
GdvSource*                      gdv_source_clone (GdvSource *this)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), NULL);
                
        GdvSourceClass *klass = (GdvSourceClass *) G_OBJECT_GET_CLASS(this);
        g_assert (klass != NULL);
        g_return_val_if_fail (klass->CloneFunc != NULL, NULL);
        
        GDV_DEBUG_ELEMENT ("Cloning", NULL);

        return klass->CloneFunc (this);
}

/* A handy wrapper function to get frame dimensions of a thumbnail */
void                            gdv_source_get_thumb_dimensions (GdvSource *this, GdvFrameDimensions *dimensions)
{
        GDV_DEBUG_ELEMENT ("Getting thumbnail frame dimensions", NULL);

        g_return_if_fail (GDV_IS_SOURCE (this));
        g_return_if_fail (dimensions != NULL);
        g_return_if_fail (this->Protected->VideoFormat != NULL);
        
        GdvFrameDimensions *orig_dimensions = NULL;
        g_object_get (G_OBJECT (this->Protected->VideoFormat), "framedimensions", &orig_dimensions, NULL);
        g_assert (dimensions != NULL);
        
        gdv_framedimensions_fit_h (orig_dimensions, 50, dimensions);
        GDV_DEBUG_ELEMENT ("Thumbnail dimensions: %dx%d", dimensions->Width, dimensions->Height);
}

/* A handy wrapper function to get small frame dimensions of a thumbnail */
void                            gdv_source_get_small_thumb_dimensions (GdvSource *this, GdvFrameDimensions *dimensions)
{
        GDV_DEBUG_ELEMENT ("Getting small thumbnail frame dimensions", NULL);

        g_return_if_fail (GDV_IS_SOURCE (this));
        g_return_if_fail (dimensions != NULL);
        g_return_if_fail (this->Protected->VideoFormat != NULL);
                
        GdvFrameDimensions *orig_dimensions = NULL;
        g_object_get (G_OBJECT (this->Protected->VideoFormat), "framedimensions", &orig_dimensions, NULL);
        g_assert (dimensions != NULL);
        
        gdv_framedimensions_fit_h (orig_dimensions, 32, dimensions);
        GDV_DEBUG_ELEMENT ("Thumbnail dimensions: %dx%d", dimensions->Width, dimensions->Height);
}

/* Private dispose */
static void                     private_dispose (GdvSource *this, GdvSourcePrivate *private)
{
        g_return_if_fail (private != NULL);
        
        if (this->Private->VSrc != NULL) {
                gdv_sourcepad_loose (this->Private->VSrc);
                this->Private->VSrc = NULL;
        }
        
        if (this->Private->ASrc != NULL) {
                gdv_sourcepad_loose (this->Private->ASrc);
                this->Private->ASrc = NULL;
        }
}

/* Protected dispose */
static void                     protected_dispose (GdvSource *this, GdvSourceProtected *protected)
{
        g_return_if_fail (protected != NULL);

        if (this->Protected->VideoFormat != NULL) {
                g_object_unref (this->Protected->VideoFormat);
                this->Protected->VideoFormat = NULL;
        }
        
        if (this->Protected->AudioFormat != NULL) {
                g_object_unref (this->Protected->AudioFormat);
                this->Protected->AudioFormat = NULL;
        }
}

gboolean                        gdv_source_set_audioformat (GdvSource *this, GdvAudioFormat *aformat)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        GDV_DEBUG_ELEMENT ("Seetting source AudioFormat", NULL);
        
        this->Protected->HasAudio = TRUE;
        if (this->Protected->AudioFormat != NULL)
                g_object_unref (this->Protected->AudioFormat);
        
        this->Protected->AudioFormat = aformat;
        g_object_ref (aformat);
                
        return gdv_sourcepad_set_audioformat (this->Private->ASrc, aformat);
}

gboolean                        gdv_source_set_videoformat (GdvSource *this, GdvVideoFormat *vformat)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        GDV_DEBUG_ELEMENT ("Seetting source VideoFormat", NULL);
        
        this->Protected->HasVideo = TRUE;
        if (this->Protected->VideoFormat != NULL)
                g_object_unref (this->Protected->VideoFormat);
        
        this->Protected->VideoFormat = vformat;
        g_object_ref (vformat);
                
        return gdv_sourcepad_set_videoformat (this->Private->VSrc, vformat);
}

gboolean                        gdv_source_set_video_target (GdvSource *this, GstPad *pad)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);

        if (pad != NULL) {
                GDV_DEBUG_ELEMENT ("Seeting video target to pad %s", GST_PAD_NAME (pad));
        
                if (! gdv_sourcepad_has_target (this->Private->VSrc))
                        return gdv_sourcepad_set_target (this->Private->VSrc, pad);
                else {
                        GDV_WARNING_ELEMENT ("VSrc pad already has target!", NULL);
                        return FALSE;
                }
        } else {
                GDV_DEBUG_ELEMENT ("Unsetting video target pad", NULL);
          
                if (gdv_sourcepad_has_target (this->Private->VSrc) == FALSE)
                        return TRUE;
                else {
                        return gdv_sourcepad_untarget (this->Private->VSrc);
                }
        }
}

gboolean                        gdv_source_set_audio_target (GdvSource *this, GstPad *pad)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        
        if (pad != NULL) {
                GDV_DEBUG_ELEMENT ("Seeting audio target to pad %s", GST_PAD_NAME (pad));
                if (! gdv_sourcepad_has_target (this->Private->ASrc))
                        return gdv_sourcepad_set_target (this->Private->ASrc, pad);
                else {
                        GDV_WARNING_ELEMENT ("ASrc pad already has target!", NULL);
                        return FALSE;
                }
        } else {
                 GDV_DEBUG_ELEMENT ("Unsetting audio target pad", NULL);
                if (gdv_sourcepad_has_target (this->Private->ASrc) == FALSE)
                        return TRUE;
                else {
                        return gdv_sourcepad_untarget (this->Private->ASrc);
                }
        }
}

gboolean                        gdv_source_set_length (GdvSource *this, GdvTime length)
{
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        GDV_DEBUG_ELEMENT ("Setting length (+on pads) to %s", gdv_time_to_string (length));

        this->Protected->Length = length;

        if (this->Protected->HasAudio)
                gdv_sourcepad_set_source_length (this->Private->ASrc, length);

        if (this->Protected->HasVideo)
                gdv_sourcepad_set_source_length (this->Private->VSrc, length);

        return TRUE;
}

gboolean                        gdv_source_send_event_stock (GdvSource *this, GstEvent *event)
{
        // FIXME: Should check for running?
        g_return_val_if_fail (GDV_IS_SOURCE (this), FALSE);
        g_return_val_if_fail (this->Private->VSrc != NULL || this->Private->ASrc != NULL, FALSE);

        if (this->Protected->Consistent) {
                GstPad *pad = (GstPad *) ((this->Protected->HasVideo == TRUE) ? this->Private->VSrc : this->Private->ASrc);
                GDV_DEBUG_ELEMENT ("Consistent source, sending event to '%s' pad", GST_PAD_NAME (pad));
                return gst_pad_send_event (pad, event);
        } else {
                gboolean res = TRUE;
                if (this->Protected->HasVideo) {
                        GDV_DEBUG_ELEMENT ("Not consistent source, copying/sending event to 'vsrc' pad", NULL);
                        res &= gst_pad_send_event (GST_PAD (this->Private->VSrc), gst_event_copy (event));
                }

                if (this->Protected->HasAudio) {
                        GDV_DEBUG_ELEMENT ("Not consistent source, copying/sending event to 'asrc' pad", NULL);
                        res &= gst_pad_send_event (GST_PAD (this->Private->ASrc), gst_event_copy (event));
                }

                return res;
        }
}
