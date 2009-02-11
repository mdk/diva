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

#include "DivaPluginsWavExporter.h"
#include "DivaPluginsWavExporterPrivate.h"

static GdvExporterClass*        ParentClass = NULL;

/* GObject type */
GType                           diva_plugins_wav_exporter_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (DivaPluginsWavExporterClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) diva_plugins_wav_exporter_class_init, 
                        NULL,
                        NULL, 
                        sizeof (DivaPluginsWavExporter),
                        2, 
                        (GInstanceInitFunc) diva_plugins_wav_exporter_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_EXPORTER, "DivaPluginsWavExporter", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     diva_plugins_wav_exporter_class_init (DivaPluginsWavExporterClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
        
        gobjectclass->dispose = (gpointer) diva_plugins_wav_exporter_dispose;
        gobjectclass->finalize = (gpointer) diva_plugins_wav_exporter_finalize;
}

/* GObject dispose */
static void                     diva_plugins_wav_exporter_dispose (DivaPluginsWavExporter *this)
{
        g_return_if_fail (DIVA_PLUGINS_IS_WAV_EXPORTER (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
}

/* GObject finalize */
static void                     diva_plugins_wav_exporter_finalize (DivaPluginsWavExporter *this)
{
        g_return_if_fail (DIVA_PLUGINS_IS_WAV_EXPORTER (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
}

/* GObject init */
static void                     diva_plugins_wav_exporter_init (DivaPluginsWavExporter *this)
{
        g_return_if_fail (DIVA_PLUGINS_IS_WAV_EXPORTER (this));
        this->Private = NULL;

        DivaPluginsWavExporterPrivate *priv = g_new0 (DivaPluginsWavExporterPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Private init */
static gboolean                 private_init (DivaPluginsWavExporter *this,
                                              DivaPluginsWavExporterPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);
        
        private->WavEnc = gst_element_factory_make ("wavenc", NULL);
        g_return_val_if_fail (private->WavEnc != NULL, FALSE);

        // Now the audio elements
        private->AudioConvert = gst_element_factory_make ("audioconvert", NULL);
        g_return_val_if_fail (private->AudioConvert != NULL, FALSE);
        private->AudioResample = gst_element_factory_make ("audioresample", NULL);
        g_return_val_if_fail (private->AudioResample != NULL, FALSE);

        // Fake sink for video
        private->VFakeSink = gst_element_factory_make ("fakesink", NULL);
        g_return_val_if_fail (private->VFakeSink != NULL, FALSE);

        // Output
        private->FileSink = gst_element_factory_make ("filesink", NULL);
        g_return_val_if_fail (private->FileSink != NULL, FALSE);

        // Caps filter
        private->CapsFilter = gst_element_factory_make ("capsfilter", NULL);
        g_return_val_if_fail (private->CapsFilter != NULL, FALSE);

        // Add to the bin
        gst_bin_add_many (GST_BIN (this),
                          private->AudioResample, private->AudioConvert,
                          private->WavEnc, private->VFakeSink, 
                          private->FileSink, private->CapsFilter,
                          NULL);

        // Link it all
        g_return_val_if_fail (gst_element_link (private->AudioConvert, private->AudioResample), FALSE);
        g_return_val_if_fail (gst_element_link (private->AudioResample, private->CapsFilter), FALSE);
        g_return_val_if_fail (gst_element_link (private->CapsFilter, private->WavEnc), FALSE);
        g_return_val_if_fail (gst_element_link (private->WavEnc, private->FileSink), FALSE);
        
        // Set the ghost target
        GstPad *VSink;
        GstPad *ASink;
        gdv_exporter_get_pads (GDV_EXPORTER (this), &VSink, &ASink);
        g_return_val_if_fail (gst_ghost_pad_set_target (GST_GHOST_PAD (VSink), gst_element_get_pad (private->VFakeSink, "sink")),
                              FALSE);
        g_return_val_if_fail (gst_ghost_pad_set_target (GST_GHOST_PAD (ASink), gst_element_get_pad (private->AudioConvert, "sink")),
                              FALSE);
                
        return TRUE;
}

/* Private dispose */
static void                     private_dispose (DivaPluginsWavExporter *this,
                                                 DivaPluginsWavExporterPrivate *private)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (DIVA_PLUGINS_IS_WAV_EXPORTER (this));
}

/* Create a new instance. File name is used as the export target */
DivaPluginsWavExporter*         diva_plugins_wav_exporter_new (gchar *filename)
{
        DivaPluginsWavExporter *this = g_object_new (DIVA_PLUGINS_TYPE_WAV_EXPORTER, NULL);
        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else {
                g_object_set (G_OBJECT (this->Private->FileSink), "location", filename, NULL);
                return this;
        }
}

void                            diva_plugins_wav_exporter_set_format (DivaPluginsWavExporter *this, GdvAudioFormat *format)
{
        g_return_if_fail (DIVA_PLUGINS_IS_WAV_EXPORTER (this));
        g_return_if_fail (GDV_IS_AUDIOFORMAT (format));

        GstCaps *caps;

        g_object_get (G_OBJECT (format), "caps", &caps, NULL);
        g_return_if_fail (caps != NULL);

        g_object_set (G_OBJECT (this->Private->CapsFilter), "caps", caps, NULL);
        gst_caps_unref (caps);
}
