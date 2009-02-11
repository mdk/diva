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

// FIXME: DISPOSE!

#include "GdvFrameRunner.h"

/* GObject type */
GType                   gdv_framerunner_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvFrameRunnerClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_framerunner_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvFrameRunner), 
                        16, 
                        (GInstanceInitFunc) gdv_framerunner_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvFrameRunner",
                                                     &objectinfo, 0);
        }
        return objecttype;
} 

/* GObject class init */
void                    gdv_framerunner_class_init (GdvFrameRunnerClass *klass)
{
        g_type_class_ref (GST_TYPE_BIN);
}

/* GObject init */
void                    gdv_framerunner_init (GdvFrameRunner *this)
{
        this->_Source = NULL;
        this->_PixbufSink = NULL;
} 

/* Create a new object */
GdvFrameRunner          *gdv_framerunner_new (GdvSource *source)
{
        GdvFrameRunner *this = NULL;
        GdvFrameDimensions *dimensions = NULL;
        
        g_assert (GDV_IS_SOURCE (source) == TRUE);
        
        // First check if we have video actually
        gboolean hasvideo;
        g_object_get (G_OBJECT (source), "hasvideo", &hasvideo, NULL);
        if (hasvideo == FALSE)
                goto finish;

        GdvVideoFormat *vformat = NULL;
        g_object_get (G_OBJECT (source), "videoformat", &vformat, NULL);
        if (vformat == NULL)
                goto finish;

        // Check if object creation ok
        this = g_object_new (GDV_TYPE_FRAMERUNNER, NULL);
        if (this == NULL)
                goto finish;
       
        GDV_DEBUG_ELEMENT ("Creating a new GdvFrameRunner object", NULL);
        
        // Create the pixbuf sink
        this->_PixbufSink = gdv_pixbufsink_new ();
        if (this->_PixbufSink == NULL)
                goto finish;
        
        // Check if we can link
        if (gst_pad_can_link (gst_element_get_pad ((GstElement *) source, "vsrc"),
                              gst_element_get_pad ((GstElement *) this->_PixbufSink, "sink")) == FALSE) {
                GDV_WARNING_EXT ("Can't link to PixbufSink, unknown video format?", NULL);
                goto finish;
        }
        
        GstCaps *caps = gst_pad_get_caps (gst_element_get_pad ((GstElement *) this->_PixbufSink, 
                                                               "sink"));
        if (gdv_videoformat_is_compatible_caps (vformat, caps)) {
                gst_caps_unref (caps);
                GDV_WARNING_EXT ("Can't link to PixbufSink, unknown video format?", NULL);
                goto finish;
        }
        gst_caps_unref (caps);
                
        
        // Set the proper dimensions
        GDV_DEBUG_ELEMENT ("Setting the proper PixbufSink dimensions", NULL);
        dimensions = gdv_framedimensions_new (0,0);
        gdv_source_get_thumb_dimensions (source, dimensions);
        g_object_set (G_OBJECT (this->_PixbufSink), "framedimensions", dimensions, NULL);
        
        this->_Source = source; 
       
        GDV_DEBUG_ELEMENT ("Adding all sub-elements and linking", NULL);
        // FIXME Should check for return values
        gst_bin_add ((GstBin *) this, (GstElement *) this->_Source);
        gst_bin_add ((GstBin *) this, (GstElement *) this->_PixbufSink);
        
        // Link
        if (gdv_source_link_to_sinks (this->_Source, (GstElement *) this->_PixbufSink, NULL) == FALSE) {
                GDV_DEBUG_ELEMENT ("Linking to PixbufSink failed!", NULL);
                goto finish;
        }
        
        // FIXME: Do we really need that?
        GDV_DEBUG_ELEMENT ("Setting the state to PAUSED", NULL);
        gst_element_set_state ((GstElement *) this, GST_STATE_PAUSED);
        // FIXME: Temporary!
        //g_usleep (1000000);
        gst_element_get_state ((GstElement *) this->_PixbufSink, NULL, NULL, GST_CLOCK_TIME_NONE);


                               
        return this;
        g_assert_not_reached ();
        
        // ** ERROR **
        finish:
        if (this != NULL)
                g_object_unref (G_OBJECT (this));
        return NULL;        
}

/* Job processing function. This is called by the JobProcessor, and actual frame rendering
 * happens here. */
gboolean                gdv_framerunner_job_work_func (GdvFrameRunnerJobData *data)
{
        g_assert (data != NULL);
        g_assert (GDV_IS_FRAMERUNNER (data->Runner));
        
        GdvFrameRunner *this = data->Runner;
        GDV_DEBUG_ELEMENT ("Entering runner work function", NULL);
        
        // Check the dimensions...
        GdvFrameDimensions *currentdimensions = NULL;
        g_object_get (G_OBJECT (this->_PixbufSink), "framedimensions", &currentdimensions, NULL);
        g_assert (currentdimensions != NULL);
        
        if (gdv_framedimensions_compare (currentdimensions, data->Dimensions) != 0) {
                GDV_DEBUG_ELEMENT ("Dimensions differ, setting", NULL);
                g_object_set (G_OBJECT (this->_PixbufSink), "framedimensions", data->Dimensions, NULL);
        }
        
        GDV_DEBUG_ELEMENT ("Locking the pixbufsink", NULL);
        gdv_pixbufsink_lock (this->_PixbufSink);
        GDV_DEBUG_ELEMENT ("Locked, seeking", NULL);
        GdvTimeSpan *span = gdv_timespan_new (data->AtTime, data->AtTime);
        gst_element_seek (GST_ELEMENT (this->_Source), 1.0, GST_FORMAT_TIME,
                          GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_ACCURATE,
                          GST_SEEK_TYPE_SET, span->Start,
                          GST_SEEK_TYPE_SET, -1);
        gdv_timespan_free (span);
        // FIXME: Wait goes here!
        // gdv_pixbufsink_wait (this->_PixbufSink);
        GDV_DEBUG_ELEMENT ("Done, getting the pixbuf", NULL);
        g_object_get (G_OBJECT (this->_PixbufSink), "pixbuf", &data->Pixbuf, NULL);
        g_assert (data->Pixbuf != NULL);
        GDV_DEBUG_ELEMENT ("Done, job is ready. Freeing things", NULL);
        
        // Free
        gdv_framedimensions_free (currentdimensions);
        
        return TRUE;
}

/* Schedules a new job to decompress a frame from this frame runner. Job is placed in the 
 * queue. This is a handy function for the FrameableInterface */
GdvToken                gdv_framerunner_job_schedule (GdvFrameRunner *this, GdvTime attime, 
                                                      GdvFrameDimensions *dim, GdvToken *settoken, 
                                                      GSourceFunc callback)
{
        g_assert (GDV_IS_FRAMERUNNER (this));
        g_assert (dim != NULL);
        g_assert (callback != NULL);
        
        GDV_DEBUG_ELEMENT ("Creating new job", NULL);
        GdvJob *job = gdv_job_new (*settoken, GDV_JOB_REQUEST);
        job->Custom = gdv_framerunnerjobdata_new (attime, dim, this);
        job->WorkFunc = (GdvJobWorkFunc) gdv_framerunner_job_work_func;
        job->IdleFunc = callback;
        job->FreeFunc = (GdvJobFreeFunc) gdv_framerunnerjobdata_free;
        
        *settoken = job->SetToken;
        GdvToken token = job->Token;

        GDV_DEBUG_ELEMENT ("Getting a queue", NULL);
        GdvJobQueue *queue = gdv_jobprocessor_get_queue ();
        g_assert (queue != NULL);
        GDV_DEBUG_ELEMENT ("Pushing a job to the queue", NULL);
        gdv_jobqueue_push_head (queue, job);
        
        return token;
}

/* Get the pixbuf of the pixbufsink */
GdkPixbuf               *gdv_framerunner_get_pixbuf (GdvFrameRunner *this)
{
        g_assert (GDV_IS_FRAMERUNNER (this));
        g_assert (GDV_IS_PIXBUFSINK (this->_PixbufSink));
        
        GDV_DEBUG_ELEMENT ("Getting a pixbuf of the sink", NULL);
        
        GdkPixbuf *pixbuf = NULL;
        g_object_get (G_OBJECT (this->_PixbufSink), "pixbuf", &pixbuf, NULL);
        
        if (pixbuf == NULL)
                GDV_WARNING ("Pixbuf not availible on this runner's sink", NULL);
        
        return pixbuf;
}
