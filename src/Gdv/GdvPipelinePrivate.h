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

struct                          _GdvPipelinePrivate 
{

        GstElement *XVImageSink;  // XOverlay element
        GstElement *AlsaSink;     // Alsa element
        GstElement *VQueue;       // Video queue
        GstElement *AQueue;       // Audio queue
        GdvVideoCompSink *VComp;  // Video compositor
        GdvAudioCompSink *AComp;  // Audio compositor
        GstElement *VTicker;      // Identity element we're using to keep track
                                  // of the video buffers reaching the video sink

        GdkWindow *TargetWindow;  // Target gdk window
        guint Watch;              // Bus watch

        gboolean Launched;        // If the pipeline was launched
        gboolean Shutdowned;      // If the pipeline was shutdown
        
        GError *LastError;        // Last pipeline error
        
        GSourceFunc ErrorFunc;    // Function to execute on each error (idle)
        GSourceFunc StateFunc;    // Function to state change (play/paused) (idle)
        GSourceFunc TickerFunc;   // Function to notify about playback time (idle)
        
        GdvTime LastBufferTime;   // Timestamp of the last buffer we sent to overlay
        GstBuffer *LastBuffer;    // A last buffer for a screenshot
        GdvTime SavedPos;         
        
        GSList *TrackList;        // A list of all the tracks

        GdvProjectFormat *Format; // Project format we're using

        GstElement *CoreBin;      // A container for all the elements except the actual
                                  // output sinks

        GdvSingleViewBin *SingleViewBin;
        GdvTwinViewBin *TwinViewBin;
        GdvExporter *Exporter;

        GdvPipelineMode Mode;     // Current mode
};

enum 
{
        ARG_0,
        ARG_LAUNCHED,
        ARG_SHUTDOWN,
        ARG_ERROR,
        ARG_TRACKS,
        ARG_TICKER,
        ARG_STATE,
        ARG_FORMAT,
        ARG_VOLUME
};

static void                     gdv_pipeline_class_init (GdvPipelineClass *klass);

static void                     gdv_pipeline_init (GdvPipeline *this);

static void                     gdv_pipeline_dispose (GdvPipeline *this);

static void                     gdv_pipeline_finalize (GdvPipeline *this);

// static gboolean                 can_handle_project_format (GdvProjectFormat *format,
//                                                           GError **error);

static void                     gdv_pipeline_get_property (GdvPipeline *this, guint propid,
                                                           GValue *value, GParamSpec *pspec);

static void                     gdv_pipeline_set_property (GdvPipeline *this, guint propid,
                                                           GValue *value, GParamSpec *pspec);

static gboolean                 bus_callback (GstBus *bus, GstMessage *message,
                                              GdvPipeline *this);

static gboolean                 private_init (GdvPipeline *this, GdvPipelinePrivate *private);

static void                     private_dispose (GdvPipeline *this, GdvPipelinePrivate *private);

static void                     private_dispose_helper (GdvPipeline *this, GstElement **element);

static void                     ticker_handoff (GstElement *ticker, GstBuffer *buffer,
                                                GdvPipeline *this);

static void                     on_reseek_needed (gpointer a, GdvPipeline *this);

static void                     reseek_all_tracks (GdvPipeline *this, gint64 time);

static gboolean                 link_elements (GdvPipeline *this);

static int                      track_compare_func (const GdvTrack *a, const GdvTrack *b);
