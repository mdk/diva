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

#include "DivaPluginsTheoraExporter.h"
#include "DivaPluginsTheoraExporterPrivate.h"

static GdvExporterClass*        ParentClass = NULL;

/* GObject type */
GType                           diva_plugins_theora_exporter_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (DivaPluginsTheoraExporterClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) diva_plugins_theora_exporter_class_init, 
                        NULL,
                        NULL, 
                        sizeof (DivaPluginsTheoraExporter),
                        2, 
                        (GInstanceInitFunc) diva_plugins_theora_exporter_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_EXPORTER, "DivaPluginsTheoraExporter", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     diva_plugins_theora_exporter_class_init (DivaPluginsTheoraExporterClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
        
        gobjectclass->dispose = (gpointer) diva_plugins_theora_exporter_dispose;
        gobjectclass->finalize = (gpointer) diva_plugins_theora_exporter_finalize;
}

/* GObject dispose */
static void                     diva_plugins_theora_exporter_dispose (DivaPluginsTheoraExporter *this)
{
        g_return_if_fail (DIVA_PLUGINS_IS_THEORA_EXPORTER (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
}

/* GObject finalize */
static void                     diva_plugins_theora_exporter_finalize (DivaPluginsTheoraExporter *this)
{
        g_return_if_fail (DIVA_PLUGINS_IS_THEORA_EXPORTER (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
}

/* GObject init */
static void                     diva_plugins_theora_exporter_init (DivaPluginsTheoraExporter *this)
{
        g_return_if_fail (DIVA_PLUGINS_IS_THEORA_EXPORTER (this));
        this->Private = NULL;

        DivaPluginsTheoraExporterPrivate *priv = g_new0 (DivaPluginsTheoraExporterPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Private init */
static gboolean                 private_init (DivaPluginsTheoraExporter *this,
                                              DivaPluginsTheoraExporterPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        // First the video pipeline
        private->ColorSpace = gst_element_factory_make ("ffmpegcolorspace", NULL);
        g_return_val_if_fail (private->ColorSpace != NULL, FALSE);

        private->TheoraEnc = gst_element_factory_make ("theoraenc", NULL);
        g_return_val_if_fail (private->TheoraEnc != NULL, FALSE);

        // Now the audio elements
        private->AudioConvert = gst_element_factory_make ("audioconvert", NULL);
        g_return_val_if_fail (private->AudioConvert != NULL, FALSE);

        private->AudioResample = gst_element_factory_make ("audioresample", NULL);
        g_return_val_if_fail (private->AudioResample != NULL, FALSE);

        private->VorbisEnc = gst_element_factory_make ("vorbisenc", NULL);
        g_return_val_if_fail (private->VorbisEnc != NULL, FALSE);

        // Shared elements
        private->OggMux = gst_element_factory_make ("oggmux", NULL);
        g_return_val_if_fail (private->OggMux != NULL, FALSE);

        private->FileSink = gst_element_factory_make ("filesink", NULL);
        g_return_val_if_fail (private->FileSink != NULL, FALSE);

        // Video scale
        private->VideoScale = gst_element_factory_make ("videoscale", NULL);
        g_return_val_if_fail (private->VideoScale != NULL, FALSE);

        // Caps filters
        private->VCapsFilter = gst_element_factory_make ("capsfilter", NULL);
        g_return_val_if_fail (private->VCapsFilter != NULL, FALSE);

        private->ACapsFilter = gst_element_factory_make ("capsfilter", NULL);
        g_return_val_if_fail (private->ACapsFilter != NULL, FALSE);

        // Add to the bin
        gst_bin_add_many (GST_BIN (this),
                          private->VideoScale, 
                          private->ColorSpace, private->TheoraEnc,
                          private->AudioConvert, private->VorbisEnc,
                          private->AudioResample, 
                          private->OggMux, private->FileSink,
                          private->VCapsFilter, private->ACapsFilter, 
                          NULL);

        // Link it all
        g_return_val_if_fail (gst_element_link (private->VideoScale, private->VCapsFilter), FALSE);
        g_return_val_if_fail (gst_element_link (private->VCapsFilter, private->ColorSpace), FALSE);
        g_return_val_if_fail (gst_element_link (private->ColorSpace, private->TheoraEnc), FALSE);
        g_return_val_if_fail (gst_element_link (private->ACapsFilter, private->VorbisEnc), FALSE);
        g_return_val_if_fail (gst_element_link (private->AudioResample, private->AudioConvert), FALSE);
        g_return_val_if_fail (gst_element_link (private->AudioConvert, private->ACapsFilter), FALSE);
        g_return_val_if_fail (gst_element_link (private->TheoraEnc, private->OggMux), FALSE);
        g_return_val_if_fail (gst_element_link (private->VorbisEnc, private->OggMux), FALSE);
        g_return_val_if_fail (gst_element_link (private->OggMux, private->FileSink), FALSE);

        // Set the ghost target
        GstPad *VSink;
        GstPad *ASink;
        gdv_exporter_get_pads (GDV_EXPORTER (this), &VSink, &ASink);
        g_return_val_if_fail (gst_ghost_pad_set_target (GST_GHOST_PAD (VSink), gst_element_get_pad (private->VideoScale, "sink")),
                              FALSE);
        g_return_val_if_fail (gst_ghost_pad_set_target (GST_GHOST_PAD (ASink), gst_element_get_pad (private->AudioResample, "sink")),
                              FALSE);
                
        return TRUE;
}

/* Private dispose */
static void                     private_dispose (DivaPluginsTheoraExporter *this,
                                                 DivaPluginsTheoraExporterPrivate *private)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (DIVA_PLUGINS_IS_THEORA_EXPORTER (this));
}

/* Create a new instance. Wile name is used as the export target */
DivaPluginsTheoraExporter*      diva_plugins_theora_exporter_new (gchar *filename,
                                                                  GdvVideoFormat *vformat, GdvAudioFormat *aformat,
                                                                  gint32 vQuality, gfloat aQuality)
{
        DivaPluginsTheoraExporter *this = g_object_new (DIVA_PLUGINS_TYPE_THEORA_EXPORTER, NULL);
        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else {
                private_set (this->Private, filename, vformat, aformat, vQuality, aQuality);
                return this;
        }
}

static void                     private_set (DivaPluginsTheoraExporterPrivate *private, gchar *filename,
                                             GdvVideoFormat *vformat, GdvAudioFormat *aformat,
                                             gint32 vQuality, gfloat aQuality)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (filename != NULL);
        g_return_if_fail (GDV_IS_VIDEOFORMAT (vformat));
        g_return_if_fail (GDV_IS_AUDIOFORMAT (aformat));

        GstCaps *vcaps = NULL;
        GstCaps *acaps = NULL;
        GstCaps *convertedcaps = NULL;
        
        // Set the filesink, I *think* it's copied here
        g_object_set (G_OBJECT (private->FileSink), "location", filename, NULL);

        // Set the video filter caps
        g_object_get (G_OBJECT (vformat), "caps", &vcaps, NULL);
        g_return_if_fail (vcaps != NULL);
        g_object_set (G_OBJECT (private->VCapsFilter), "caps", vcaps, NULL);
        gst_caps_unref (vcaps);

        // Set the audio filter caps
        g_object_get (G_OBJECT (aformat), "caps", &acaps, NULL);
        g_return_if_fail (acaps != NULL);

        convertedcaps = convert_format_caps (acaps);
                
        g_object_set (G_OBJECT (private->ACapsFilter), "caps", convertedcaps, NULL);
        gst_caps_unref (acaps);
        gst_caps_unref (convertedcaps);
        
        // Set the video quality
        g_object_set (G_OBJECT (private->TheoraEnc), "quality", vQuality, NULL);

        // Set the audio quality
        g_object_set (G_OBJECT (private->VorbisEnc), "quality", aQuality, NULL);
}

static GstCaps*                 convert_format_caps (GstCaps *caps)
{
        g_return_val_if_fail (caps != NULL, NULL);

        GstCaps *newcaps = gst_caps_new_simple ("audio/x-raw-float",
                                                "endianness", G_TYPE_INT, 1234,
                                                "width", G_TYPE_INT, 32,
                                                "channels", G_TYPE_INT, 2,
                                                NULL);

        // Get new structure
        GstStructure *newstructure = NULL;
        newstructure = gst_caps_get_structure (newcaps, 0);
        g_return_val_if_fail (newstructure != NULL, NULL);

        // Get old structure
        GstStructure *oldstructure = NULL;
        oldstructure = gst_caps_get_structure (caps, 0);
        g_return_val_if_fail (oldstructure != NULL, NULL);

        // Get the rate from old
        gint32 rate = -1;
        gst_structure_get_int (oldstructure, "rate", &rate);
        g_return_val_if_fail (rate != -1, NULL);

        // Set the rate
        gst_structure_set (newstructure, "rate", G_TYPE_INT, rate, NULL);

        return newcaps;
}
