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

struct                          _DivaPluginsTheoraExporterPrivate
{
        GstElement *VideoRate;
        GstElement *ColorSpace;
        GstElement *TheoraEnc;

        GstElement *AudioConvert;
        GstElement *AudioResample;
        GstElement *VorbisEnc;
        
        GstElement *OggMux;
        GstElement *FileSink;

        GstElement *VCapsFilter;
        GstElement *ACapsFilter;
        GstElement *VideoScale;
};

static void                     diva_plugins_theora_exporter_class_init (DivaPluginsTheoraExporterClass *klass);

static void                     diva_plugins_theora_exporter_init (DivaPluginsTheoraExporter *this);

static void                     diva_plugins_theora_exporter_dispose (DivaPluginsTheoraExporter *this);

static void                     diva_plugins_theora_exporter_finalize (DivaPluginsTheoraExporter *this);

static gboolean                 private_init (DivaPluginsTheoraExporter *this,
                                              DivaPluginsTheoraExporterPrivate *private);

static void                     private_dispose (DivaPluginsTheoraExporter *this,
                                                 DivaPluginsTheoraExporterPrivate *private);

static void                     private_set (DivaPluginsTheoraExporterPrivate *private, gchar *filename,
                                             GdvVideoFormat *vformat, GdvAudioFormat *aformat,
                                             gint32 vQuality, gfloat aQuality);

static GstCaps*                 convert_format_caps (GstCaps *caps);

