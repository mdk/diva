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

/* GdvPipeline is the main, top-level object for playback. It's initialized using
 * a project format, and can be queried before, if it handles a given format.
 * It's based on GstPipeline directly */

#include "GdvPipeline.h"
#include "GdvPipelinePrivate.h"

static GstPipelineClass*        ParentClass = NULL;

/* Private constructor */
static gboolean                 private_init (GdvPipeline *this, GdvPipelinePrivate *priv)
{
        g_return_val_if_fail (priv != NULL && this != NULL, FALSE);

        // Core bin
        priv->CoreBin = gst_bin_new ("CoreBin");
        g_return_val_if_fail (priv->CoreBin != NULL, FALSE);
        
        // Output
        priv->XVImageSink = gst_element_factory_make ("xvimagesink", NULL);
        priv->AlsaSink = gst_element_factory_make ("alsasink", NULL);
        g_return_val_if_fail (priv->XVImageSink != NULL && priv->AlsaSink != NULL, FALSE);
        
        // Queues
        priv->AQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (priv->AQueue != NULL, FALSE);
        g_object_set (G_OBJECT (priv->AQueue), "max-size-time", GST_SECOND * 1, NULL);
        
        priv->VQueue = gst_element_factory_make ("queue", NULL);
        g_return_val_if_fail (priv->VQueue != NULL, FALSE);
        g_object_set (G_OBJECT (priv->VQueue), "max-size-time", GST_SECOND * 1, NULL);
        
        // Comp sinks
        priv->VComp = gdv_videocompsink_new ();
        priv->AComp = gdv_audiocompsink_new ();
        g_return_val_if_fail (priv->AComp != NULL && priv->VComp != NULL, FALSE);
        
        // Extra
        priv->VTicker = gst_element_factory_make ("identity", NULL);
        g_return_val_if_fail (priv->VTicker != NULL, FALSE);
        g_signal_connect (G_OBJECT (priv->VTicker), "handoff",
                          G_CALLBACK (ticker_handoff), this);
        
        // Add to the core bin
        gst_bin_add_many (GST_BIN (priv->CoreBin), 
                          priv->VQueue, priv->AQueue, 
                          priv->VComp, priv->AComp,
                          priv->VTicker, 
                          NULL);

        // Add to to the root
        gst_bin_add_many (GST_BIN (this),
                          priv->CoreBin,
                          priv->XVImageSink,
                          priv->AlsaSink,
                          NULL);
                        
        // Bus
        GDV_DEBUG_ELEMENT ("Setting the bus watch", NULL);
        priv->Watch = gst_bus_add_watch (gst_pipeline_get_bus (GST_PIPELINE (this)),
                                         (gpointer) bus_callback, this);
        g_return_val_if_fail (priv->Watch > 0, FALSE);

        // Window
        priv->TargetWindow = NULL;

        // Status
        priv->Launched = FALSE;
        priv->Shutdowned = FALSE;
        
        // Error 
        priv->LastError = NULL;
        priv->ErrorFunc = NULL;
        
        // Status func
        priv->StateFunc = NULL;
        
        // Ticker
        priv->TickerFunc = NULL;
        priv->LastBufferTime = 0;
        priv->LastBuffer = NULL;
        priv->SavedPos = 0;

        // Modes
        priv->Mode = GDV_PIPELINE_MODE_NORMAL;
        priv->SingleViewBin = NULL;
        priv->TwinViewBin = NULL;
        priv->Exporter = NULL;
        
        // Track list
        priv->TrackList = NULL;
        
        return TRUE;
}

/* Destructing helper */
static void                     private_dispose_helper (GdvPipeline *this, GstElement **element)
{
        gst_bin_remove (GST_BIN (this->Private->CoreBin), *element);
        *element = NULL;
}

/* Private destructor */
static void                     private_dispose (GdvPipeline *this, GdvPipelinePrivate *priv)
{
        g_return_if_fail (this != NULL && priv != NULL);
        GDV_DEBUG_ELEMENT ("Disposing private", NULL);
        
        // Tracks
        if (priv->TrackList != NULL) {
                GSList *iterator;
                for (iterator = priv->TrackList; iterator; iterator = g_slist_next (iterator)) {
                        GdvTrack *track = GDV_TRACK (iterator->data);
                        gst_bin_remove (GST_BIN (this->Private->CoreBin), GST_ELEMENT (track));
                }
                g_slist_free (priv->TrackList);
                priv->TrackList = NULL;
        }

        // FIXME: Why?
        // Outputs
        // if (priv->XVImageSink != NULL)
        //         private_dispose_helper (this, &priv->XVImageSink);
        // if (priv->AlsaSink != NULL) 
        //        private_dispose_helper (this, &priv->AlsaSink);
                
        // Queues
        if (priv->VQueue != NULL)
                private_dispose_helper (this, &priv->VQueue);
        if (priv->AQueue != NULL)
                private_dispose_helper (this, &priv->AQueue);
        
        // Extras
        if (priv->VTicker != NULL)
                private_dispose_helper (this, &priv->VTicker);
        
        // Compositors
        if (priv->VComp != NULL)
                private_dispose_helper (this, (GstElement **) &priv->VComp);
        if (priv->AComp != NULL)
                private_dispose_helper (this, (GstElement **) &priv->AComp);
                
        // Unbind the watch
        if (priv->Watch > 0) {
                g_source_remove (priv->Watch);
                priv->Watch = 0;
        }

        // Unref the window
        if (priv->TargetWindow != NULL) {
                g_object_unref (G_OBJECT (priv->TargetWindow));
                priv->TargetWindow = NULL;
        }

        // Format
        if (priv->Format != NULL) {
                g_object_unref (G_OBJECT (priv->Format));
                priv->Format = NULL;
        }
        
        // Errors
        if (priv->LastError != NULL) {
                g_error_free (priv->LastError);
                priv->LastError = NULL;
        }

        // Last (screenshot) buffer
        if (priv->LastBuffer != NULL) {
                gst_buffer_unref (priv->LastBuffer);
                priv->LastBuffer = NULL;
        }
        
        priv->ErrorFunc = NULL;
        priv->StateFunc = NULL;
}

/* Check if this pipeline can handle the given format. Error will contain the reson why
 * the format cannot be handled 
static gboolean                 can_handle_project_format (GdvProjectFormat *format,
                                                           GError **error)
{
        return TRUE;
}
*/

/* Here we handle the bus events during the playback */
static gboolean                 bus_callback (GstBus *bus, GstMessage *message,
                                              GdvPipeline *this)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_assert (this->Private != NULL);
        GDV_DEBUG_ELEMENT ("-> '%s'", gst_message_type_get_name (GST_MESSAGE_TYPE (message)));
        
        // Normal maessage handling
        switch (GST_MESSAGE_TYPE (message)) {
                
                case GST_MESSAGE_ERROR: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_error (message, &err, &debug);
                        GDV_DEBUG_ELEMENT ("Error: %s (%s)", err->message, debug);
                        if (this->Private->LastError != NULL)
                                g_error_free (this->Private->LastError);
                        this->Private->LastError = err;
                        
                        if (this->Private->ErrorFunc != NULL) {
                                GError *copy = g_error_copy (err);
                                g_idle_add (this->Private->ErrorFunc, copy);
                        }
                        
                } break;
                        
                case GST_MESSAGE_STATE_CHANGED: {
		   
                        // Ignore state-change messages from other elements
		        if ((GdvPipeline *) (GST_MESSAGE_SRC (message)) != this)
		                return TRUE;
                        
		        GstState oldstate, newstate, pending;
		        gst_message_parse_state_changed (message, &oldstate, &newstate, &pending);
                   		   
		        // Better break now if no func set
		        if (this->Private->StateFunc == NULL)
		                break;

		        GdvPipelineState *state = g_malloc (sizeof (GdvPipelineState));
                        
                        if (newstate == GST_STATE_PAUSED) 
                                *state = GDV_PIPELINE_STATE_PAUSED;
                        else if (newstate == GST_STATE_PLAYING)
                                *state = GDV_PIPELINE_STATE_PLAYING;
                        else
                                *state = GDV_PIPELINE_STATE_UNKNOWN;
                    
                        g_idle_add (this->Private->StateFunc, state);
                        
                } break;
                        
                case GST_MESSAGE_WARNING: {
                        GError *err;
                        gchar *debug;
                        gst_message_parse_warning (message, &err, &debug);
                        // FIXME: Free the debug?!
                        GDV_WARNING_ELEMENT ("%s (%s)", err->message, debug);
                        g_error_free (err);
                } break;
                        
                default:
                        break;
        }
                
        return TRUE;
}

/* GObject Type */
GType                           gdv_pipeline_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvPipelineClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_pipeline_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvPipeline),
                        1,
                        (GInstanceInitFunc) gdv_pipeline_init,
                };
                objecttype = g_type_register_static (GST_TYPE_PIPELINE, "GdvPipeline", &objectinfo, 0);
        }

        return objecttype;
}

/* GObject class init */
static void                     gdv_pipeline_class_init (GdvPipelineClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;

        gobjectclass->dispose = (gpointer) gdv_pipeline_dispose;
        gobjectclass->finalize = (gpointer) gdv_pipeline_finalize;
        ParentClass = g_type_class_ref (GST_TYPE_PIPELINE);

        gobjectclass->get_property = (gpointer) gdv_pipeline_get_property;
        gobjectclass->set_property = (gpointer) gdv_pipeline_set_property;

        g_object_class_install_property (gobjectclass, ARG_LAUNCHED,
                                         g_param_spec_boolean
                                         ("launched", "Launched",
                                          "If the pipeline was launched",
                                          FALSE, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_SHUTDOWN,
                                         g_param_spec_boolean
                                         ("shutdowned", "Shutdowned",
                                          "If the pipeline was shutdown",
                                          FALSE, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_ERROR,
                                         g_param_spec_pointer
                                         ("error", "Error", "The last error on the pipeline",
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_FORMAT,
                                         g_param_spec_object
                                         ("projectformat", "ProjectFormat", "Pipeline format",
                                          GDV_TYPE_PROJECTFORMAT,
                                          G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_TRACKS,
                                         g_param_spec_pointer
                                         ("tracks", "Tracks", "The list of all the tracks",
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_TICKER,
                                         g_param_spec_uint64
                                         ("ticker", "Ticker",
                                          "Current ticker",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_STATE,
                                         g_param_spec_int
                                         ("state", "State",
                                          "State of the pipeline",
                                          0, G_MAXINT, 0, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_VOLUME,
                                         g_param_spec_double
                                         ("volume", "Volume",
                                          "Master volume for the pipeline",
                                          0.0, 1.0, 1.0, G_PARAM_READABLE | G_PARAM_WRITABLE));
}

/* GObject init */
static void                     gdv_pipeline_init (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        this->Private = NULL;
        
        GdvPipelinePrivate *priv = g_new0 (GdvPipelinePrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* GObject dispose */
static void                     gdv_pipeline_dispose (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_pipeline_finalize (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Create a new instance */
GdvPipeline*                    gdv_pipeline_new (GdvProjectFormat *format)
{
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), NULL);
        // FIXME: Check if format is actually supported
        
        GdvPipeline *this = g_object_new (GDV_TYPE_PIPELINE, NULL);
        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        }

        g_object_ref (format);
        this->Private->Format = format;

        // Set the compsink formats
        gdv_videocompsink_set_format (this->Private->VComp, format);
        gdv_audiocompsink_set_format (this->Private->AComp, format);
        
        if (! link_elements (this)) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else 
                return this;
}

/* Set the target window we'll overlay */
void                            gdv_pipeline_set_target_window (GdvPipeline *this, GdkWindow *window)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (window != NULL);

        GDV_DEBUG_ELEMENT ("Setting target window", NULL);
        
        gulong xid = GDK_WINDOW_XWINDOW (window);

        GDV_DEBUG_ELEMENT ("Setting %s xid to %ld", GST_OBJECT_NAME (this->Private->XVImageSink), xid);
        gst_x_overlay_set_xwindow_id (GST_X_OVERLAY (this->Private->XVImageSink), xid);
        
        this->Private->TargetWindow = window;
        g_object_ref (G_OBJECT (window));
}

/* Launch (initialize) the pipeline. We set it here to PAUSED */
void                            gdv_pipeline_launch (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));

        if (this->Private->Launched == TRUE)
                GDV_WARNING_ELEMENT ("Already launched", NULL);
        
        GDV_DEBUG_ELEMENT ("Launching the pipeline", NULL);
        
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        this->Private->Launched = TRUE;
}

/* Shutdown (deinitialized) the pipeline */
void                            gdv_pipeline_shutdown (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));

        if (this->Private->Shutdowned == TRUE)
                GDV_WARNING_ELEMENT ("Already shutdown", NULL);
        
        GDV_DEBUG_ELEMENT ("Shutting down the pipeline", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_NULL);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        this->Private->Shutdowned = TRUE;
}

/* Force the re-exposition of the overlay. That happens when we move the target
 * window, etc. Last frame is re-exposed */
void                            gdv_pipeline_force_expose (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (this->Private->Shutdowned != TRUE && this->Private->Launched == TRUE);

        GDV_DEBUG_ELEMENT ("Forcing overlay expose", NULL);
        gst_x_overlay_expose (GST_X_OVERLAY (this->Private->XVImageSink));
}

/* GObject get property */
static void                     gdv_pipeline_get_property (GdvPipeline *this, guint propid,
                                                           GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_assert (this->Private != NULL);
                
        switch (propid) {
                
        case ARG_LAUNCHED:
                g_value_set_boolean (value, this->Private->Launched);
                break;
                
        case ARG_SHUTDOWN:
                g_value_set_boolean (value, this->Private->Shutdowned);
                break;
                
        case ARG_ERROR:
                g_return_if_fail (this->Private->LastError != NULL);
                g_value_set_pointer (value, this->Private->LastError);
                break;
                
        case ARG_TRACKS:
                g_value_set_pointer (value, this->Private->TrackList);
                break;

        case ARG_TICKER:
                GST_OBJECT_LOCK (this);
                g_value_set_uint64 (value, this->Private->LastBufferTime);
                GST_OBJECT_UNLOCK (this);
                break;

        case ARG_FORMAT:
                g_value_set_object (value, this->Private->Format);
                break;

        case ARG_STATE: {
                GstState state;
                gst_element_get_state (GST_ELEMENT (this), &state, NULL,
                                       GST_CLOCK_TIME_NONE);

                if (state == GST_STATE_PAUSED)
                        g_value_set_int (value, GDV_PIPELINE_STATE_PAUSED);
                else if (state == GST_STATE_PLAYING)
                        g_value_set_int (value, GDV_PIPELINE_STATE_PLAYING);
                else
                        g_value_set_int (value, GDV_PIPELINE_STATE_UNKNOWN);
        } break;
                
        case ARG_VOLUME: {
                gdouble volume = gdv_audiocompsink_get_volume (this->Private->AComp);
                g_value_set_double (value, volume);
        } break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }

}

/* GObject get property */
static void                     gdv_pipeline_set_property (GdvPipeline *this, guint propid,
                                                           GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_assert (this->Private != NULL);

        switch (propid) {

        case ARG_VOLUME: {
                gdouble volume = g_value_get_double (value);
                gdv_audiocompsink_set_volume (this->Private->AComp, volume);
        } break;

        default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }

}

void                            gdv_pipeline_set_error_idle_func (GdvPipeline *this, gpointer func)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (func != NULL);
        g_assert (this->Private != NULL);
        

        GDV_DEBUG_ELEMENT ("Setting error func", NULL);
        if (this->Private->ErrorFunc != NULL)
                GDV_WARNING_ELEMENT ("Already has a error func set", NULL);
        
        this->Private->ErrorFunc = func;
}

void                            gdv_pipeline_set_state_idle_func (GdvPipeline *this, gpointer func)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (func != NULL);
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Setting state func", NULL);
        if (this->Private->StateFunc != NULL)
                GDV_WARNING_ELEMENT ("Already has a state func set", NULL);
        
        this->Private->StateFunc = func;
}

void                            gdv_pipeline_remove_state_idle_func (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Removing state func", NULL);
        if (this->Private->StateFunc == NULL)
                GDV_WARNING_ELEMENT ("Doesn't have a state func set", NULL);

        this->Private->StateFunc = NULL;
}

void                            gdv_pipeline_remove_error_idle_func (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Removing error func", NULL);
        if (this->Private->ErrorFunc == NULL)
                GDV_WARNING_ELEMENT ("Doesn't have an error func set", NULL);
        
        this->Private->ErrorFunc = NULL;
}

void                            gdv_pipeline_set_ticker_idle_func (GdvPipeline *this, gpointer func)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (func != NULL);
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Setting ticker func", NULL);
        if (this->Private->TickerFunc != NULL)
                GDV_WARNING_ELEMENT ("Already has a ticker func set", NULL);
        
        this->Private->TickerFunc = func;
}

void                            gdv_pipeline_remove_ticker_idle_func (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Removing ticker func", NULL);
        if (this->Private->TickerFunc == NULL)
                GDV_WARNING_ELEMENT ("Doesn't have a ticker func set", NULL);

        this->Private->TickerFunc = NULL;
}

/* Add a link a given track to this pipeline */
gboolean                        gdv_pipeline_add_track (GdvPipeline *this, GdvTrack *track)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (GDV_IS_TRACK (track), FALSE);
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Adding & linking a new track '%s'", GST_ELEMENT_NAME (track));
        
        g_return_val_if_fail (gst_bin_add (GST_BIN (this->Private->CoreBin), GST_ELEMENT (track)), FALSE);
        
        gst_object_ref (track);

        g_return_val_if_fail (gdv_videocompsink_link_track (this->Private->VComp, track), FALSE);
        g_return_val_if_fail (gdv_audiocompsink_link_track (this->Private->AComp, track), FALSE);
        
        // Append to the list
        this->Private->TrackList = g_slist_insert_sorted (this->Private->TrackList, track,
                                                          (GCompareFunc) track_compare_func);
        
        // Connect the signal
        g_signal_connect (G_OBJECT (track), "reseek-needed",
                          (GCallback) on_reseek_needed,
                          this);
        
        return TRUE;
}

/* FIXME: Implement */
void                            gdv_pipeline_remove_track (GdvPipeline *this, GdvTrack *track)
{
        return;
}

/* Change the pipeline to playing */
void                            gdv_pipeline_play (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (this->Private->Launched);
        g_return_if_fail (this->Private->Shutdowned == FALSE);

        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PLAYING);
}

/* Change the pipeline to paused */
void                            gdv_pipeline_pause (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (this->Private->Launched);
        g_return_if_fail (this->Private->Shutdowned == FALSE);
        
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);
}

/* Change the pipeline to stopped */
void                            gdv_pipeline_ready (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (this->Private->Launched);
        g_return_if_fail (this->Private->Shutdowned == FALSE);
        
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
}

/* Change the pipeline to paused if playing. Or change the pipeline to 
 * playing if paused */
void                            gdv_pipeline_playpause (GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (this->Private->Launched);
        g_return_if_fail (this->Private->Shutdowned == FALSE);
        
        GstState current, pending;
        gst_element_get_state (GST_ELEMENT (this), &current, &pending, GST_SECOND / 2);
        
        if (current == GST_STATE_PAUSED || pending == GST_STATE_PAUSED) {
                gdv_pipeline_play (this);
                return;
        }
        
        if (current == GST_STATE_PLAYING || pending == GST_STATE_PLAYING) {
                gdv_pipeline_pause (this);
                return;
        }
}

static void                     ticker_handoff (GstElement *ticker, GstBuffer *buffer,
                                                GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        g_return_if_fail (buffer != NULL);
        g_assert (this->Private != NULL);

        GDV_DEBUG_ELEMENT ("Ticker reports '%s'", gdv_time_to_string (GST_BUFFER_TIMESTAMP (buffer)));
        g_debug ("Ticker is %s", gdv_time_to_string (GST_BUFFER_TIMESTAMP (buffer)));

        GST_OBJECT_LOCK (this);
        this->Private->LastBufferTime = GST_BUFFER_TIMESTAMP (buffer);
        
        #if 1
        // FIXME: That is SO shit I can't even believe it's here
        if (this->Private->TickerFunc != NULL) {
                GdvTime *time = g_malloc (sizeof (GdvTime));
                *time = this->Private->LastBufferTime;
                g_idle_add (this->Private->TickerFunc, time);
        }
        #endif

        // Store the last buffer for possible screenshot
        if (this->Private->LastBuffer != NULL)
                gst_buffer_unref (this->Private->LastBuffer);
        this->Private->LastBuffer = buffer;
        gst_buffer_ref (this->Private->LastBuffer);

        GST_OBJECT_UNLOCK (this);
}

/* This signal is emitted by the Tracks when they need a reseek. 
 * One track reseek = all tracks reseek. This usually happens when a clip 
 * is added to a track */
static void                     on_reseek_needed (gpointer a, GdvPipeline *this)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
                
        reseek_all_tracks (this, this->Private->LastBufferTime);
}

void                            gdv_pipeline_seek (GdvPipeline *this, GdvTime seektime)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));
        GDV_DEBUG_ELEMENT ("Seek request to %s", gdv_time_to_string (seektime));

        reseek_all_tracks (this, seektime);
        g_debug ("All reseek done!");
}

static void                     reseek_all_tracks (GdvPipeline *this, gint64 time)
{
        g_return_if_fail (GDV_IS_PIPELINE (this));

        g_debug ("Reseek all at %s", gdv_time_to_string (time));
        GDV_DEBUG_ELEMENT ("Reseking all tracks at %s", gdv_time_to_string (time));
        
        gst_element_seek (GST_ELEMENT (this), 1.0, GST_FORMAT_TIME,  GST_SEEK_FLAG_FLUSH,
                          GST_SEEK_TYPE_SET, time, GST_SEEK_TYPE_SET, -1);
        g_debug ("Reseek done!");
}

static gboolean                 link_elements (GdvPipeline *this)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);

        GDV_DEBUG_ELEMENT ("Linking pipeline elements", NULL);
        GdvPipelinePrivate *priv = this->Private;

        g_return_val_if_fail (gst_element_link (priv->VQueue, 
                                                priv->VTicker), FALSE);
        g_return_val_if_fail (gst_element_link (priv->VTicker, 
                                                priv->XVImageSink), FALSE);
        g_return_val_if_fail (gst_element_link (priv->AQueue, 
                                                priv->AlsaSink), FALSE);
        
        g_return_val_if_fail (gst_element_link (GST_ELEMENT (priv->VComp), 
                                                priv->VQueue), FALSE);
        g_return_val_if_fail (gst_element_link (GST_ELEMENT (priv->AComp),
                                                priv->AQueue), FALSE);

        return TRUE;
}

/* Change the pipeline mode to single-view and connect the given source */
gboolean                        gdv_pipeline_start_single_view (GdvPipeline *this, GdvSource *source, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (GDV_IS_SOURCE (source), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_NORMAL, FALSE);        

        GDV_DEBUG_ELEMENT ("Setting pipeline to single view mode with '%s'", GST_OBJECT_NAME (source));

        GST_OBJECT_LOCK (this);
        this->Private->SavedPos = this->Private->LastBufferTime;
        GST_OBJECT_UNLOCK (this);

        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        gst_element_unlink (this->Private->VTicker, this->Private->XVImageSink);
        gst_element_unlink (this->Private->AQueue, this->Private->AlsaSink);
        
        // Remove the core bin
        gst_object_ref (this->Private->CoreBin);
        gst_bin_remove (GST_BIN (this), this->Private->CoreBin);

        // Remove the alsa
        gst_object_ref (this->Private->AlsaSink);
        gst_bin_remove (GST_BIN (this), this->Private->AlsaSink);

        // Create the new singleviewbin
        this->Private->SingleViewBin = gdv_singleviewbin_new (this->Private->Format,
                                                              source);
        g_return_val_if_fail (this->Private->SingleViewBin != NULL, FALSE);

        // Add it
        gst_bin_add (GST_BIN (this), GST_ELEMENT (this->Private->SingleViewBin));

        gst_element_link_pads (GST_ELEMENT (this->Private->SingleViewBin),
                               "src", this->Private->XVImageSink, "sink");

        // Initially seek the source
        gdv_singleviewbin_seek (this->Private->SingleViewBin, time);
        this->Private->Mode = GDV_PIPELINE_MODE_SINGLE;

        GDV_DEBUG_ELEMENT ("Setting the state", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);

        return TRUE;
}

gboolean                        gdv_pipeline_start_twin_view (GdvPipeline *this,
                                                              GdvSource *source1, GdvSource *source2,
                                                              GdvTime time1, GdvTime time2)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (GDV_IS_SOURCE (source1), FALSE);
        g_return_val_if_fail (GDV_IS_SOURCE (source2), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_NORMAL, FALSE);        

        GDV_DEBUG_ELEMENT ("Setting pipeline to twin view mode with '%s' and '%s'",
                           GST_OBJECT_NAME (source1), GST_OBJECT_NAME (source2));

        GST_OBJECT_LOCK (this);
        this->Private->SavedPos = this->Private->LastBufferTime;
        GST_OBJECT_UNLOCK (this);

        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        gst_element_unlink (this->Private->VTicker, this->Private->XVImageSink);
        gst_element_unlink (this->Private->AQueue, this->Private->AlsaSink);

        // Remove the core bin
        gst_object_ref (this->Private->CoreBin);
        gst_bin_remove (GST_BIN (this), this->Private->CoreBin);

        // Remove the alsa
        gst_object_ref (this->Private->AlsaSink);
        gst_bin_remove (GST_BIN (this), this->Private->AlsaSink);

        // Create the new singleviewbin
        this->Private->TwinViewBin = gdv_twinviewbin_new (this->Private->Format,
                                                          source1, source2);
        
        g_return_val_if_fail (this->Private->TwinViewBin != NULL, FALSE);

        // Add it
        gst_bin_add (GST_BIN (this), GST_ELEMENT (this->Private->TwinViewBin));
        
        gst_element_link_pads (GST_ELEMENT (this->Private->TwinViewBin),
                "src", this->Private->XVImageSink, "sink");

        // Initially seek the source
        gdv_twinviewbin_seek (this->Private->TwinViewBin, time1, time2, TRUE);
        this->Private->Mode = GDV_PIPELINE_MODE_TWIN;

        GDV_DEBUG_ELEMENT ("Setting the state", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);

        return TRUE;
}

gboolean                        gdv_pipeline_seek_twin_view (GdvPipeline *this, GdvTime time1, GdvTime time2)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_TWIN, FALSE);        

        GDV_DEBUG_ELEMENT ("Twin seeking to %s:%s",
                           gdv_time_to_string (time1), gdv_time_to_string (time2));
        
        return gdv_twinviewbin_seek (this->Private->TwinViewBin, time1, time2, FALSE);
}

gboolean                        gdv_pipeline_seek_single_view (GdvPipeline *this, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_SINGLE, FALSE);        

        GDV_DEBUG_ELEMENT ("Signel seeking to %s", gdv_time_to_string (time));
        
        return gdv_singleviewbin_seek (this->Private->SingleViewBin, time);
}

gboolean                        gdv_pipeline_stop_single_view (GdvPipeline *this)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_SINGLE, FALSE);        
        
        GDV_DEBUG_ELEMENT ("Stopping pipeline single view mode", NULL);

        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        gst_element_unlink (GST_ELEMENT (this->Private->SingleViewBin), this->Private->XVImageSink);

        // Remove source
        gst_bin_remove (GST_BIN (this), GST_ELEMENT (this->Private->SingleViewBin));

        // Add the core bin
        gst_bin_add (GST_BIN (this), this->Private->CoreBin);
        
        // Add the alsa
        gst_bin_add (GST_BIN (this), this->Private->AlsaSink);

        // Link us back
        gst_element_link (this->Private->VTicker, this->Private->XVImageSink);
        gst_element_link (this->Private->AQueue, this->Private->AlsaSink);

        this->Private->Mode = GDV_PIPELINE_MODE_NORMAL;

        reseek_all_tracks (this, this->Private->SavedPos);
        this->Private->SavedPos = 0;
        
        GDV_DEBUG_ELEMENT ("Setting the state", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);
        
        return TRUE;
}

gboolean                        gdv_pipeline_stop_twin_view (GdvPipeline *this)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_TWIN, FALSE);        
        
        GDV_DEBUG_ELEMENT ("Stopping pipeline twin view mode", NULL);

        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        gst_element_unlink (GST_ELEMENT (this->Private->TwinViewBin), this->Private->XVImageSink);

        // Remove source
        gst_bin_remove (GST_BIN (this), GST_ELEMENT (this->Private->TwinViewBin));

        // Add the core bin
        gst_bin_add (GST_BIN (this), this->Private->CoreBin);
        
        // Add the alsa
        gst_bin_add (GST_BIN (this), this->Private->AlsaSink);

        // Link us back
        gst_element_link (this->Private->VTicker, this->Private->XVImageSink);
        gst_element_link (this->Private->AQueue, this->Private->AlsaSink);

        this->Private->Mode = GDV_PIPELINE_MODE_NORMAL;

        reseek_all_tracks (this, this->Private->SavedPos);
        this->Private->SavedPos = 0;
        
        GDV_DEBUG_ELEMENT ("Setting the state", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);

        return TRUE;
}

static int                      track_compare_func (const GdvTrack *a, const GdvTrack *b)
{
        g_return_val_if_fail (GDV_IS_TRACK (a), 0);
        g_return_val_if_fail (GDV_IS_TRACK (b), 0);

        g_object_get (G_OBJECT (a), "layer", &a, NULL);
        g_object_get (G_OBJECT (b), "layer", &b, NULL);

        if (a < b)
                return 1;
        else if (a > b)
                return -1;
        else
                return 0;
}

gboolean                        gdv_pipeline_start_exporting (GdvPipeline *this, GdvExporter *exporter)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (GDV_IS_EXPORTER (exporter), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_NORMAL, FALSE);        

        GDV_DEBUG_ELEMENT ("Adding exporter '%s'", GST_OBJECT_NAME (exporter));

        // Make us ready
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        // Disconnect the sinks
        gst_element_unlink (this->Private->VTicker, this->Private->XVImageSink);
        gst_element_unlink (this->Private->AQueue, this->Private->AlsaSink);

        // Remove the sinks from the bin
        gst_object_ref (this->Private->XVImageSink);
        gst_bin_remove (GST_BIN (this), this->Private->XVImageSink);
        gst_object_ref (this->Private->AlsaSink);
        gst_bin_remove (GST_BIN (this), this->Private->AlsaSink);

        // Add the exporter
        gst_object_ref (exporter);
        gst_bin_add (GST_BIN (this), GST_ELEMENT (exporter));
        this->Private->Exporter = exporter;

        // Link it
        g_return_val_if_fail (gst_element_link_pads ((this->Private->VTicker), "src", GST_ELEMENT (exporter), "vsink"), FALSE);
        g_return_val_if_fail (gst_element_link_pads ((this->Private->AQueue), "src", GST_ELEMENT (exporter), "asink"), FALSE);

        // Set state
        GDV_DEBUG_ELEMENT ("Setting the state of rebuilt pipeline to PAUSED", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);

        this->Private->Mode = GDV_PIPELINE_MODE_EXPORT;

        return TRUE;
}

gboolean                        gdv_pipeline_stop_exporting (GdvPipeline *this)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (this->Private->Mode == GDV_PIPELINE_MODE_EXPORT, FALSE);        
        
        GDV_DEBUG_ELEMENT ("Stopping exporting mode", NULL);

        gst_element_set_state (GST_ELEMENT (this), GST_STATE_READY);
        gst_element_get_state (GST_ELEMENT (this), NULL, NULL, GST_CLOCK_TIME_NONE);

        gst_element_unlink (this->Private->VTicker, GST_ELEMENT (this->Private->Exporter));
        gst_element_unlink (this->Private->AQueue, GST_ELEMENT (this->Private->Exporter));

        // Remove source
        gst_bin_remove (GST_BIN (this), GST_ELEMENT (this->Private->Exporter));

        // Add the xvimagesink
        gst_bin_add (GST_BIN (this), this->Private->XVImageSink);
        
        // Add the alsa
        gst_bin_add (GST_BIN (this), this->Private->AlsaSink);

        // Link us back
        gst_element_link (this->Private->VTicker, this->Private->XVImageSink);
        gst_element_link (this->Private->AQueue, this->Private->AlsaSink);

        this->Private->Mode = GDV_PIPELINE_MODE_NORMAL;
        
        reseek_all_tracks (this, this->Private->SavedPos);
        this->Private->SavedPos = 0;
        
        GDV_DEBUG_ELEMENT ("Setting the state", NULL);
        gst_element_set_state (GST_ELEMENT (this), GST_STATE_PAUSED);
        
        //GDV_WARNING_ELEMENT ("SAVED: %ld", this->Private->SavedPos);
        
        return TRUE;
}

/* Saves the last buffer outputted to a png filename using Gdk/Gtk pixbuf funcs */
gboolean                        gdv_pipeline_save_last_buffer (GdvPipeline *this, gchar *filename)
{
        g_return_val_if_fail (GDV_IS_PIPELINE (this), FALSE);
        g_return_val_if_fail (filename != NULL, FALSE);

        GDV_DEBUG_ELEMENT ("Exporting last buffer to '%s'", filename);

        GdkPixbuf *pixbuf = NULL;
        gpointer pixbufdata = NULL;
        gboolean result = TRUE;
        gint width;
        gint height;

        // FIXME: We need a different kind of locking here
        //GST_OBJECT_LOCK (this);

        // Analyze the data. We assume it's YUY2
        GstCaps *buffercaps = GST_BUFFER_CAPS (this->Private->LastBuffer);
        if (buffercaps == NULL)
                goto error;

        GstStructure *bufferstruct = gst_caps_get_structure (buffercaps, 0);
        if (bufferstruct == NULL)
                goto error;

        gst_structure_get_int (bufferstruct, "width", &width);
        gst_structure_get_int (bufferstruct, "height", &height);

        // Allocate memory
        GDV_DEBUG_ELEMENT ("Mallocating %d bytes for RGB data [%d x %d]", width * height * 3,
                           width, height);
        pixbufdata = g_malloc (width * height * 3);
        if (pixbufdata == NULL)
                goto error;

        // Fill the mem with YUY2 -> RGB converted values
        GDV_DEBUG_ELEMENT ("Converting YUY2 to RGB", NULL);
        gdv_yuy2_to_rgb (GST_BUFFER_DATA (this->Private->LastBuffer), width, height,
                         width * 3, pixbufdata, width, height, width * 3);
        
        // Create the pixbuf
        pixbuf = gdk_pixbuf_new_from_data (pixbufdata, GDK_COLORSPACE_RGB, FALSE,
                                           8, width, height, 
                                           width * 3, (GdkPixbufDestroyNotify) g_free, NULL);
        if (pixbuf == NULL)
                goto error;

        // Save the pixbuf to a file
        if (! gdk_pixbuf_save (pixbuf, filename, "png", NULL, NULL))
                goto error;

        // Done, all ok
        goto done;

        //// DEAD ////
        
 error:
        GDV_WARNING_ELEMENT ("Failed to save screenshot", NULL);
        result = FALSE;
        
 done:
        if (pixbuf != NULL)
                g_object_unref (G_OBJECT (pixbuf));
        else
                if (pixbufdata != NULL)
                        g_free (pixbufdata);
        
        // FIXME: We need a different kind of locking here
        //GST_OBJECT_UNLOCK (this);
        return result;
}
