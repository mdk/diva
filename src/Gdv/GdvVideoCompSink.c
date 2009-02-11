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

#include "GdvVideoCompSink.h"
#include "GdvVideoCompSinkPrivate.h"

static GdvCompSinkClass*        ParentClass = NULL;

static GstStaticPadTemplate     SinkFactory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                       GST_PAD_SINK,
                                                                       GST_PAD_REQUEST,
                                                                       VIDEO_CAPS);

static gboolean                 private_init (GdvVideoCompSink *this, GdvVideoCompSinkPrivate *priv)
{
        g_return_val_if_fail (GDV_IS_VIDEOCOMPSINK (this) && priv != NULL, FALSE);
        
        // Boy
        priv->Boy = NULL;
                
        return TRUE;
}

static gboolean                 private_dispose (GdvVideoCompSink *this, GdvVideoCompSinkPrivate *priv)
{
        g_return_val_if_fail (GDV_IS_VIDEOCOMPSINK (this) && priv != NULL, FALSE);

        if (priv->Boy != NULL) {
                g_object_unref (G_OBJECT (priv->Boy));
                priv->Boy = NULL;
        }
                
        return TRUE;
}

/* Base init to add the template */
static void                     gdv_videocompsink_base_init (gpointer klass)
{
        GstElementClass *elementclass = GST_ELEMENT_CLASS (klass);

        GDV_DEBUG_EXT ("Adding sink pad template", NULL);
        gst_element_class_add_pad_template (elementclass, 
                                            gst_static_pad_template_get (&SinkFactory));
}

/* GObject type */
GType                           gdv_videocompsink_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvVideoCompSinkClass), 
                        gdv_videocompsink_base_init, 
                        NULL,
                        (GClassInitFunc) gdv_videocompsink_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvVideoCompSink),
                        1,
                        (GInstanceInitFunc) gdv_videocompsink_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_COMPSINK, "GdvVideoCompSink",
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_videocompsink_class_init (GdvVideoCompSinkClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GdvCompSinkClass *compsinkclass = (GdvCompSinkClass *) klass;
        
        ParentClass = g_type_class_ref (GDV_TYPE_COMPSINK);

        gobjectclass->dispose = (gpointer) gdv_videocompsink_dispose;
        gobjectclass->finalize = (gpointer) gdv_videocompsink_finalize;

        compsinkclass->ComposeFunc = (gpointer) compose_func;
}

/* GObject init */
static void                     gdv_videocompsink_init (GdvVideoCompSink *this)
{
        g_return_if_fail (GDV_IS_VIDEOCOMPSINK (this));
        
        GDV_DEBUG_ELEMENT ("Initializing new GdvVideoCompSink", NULL);
        
        this->Private = NULL;
        
        GdvVideoCompSinkPrivate *priv = g_new0 (GdvVideoCompSinkPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Create a new instance */
GdvVideoCompSink*               gdv_videocompsink_new ()
{
        GdvVideoCompSink *this = NULL;
        
        this = g_object_new (GDV_TYPE_VIDEOCOMPSINK, NULL);
        g_return_val_if_fail (this != NULL, NULL);
        
        if (this->Private == NULL) {
                g_object_unref (G_OBJECT (this));
                return NULL;
        } else
                return this;
}

gboolean                        gdv_videocompsink_set_format (GdvVideoCompSink *this, GdvProjectFormat *format)
{
        g_return_val_if_fail (GDV_IS_VIDEOCOMPSINK (this), FALSE);
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), FALSE);

        // Caps
        GDV_DEBUG_ELEMENT ("Setting the video format", NULL);

        GdvVideoFormat *vformat = NULL;
        g_object_get (G_OBJECT (format), "videoformat", &vformat, NULL);
        g_return_val_if_fail (vformat != NULL, FALSE);

        GstCaps *caps;
        GstCaps *loosecaps;

        GstStaticCaps staticcaps = VIDEO_CAPS;
        loosecaps = gst_caps_make_writable (gst_static_caps_get (&staticcaps));
        g_return_val_if_fail (loosecaps != NULL, FALSE);

        // FIXME: Most probbably we should slightly alter the caps -- add framerate and such
        // like in GdvBlackSheep
        
        g_object_get (G_OBJECT (vformat), "caps", &caps, NULL);
        g_return_val_if_fail (caps != NULL, FALSE);
        
        // Call the parent func
        gdv_compsink_set_master_caps (GDV_COMPSINK (this), loosecaps);
        
        // Set other common params
        GdvFraction *fps = NULL;
        g_object_get (G_OBJECT (vformat), "fps", &fps, NULL);
        g_return_val_if_fail (fps != NULL, FALSE);
        
        this->Private->BufferLength = gdv_fraction_fps_frame_duration (fps); 
        gdv_fraction_free (fps);

        // Create our video boy
        this->Private->Boy = gdv_videoboy_new (gst_element_get_pad (GST_ELEMENT (this), "src"), caps, USE_PAD_ALLOC);
        // FIXME: Pad unref?

        g_object_unref (vformat);
        gst_caps_unref (caps);
        return TRUE;
}

/* Link a track video to this sink */
gboolean                        gdv_videocompsink_link_track (GdvVideoCompSink *this, GdvTrack *track)
{
        g_return_val_if_fail (GDV_IS_TRACK (track), FALSE);
        g_return_val_if_fail (GDV_IS_VIDEOCOMPSINK (this), FALSE);
        g_assert (this->Private != NULL);
        
        GDV_DEBUG_ELEMENT ("Trying to link in track '%s'", GST_OBJECT_NAME (track));
        return gst_element_link_pads (GST_ELEMENT (track), 
                                      "vsrc", GST_ELEMENT (this), NULL);
}

static GstBuffer*               compose_func (GdvVideoCompSink *this, GSList *inputs, GdvTime now)
{
        g_return_val_if_fail (GDV_IS_VIDEOCOMPSINK (this), NULL);

        GDV_DEBUG_ELEMENT ("Starting composing using boy", NULL);

        // Start composing
        gdv_videoboy_start_composing (this->Private->Boy);
	
        GSList *iterator;
        for (iterator = inputs; iterator; iterator = g_slist_next (iterator)) {
                GdvCompSinkInput *input = (GdvCompSinkInput *) iterator->data;
                g_assert (input != NULL);
                
                gdv_videoboy_add_buffer (this->Private->Boy, input->Buffer);
        }
 	
        g_debug ("COMPOSING!");
        // Get buffer, stop composing
        GstBuffer *buffer = gdv_videoboy_get_rendered_buffer (this->Private->Boy);
        GST_BUFFER_TIMESTAMP (buffer) = now;
        GST_BUFFER_DURATION (buffer) = this->Private->BufferLength;
        
        return buffer;
}

/* GObject dispose */
static void                     gdv_videocompsink_dispose (GdvVideoCompSink *this)
{
        g_return_if_fail (GDV_IS_VIDEOCOMPSINK (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        // Dispose
        if (this->Private != NULL) 
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_videocompsink_finalize (GdvVideoCompSink *this)
{
        g_return_if_fail (GDV_IS_VIDEOCOMPSINK (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);

        // Free
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}
