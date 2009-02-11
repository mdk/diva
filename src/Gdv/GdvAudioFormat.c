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

#include "GdvAudioFormat.h"
#include "GdvAudioFormatPrivate.h"

/* GObject type */
GType                           gdv_audioformat_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvAudioFormatClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_audioformat_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvAudioFormat),
                        32,
                        (GInstanceInitFunc) gdv_audioformat_init,
                };
                objecttype = g_type_register_static
                (G_TYPE_OBJECT, "GdvAudioFormat", &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_audioformat_class_init (GdvAudioFormatClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        g_type_class_ref (G_TYPE_OBJECT);
        // FIXME: Unref it? 

        gobjectclass->get_property = (gpointer) gdv_audioformat_get_property;
        gobjectclass->set_property = (gpointer) gdv_audioformat_set_property;

        g_object_class_install_property (gobjectclass, ARG_SAMPLERATE,
					 g_param_spec_int
					 ("samplerate", "Samplerate",
					  "Samples per second",
					  0, 96000, 0, G_PARAM_READWRITE));

	g_object_class_install_property (gobjectclass, ARG_DEPTH,
					 g_param_spec_int
					 ("depth", "Depth",
					  "Bit depth of a single sample",
					  0, 4096, 0, G_PARAM_READWRITE));

        g_object_class_install_property (gobjectclass, ARG_CHANNELS,
					 g_param_spec_int
					 ("channels", "Channels",
					  "Number of channels",
					  0, 10, 0, G_PARAM_READWRITE));
        
        g_object_class_install_property (gobjectclass, ARG_CAPS,
                                         g_param_spec_pointer
					 ("caps", "Caps",
					  "The GstCaps of this format",
					  G_PARAM_READABLE));

        gobjectclass->dispose = (gpointer) gdv_audioformat_dispose;
        gobjectclass->finalize = (gpointer) gdv_audioformat_finalize;
}

/* GObject init */
static void                     gdv_audioformat_init (GdvAudioFormat *this)
{
        this->Private = NULL;

        GdvAudioFormatPrivate *priv = g_new0 (GdvAudioFormatPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Check if the given caps contain all the information that is needed to create
 * a valid audio format */
gboolean                        gdv_audioformat_caps_ok (GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, FALSE);

        // Kick some initial assumptions
        if (gst_caps_is_any (caps) || gst_caps_is_empty (caps))
                return FALSE;

        if (! gst_caps_is_fixed (caps))
                return FALSE;

        GstStructure *struc = gst_caps_get_structure (caps, 0);

        // Check for availibility of all the crucial fields
        if (! gst_structure_has_field_typed (struc, "rate", G_TYPE_INT))
                return FALSE;
        
        if (! gst_structure_has_field_typed (struc, "depth", G_TYPE_INT) &&
            ! gst_structure_has_field_typed (struc, "width", G_TYPE_INT))
                return FALSE;

        if (! gst_structure_has_field_typed (struc, "channels", G_TYPE_INT))
                return FALSE;

        return TRUE;
}

/* Create a new instance */
GdvAudioFormat*                 gdv_audioformat_new (GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, NULL);
        g_return_val_if_fail (gdv_audioformat_caps_ok (caps), NULL);
        
        GdvAudioFormat *this = g_object_new (GDV_TYPE_AUDIOFORMAT, NULL);
        GDV_DEBUG_EXT ("Created a AudioFormat based on caps %s", gst_caps_to_string (caps));

        this->Private->Caps = gst_caps_copy (caps);

        return this;
}

/* Create from a string describing the caps (GstCaps) */
GdvAudioFormat*                 gdv_audioformat_new_from_string (const gchar *string)
{
        g_return_val_if_fail (string != NULL, NULL);
        GDV_DEBUG_EXT ("Creating new AudioFormat from string %s", string);
        
        GstCaps *caps = NULL;
        caps = gst_caps_from_string (string);
        g_return_val_if_fail (caps != NULL, NULL);

        return gdv_audioformat_new (caps);
}

GdvAudioFormat*                 gdv_audioformat_new_default (void)
{
        GstCaps *acaps = gst_caps_new_simple ("audio/x-raw-int",
                                              "endianness", G_TYPE_INT, 1234,
                                              "rate", G_TYPE_INT, 48000,
                                              "width", G_TYPE_INT, 16,
                                              "depth", G_TYPE_INT, 16,
                                              "channels", G_TYPE_INT, 2,
                                              "signed", G_TYPE_BOOLEAN, TRUE, 
                                              NULL);
        
        GdvAudioFormat *aformat = gdv_audioformat_new (acaps);
        gst_caps_unref (acaps);

        return aformat;
}

/* Save the format to a string, that can be later on used to bring the format 
 * back */
gchar*                          gdv_audioformat_serialize_to_string (GdvAudioFormat *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOFORMAT (this), NULL);
        g_return_val_if_fail (this->Private->Caps != NULL, NULL);
        GDV_DEBUG_EXT ("Serializing AudioFormat to string", NULL);
        
        return gst_caps_to_string (this->Private->Caps);
}

/* GObject dispose */
void                            gdv_audioformat_dispose (GdvAudioFormat *this)
{
        g_return_if_fail (GDV_IS_AUDIOFORMAT (this));
        GDV_DEBUG_EXT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this->Private);
}

/* GObject finalize */
void                            gdv_audioformat_finalize (GdvAudioFormat *this)
{
        g_return_if_fail (GDV_IS_AUDIOFORMAT (this));
        GDV_DEBUG_EXT ("Finalizing", NULL);

        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
}        

/* GObject get property */
void                            gdv_audioformat_get_property (GdvAudioFormat *this, guint propid,
                                                              GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_AUDIOFORMAT (this));
        
        GstStructure *structure = NULL;
        if (this->Private->Caps != NULL)
                structure = gst_caps_get_structure (this->Private->Caps, 0);
        
        g_return_if_fail (structure != NULL);
        
        switch (propid) {
                
	        case ARG_SAMPLERATE: {
                        gint32 rate = -1;
                        gst_structure_get_int (structure, "rate", &rate);
                        g_assert (rate != -1);
                        
                        g_value_set_int (value, rate);
                } break;
                        
	        case ARG_DEPTH: {
                        gint32 depth = -1;
                        gst_structure_get_int (structure, "depth", &depth);
                        // FIXME: A hack or what?
                        if (depth == -1)
                                gst_structure_get_int (structure, "width", &depth);
                        
                        g_assert (depth != -1);

                        g_value_set_int (value, depth);
                } break;

                case ARG_CHANNELS: {
                        gint32 channels = -1;
                        gst_structure_get_int (structure, "channels", &channels);
                        g_assert (channels != -1);

                        g_value_set_int (value, channels);
                } break;
                
                case ARG_CAPS: {
                        g_assert (this->Private->Caps != NULL);
                        g_value_set_pointer (value, this->Private->Caps);
                        gst_caps_ref (this->Private->Caps);
                } break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }

}

/* GObject set property */
void                            gdv_audioformat_set_property (GdvAudioFormat *this, guint propid,
                                                              const GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_AUDIOFORMAT (this));

        GstStructure *structure = NULL;
        if (this->Private->Caps != NULL)
                structure = gst_caps_get_structure (this->Private->Caps, 0);
        
        g_return_if_fail (structure != NULL);
        
        switch (propid) {
                
                case ARG_SAMPLERATE: {
                        int samplerate = g_value_get_int (value);
                        g_return_if_fail (samplerate > 0);

                        gst_structure_set (structure, "rate", G_TYPE_INT, samplerate, NULL);
                } break;
                        
                case ARG_DEPTH: {
                        int depth = g_value_get_int (value);
                        g_return_if_fail (depth > 0);

                        gst_structure_set (structure, "depth", G_TYPE_INT, depth, NULL);
                        gst_structure_set (structure, "width", G_TYPE_INT, depth, NULL);
                } break;

                case ARG_CHANNELS: {
                        int channels = g_value_get_int (value);
                        g_return_if_fail (channels != 1 || channels != 2);

                        gst_structure_set (structure, "channels", G_TYPE_INT, channels, NULL);
                } break;

                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

GdvAudioFormat*                 gdv_audioformat_clone (GdvAudioFormat *this)
{
        g_return_val_if_fail (GDV_IS_AUDIOFORMAT (this), NULL);

        GDV_DEBUG_EXT ("Cloning audioformat %s", gst_caps_to_string (this->Private->Caps));

        return gdv_audioformat_new (this->Private->Caps);
}

/* Private init */
static gboolean                 private_init (GdvAudioFormatPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        private->Caps = NULL;

        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvAudioFormatPrivate *private)
{
        g_return_if_fail (private != NULL);

        if (private->Caps != NULL) {
                gst_caps_unref (private->Caps);
                private->Caps = NULL;
        }
}
