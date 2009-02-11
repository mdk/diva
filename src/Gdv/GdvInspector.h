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

#ifndef __GDVINSPECTOR_H__
#define __GDVINSPECTOR_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvErrors.h"
#include "GdvVideoFormat.h"
#include "GdvAudioFormat.h"

struct _GdvInspector {
        GstPipeline Parent;
        
        GstElement *_Filesrc;          /* The filesrc Gst Element */
        GstElement *_Decodebin;        /* Our hero of the day */
        
        gchar *_Mime;                  /* The mime type of the file */
        gchar *_Url;                   /* Url being used */
        
        gboolean _HasAudio;            /* If audio was detected */
        gboolean _HasVideo;            /* If video was detected */
        
        GdvVideoFormat *_VideoFormat;  /* Video format specification */
        GdvAudioFormat *_AudioFormat;  /* Audio format specification */
        
        GdvTime _Length;               /* Length of the source */

        GstTagList *_Tags;             /* Tags present in the source */

        GError *_Error;                /* Error during preroll or NULL for success */
        gboolean _Executed;            /* If the inspector has been started/excuted */
        gboolean _Finished;            /* If the stop was already called */
        guint _Watch;                  /* Watch we're using */

        
        GSourceFunc _ErrorFunc;        /* Idle to call on error */
        GSourceFunc _DoneFunc;         /* Idle to call on done/success */
};

struct _GdvInspectorClass {
        GstPipelineClass ParentClass;
};

GType                           gdv_inspector_get_type (void);

GdvInspector*                   gdv_inspector_new (const gchar *filename);

void                            gdv_inspector_start (GdvInspector *this);

void                            gdv_inspector_set_done_func (GdvInspector *this, gpointer func);

void                            gdv_inspector_set_error_func (GdvInspector *this, gpointer func);

void                            gdv_inspector_abort (GdvInspector *this);

gboolean                        gdv_inspector_get_int_tag (GdvInspector *this, gchar *tag, gint64 *value);

gboolean                        gdv_inspector_get_uint_tag (GdvInspector *this, gchar *tag, guint64 *value);

gboolean                        gdv_inspector_get_string_tag (GdvInspector *this, gchar *tag, gchar **value);


#endif
