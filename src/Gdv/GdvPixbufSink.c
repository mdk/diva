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

/* This is a sink that produces/stores pixbufs. It's used to generate the
 * frame thumbnails
 */

#include "GdvPixbufSink.h"
#include "GdvVideoBuffer.h"

enum {
        ARG_0,
        ARG_PIXBUF,
        ARG_FRAMEDIMENSIONS
};

static GstBaseSinkClass *ParentClass = NULL;

// FIXME: Actual caps here
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                   GST_PAD_SINK,
                                                                   GST_PAD_ALWAYS,
                                                                   GST_STATIC_CAPS
                                                                   ("video/x-raw-yuv, "
                                                                    "format = (fourcc) { I420, "
                                                                    "YUY2 }"));

/* GObject type */
GType                   gdv_pixbufsink_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvPixbufSinkClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_pixbufsink_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvPixbufSink), 
                        8, 
                        (GInstanceInitFunc) gdv_pixbufsink_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BASE_SINK, "GdvPixbufSink",
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
void                    gdv_pixbufsink_class_init (GdvPixbufSinkClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) (klass);
        GstBaseSinkClass *basesinkclass = (GstBaseSinkClass *) (klass);
        
        ParentClass = g_type_class_ref (GST_TYPE_BASE_SINK);
        
        gobjectclass->get_property = (gpointer) gdv_pixbufsink_get_property;
        gobjectclass->set_property = (gpointer) gdv_pixbufsink_set_property;
        gobjectclass->dispose = (gpointer) gdv_pixbufsink_dispose;
        
        g_object_class_install_property (gobjectclass, ARG_PIXBUF, 
                                         g_param_spec_object 
                                         ("pixbuf", "Pixbuf",
                                          "The pixbuf obtained",
                                          GDK_TYPE_PIXBUF,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_FRAMEDIMENSIONS,
                                         g_param_spec_boxed
					 ("framedimensions", "FrameDimensions",
					  "The frame dimensions to render",
					  GDV_TYPE_FRAMEDIMENSIONS, G_PARAM_READWRITE));
        
        basesinkclass->preroll = (gpointer) gdv_pixbufsink_preroll;
        elementclass->change_state = (gpointer) gdv_pixbufsink_change_state;
        
        // Create our sink pad template
        gst_element_class_add_pad_template (elementclass, 
                                            gst_static_pad_template_get (&sink_factory));
}

/* GObject init */
void                    gdv_pixbufsink_init (GdvPixbufSink *this)
{
        GDV_DEBUG_ELEMENT ("Initializing new GdvPixbufSink", NULL);
        
        this->_PropMutex = g_mutex_new ();
        this->_CondMutex = g_mutex_new ();
        this->_Cond = g_cond_new ();
        g_assert (this->_PropMutex != NULL);
        this->_Pixbuf = NULL;
        this->_FrameDimensions = gdv_framedimensions_new (90, 60);
        
        GST_BASE_SINK (this)->sync = TRUE;
        
}

/* Create a new FrameSink  */
GdvPixbufSink           *gdv_pixbufsink_new (void)
{
        GdvPixbufSink *this = g_object_new (GDV_TYPE_PIXBUFSINK, NULL);
        if (this == NULL)
                return NULL;
        
        GDV_DEBUG_ELEMENT ("Created new GdvPixbufSink element", NULL);
        
        return this;
}

/* One frame preroll. Our main function */
GstFlowReturn           gdv_pixbufsink_preroll (GdvPixbufSink *this, GstBuffer *buffer)
{
        GdvFrameDimensions *dimensions = NULL;

        GDV_DEBUG_ELEMENT ("Entering the preroll, locking the mutex", NULL);

        // We don't want no one to alter our properties while we're here
        g_mutex_lock (this->_CondMutex);
        g_mutex_lock (this->_PropMutex);
        
        GDV_DEBUG_ELEMENT ("Locked", NULL);
        
        GstCaps *buffercaps = GST_BUFFER_CAPS (buffer);
        GDV_DEBUG_ELEMENT ("Got buffer in preroll. Caps: %s", gst_caps_to_string (buffercaps));
        
        GstStructure *str = gst_caps_get_structure (buffercaps, 0);
        g_assert (str != NULL);
        
        gint nominator, denominator;
        double framerate = 0.0;
        int width = 0;
        int height = 0;
        guint32 fourcc = 0;
                
        gst_structure_get_fraction (str, "framerate", &nominator, &denominator);
        gst_structure_get_int (str, "width", &width);
        gst_structure_get_int (str, "height", &height);
        gst_structure_get_fourcc (str, "format", &fourcc);
        if (denominator != 0)
                framerate = (gdouble) nominator / (gdouble) denominator;
        
        g_assert (framerate != 0 && width != 0 && height != 0);
        
        GDV_DEBUG_ELEMENT ("Buffer %dx%d %.2f fps", width, height,
                   framerate);
        
        // Frame dimensions
        dimensions = gdv_framedimensions_new (width, height);

        GDV_DEBUG_ELEMENT ("Scaling buffer to: %dx%d", this->_FrameDimensions->Width, this->_FrameDimensions->Height);

        GdvVideoBuffer *outbuf = gdv_videobuffer_new ((GdvVideoBuffer *) buffer, GST_STR_FOURCC ("RGB2") , this->_FrameDimensions->Width, this->_FrameDimensions->Height);

        // We need to COPY the buffer data to be able to unref the buffer and destroy
        // the pixbuf later on. Without that we have no way of actually free the buffer struct
        // (we can only free it's data)

        GDV_DEBUG_ELEMENT ("Preparing pixbuf etc.", NULL);
        gpointer data = g_memdup (GST_BUFFER_DATA (outbuf), GST_BUFFER_SIZE (outbuf));
        g_assert (data != NULL);
                
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_data (data, GDK_COLORSPACE_RGB, FALSE,
                                                      8, this->_FrameDimensions->Width, this->_FrameDimensions->Height,
                                                      this->_FrameDimensions->Width * 3, gdv_util_pixbuf_destroyfunc, NULL);
        
        if (this->_Pixbuf != NULL)
                gdk_pixbuf_unref (this->_Pixbuf);
        
        this->_Pixbuf = pixbuf;
                                      
        /* FINISH */
        GDV_DEBUG_ELEMENT ("Unlocking the properties mutex", NULL);
        g_mutex_unlock (this->_PropMutex);
        GDV_DEBUG_ELEMENT ("Unlocked", NULL);
        
        GDV_DEBUG_ELEMENT ("Signalling the cond", NULL);
        // signal & unlock
        g_cond_signal (this->_Cond);
        GDV_DEBUG_ELEMENT ("Unlocking the cond mutex", NULL);
        g_mutex_unlock (this->_CondMutex);
        GDV_DEBUG_ELEMENT ("Unlocked", NULL);        
                
        if (dimensions != NULL)
                gdv_framedimensions_free (dimensions);
        
        GDV_DEBUG_ELEMENT ("Preroll done", NULL);
        gst_buffer_unref (outbuf);

        return GST_FLOW_OK;
}

/* GObject get property */
void                    gdv_pixbufsink_get_property (GdvPixbufSink *this, guint propid, 
                                                     GValue *value, GParamSpec *pspec)
{
        GDV_DEBUG_ELEMENT ("Getting the property %d, locking mutex", propid);
        g_mutex_lock (this->_PropMutex);
        GDV_DEBUG_ELEMENT ("Locked", NULL);
        
        switch (propid) {
                
                case ARG_PIXBUF:
                if (this->_Pixbuf != NULL) 
                        g_value_set_object (value, this->_Pixbuf);
                else 
                        GDV_WARNING_ELEMENT ("Trying to get a pixbuf property but it's NULL", NULL);
                break;
                
                case ARG_FRAMEDIMENSIONS:
                if (this->_FrameDimensions != NULL)
                        g_value_set_boxed (value, this->_FrameDimensions);
                else
                        GDV_WARNING_ELEMENT ("Trying to get a framedimensions property but it's NULL", NULL);
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
        
        GDV_DEBUG_ELEMENT ("Unlocking the mutex", NULL);
        g_mutex_unlock (this->_PropMutex);
        GDV_DEBUG_ELEMENT ("Unlocked", NULL);
} 

/* GObject set property */
void                    gdv_pixbufsink_set_property (GdvPixbufSink *this, guint propid, 
                                                    const GValue *value, GParamSpec *pspec)
{
        g_object_freeze_notify (G_OBJECT (this));
        GDV_DEBUG_ELEMENT ("Setting the property %d, locking mutex", propid);
        g_mutex_lock (this->_PropMutex);
        GDV_DEBUG_ELEMENT ("Locked", NULL);
        
        switch (propid) {
                
                case ARG_FRAMEDIMENSIONS:
                // Free the last one
                if (this->_FrameDimensions != NULL)
                        gdv_framedimensions_free (this->_FrameDimensions);
                                
                this->_FrameDimensions = g_value_dup_boxed (value);
                GDV_DEBUG_ELEMENT ("New rendering dimensions: %d x %d", this->_FrameDimensions->Width,
                                   this->_FrameDimensions->Height);
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
        
        GDV_DEBUG_ELEMENT ("Unlocking the mutex", NULL);
        g_mutex_unlock (this->_PropMutex);
        GDV_DEBUG_ELEMENT ("Unlocked", NULL);
        
        g_object_thaw_notify (G_OBJECT (this));
}

/* GObject dispose */
void                    gdv_pixbufsink_dispose (GdvPixbufSink *this)
{
        // FIXME: Implement
        g_assert_not_reached ();
}

/* Prepare for a signaled wait for a pixbuf */
void                    gdv_pixbufsink_lock (GdvPixbufSink *this)
{
        g_assert (GDV_IS_PIXBUFSINK (this) == TRUE);
        
        GDV_DEBUG_ELEMENT ("Lock on PixbufSink. Locking the CondMutext", NULL);
        g_mutex_lock (this->_CondMutex);
        GDV_DEBUG_ELEMENT ("Locked", NULL);
}

/* Wait for a pixbuf to appear */
void                    gdv_pixbufsink_wait (GdvPixbufSink *this)
{
        g_assert (GDV_IS_PIXBUFSINK (this) == TRUE);
        
        GDV_DEBUG_ELEMENT ("Waiting for a pixbuf/signal", NULL);
        g_cond_wait (this->_Cond, this->_CondMutex);
        GDV_DEBUG_ELEMENT ("Wait for a signal/pixbuf over", NULL);
}

/* Change state function */
GstStateChangeReturn    gdv_pixbufsink_change_state (GdvPixbufSink *this, GstStateChange transition)
{
        GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
        GDV_DEBUG_ELEMENT ("Changing state. Transition %d", transition);
        ret = GST_ELEMENT_CLASS (ParentClass)->change_state ((GstElement *) this, transition);
        
        return ret;
}
