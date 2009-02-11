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

#ifndef __DIVA_PLUGINS_WAV_EXPORTER_H__
#define __DIVA_PLUIGNS_WAV_EXPORTER_H__

#include <gst/gst.h>
#include <gdv.h>

typedef struct                  _DivaPluginsWavExporterPrivate DivaPluginsWavExporterPrivate;

typedef struct                  _DivaPluginsWavExporter DivaPluginsWavExporter;

typedef struct                  _DivaPluginsWavExporterClass DivaPluginsWavExporterClass;

#define                         DIVA_PLUGINS_TYPE_WAV_EXPORTER (diva_plugins_wav_exporter_get_type ())

#define                         DIVA_PLUGINS_IS_WAV_EXPORTER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
                                                                                               DIVA_PLUGINS_TYPE_WAV_EXPORTER))

struct                          _DivaPluginsWavExporter
{
        GdvExporter Parent;
        DivaPluginsWavExporterPrivate *Private;
};

struct                          _DivaPluginsWavExporterClass
{
        GstBinClass ParentClass;
};

GType                           diva_plugins_wav_exporter_get_type (void);

DivaPluginsWavExporter*         diva_plugins_wav_exporter_new (gchar *filename);

void                            diva_plugins_wav_exporter_set_format (DivaPluginsWavExporter *this, GdvAudioFormat *format);

#endif
