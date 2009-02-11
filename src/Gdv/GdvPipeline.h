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

#ifndef __GDVPIPELINE_H__
#define __GDVPIPELINE_H__

#include <gst/gst.h>
#include <gst/interfaces/xoverlay.h>
#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include "GdvVideoCompSink.h"
#include "GdvAudioCompSink.h"
#include "GdvTime.h"
#include "GdvTrack.h"
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvProjectFormat.h"
#include "GdvSource.h"
#include "GdvSingleViewBin.h"
#include "GdvTwinViewBin.h"
#include "GdvExporter.h"

typedef struct                  _GdvPipelinePrivate GdvPipelinePrivate;

struct                          _GdvPipeline 
{
        GstPipeline Parent;
        GdvPipelinePrivate *Private;
};

struct                          _GdvPipelineClass 
{
        GstPipelineClass ParentClass;
};

enum                            _GdvPipelineState
{
        GDV_PIPELINE_STATE_UNKNOWN,
        GDV_PIPELINE_STATE_PAUSED,
        GDV_PIPELINE_STATE_PLAYING
} typedef GdvPipelineState;

enum                            _GdvPipelineMode
{
        GDV_PIPELINE_MODE_NORMAL,
        GDV_PIPELINE_MODE_SINGLE,
        GDV_PIPELINE_MODE_TWIN,
        GDV_PIPELINE_MODE_EXPORT,
} typedef GdvPipelineMode;

GType                           gdv_pipeline_get_type (void);

GdvPipeline*                    gdv_pipeline_new (GdvProjectFormat *format);

void                            gdv_pipeline_set_target_window (GdvPipeline *this, GdkWindow *window);

void                            gdv_pipeline_launch (GdvPipeline *this);

void                            gdv_pipeline_shutdown (GdvPipeline *this);

void                            gdv_pipeline_play (GdvPipeline *this);

void                            gdv_pipeline_pause (GdvPipeline *this);

void                            gdv_pipeline_playpause (GdvPipeline *this);

void                            gdv_pipeline_force_expose (GdvPipeline *this);

void                            gdv_pipeline_set_error_idle_func (GdvPipeline *this, gpointer func);

void                            gdv_pipeline_set_state_idle_func (GdvPipeline *this, gpointer func);

void                            gdv_pipeline_set_ticker_idle_func (GdvPipeline *this, gpointer func);

void                            gdv_pipeline_remove_error_idle_func (GdvPipeline *this);

void                            gdv_pipeline_remove_state_idle_func (GdvPipeline *this);

void                            gdv_pipeline_remove_ticker_idle_func (GdvPipeline *this);

gboolean                        gdv_pipeline_add_track (GdvPipeline *this, GdvTrack *track);

void                            gdv_pipeline_remove_track (GdvPipeline *this, GdvTrack *track);

void                            gdv_pipeline_seek (GdvPipeline *this, GdvTime seektime);

void                            gdv_pipeline_ready (GdvPipeline *this);

gboolean                        gdv_pipeline_start_single_view (GdvPipeline *this, GdvSource *source,
                                                                GdvTime time);

gboolean                        gdv_pipeline_seek_single_view (GdvPipeline *this, GdvTime time);

gboolean                        gdv_pipeline_stop_single_view (GdvPipeline *this);

gboolean                        gdv_pipeline_start_twin_view (GdvPipeline *this,
                                                              GdvSource *source1, GdvSource *source2,
                                                              GdvTime time1, GdvTime time2);

gboolean                        gdv_pipeline_seek_twin_view (GdvPipeline *this, GdvTime time1, GdvTime time2);

gboolean                        gdv_pipeline_stop_twin_view (GdvPipeline *this);

gboolean                        gdv_pipeline_start_exporting (GdvPipeline *this, GdvExporter *exporter);

gboolean                        gdv_pipeline_stop_exporting (GdvPipeline *this);

gboolean                        gdv_pipeline_save_last_buffer (GdvPipeline *this, gchar *filename);

#endif
