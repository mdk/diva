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

#include "GdvImageSheep.h"
#include "GdvImageSheepPrivate.h"

static GstElementClass*         ParentClass = NULL;

/* GObject type */
GType                           gdv_imagesheep_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvImageSheepClass),
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_imagesheep_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvImageSheep), 
                        4, 
                        (GInstanceInitFunc) gdv_imagesheep_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_SHEEP, "GdvImageSheep", &objectinfo, 0);

                // FileBased interface
                static const GInterfaceInfo filebasedinfo = {
                        NULL,
                        NULL,
                        NULL
                };
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_FILE_BASED,
                                             &filebasedinfo);
        }
        return objecttype;
}

static void                     gdv_imagesheep_class_init (GdvImageSheepClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) (klass);
        GdvSheepClass *sheepclass = (GdvSheepClass *) (klass);
        
        // Ref stuff 
        ParentClass = g_type_class_ref (GDV_TYPE_SHEEP);

        gobjectclass->get_property = (gpointer) (gdv_imagesheep_get_property);
        
        g_object_class_install_property (gobjectclass, ARG_BASE_FILE,
                                         g_param_spec_string
                                         ("basefile", "BaseFile",
                                          "The media file we're based on",
                                          NULL, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_PROJECTFORMAT,
                                         g_param_spec_object
                                         ("projectformat", "ProjectFormat",
                                          "Project format we're using",
                                          GDV_TYPE_PROJECTFORMAT, G_PARAM_READABLE));
        
        sheepclass->buffer_func = (gpointer) buffer_func;
        sheepclass->get_caps = (gpointer) get_caps_function;
        gobjectclass->dispose = (gpointer) gdv_imagesheep_dispose;
        gobjectclass->finalize = (gpointer) gdv_imagesheep_finalize;
        
        elementclass->change_state = (gpointer) change_state;
}

static void                     gdv_imagesheep_init (GdvImageSheep *this)
{
        GDV_DEBUG_EXT ("Initializing new GdvImageSheep element", NULL);
        
        g_return_if_fail (GDV_IS_IMAGESHEEP (this));
        this->Private = NULL;

        GdvImageSheepPrivate *priv = g_new0 (GdvImageSheepPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                g_free (priv);
                this->Private = NULL;
        }
}

/* Create a new instance */
GdvImageSheep*                  gdv_imagesheep_new (GdvProjectFormat *format, gchar *filename)
{
        g_return_val_if_fail (GDV_IS_PROJECTFORMAT (format), NULL);
        
        GDV_DEBUG_EXT ("Creating a new GdvImageSheep", NULL);
        
        GdvImageSheep *this = g_object_new (GDV_TYPE_IMAGESHEEP, NULL);
        if (this == NULL)
                return NULL;
        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }

        private_set (this->Private, format, filename);

        return this;
}

static GstBuffer*               buffer_func (GdvImageSheep *this, GstPad *pad, GdvTime time, GdvTime endtime)
{
        g_return_val_if_fail (this->Private->MasterBuffer != NULL, NULL);
        
        GstBuffer *buffer = gst_buffer_copy (this->Private->MasterBuffer);

        GST_BUFFER_TIMESTAMP (buffer) = time;
        GST_BUFFER_DURATION (buffer) = this->Private->BufferLength;

        return buffer;
}

static GstCaps*                 get_caps_function (GdvImageSheep *this, GstPad *pad)
{
        
        g_return_val_if_fail (GDV_IS_IMAGESHEEP (this), NULL);
        g_return_val_if_fail (pad != NULL, NULL);

        return gst_caps_ref (this->Private->MasterCaps);
}

/* GObject dispose */
static void                     gdv_imagesheep_dispose (GdvImageSheep *this)
{
        g_return_if_fail (GDV_IS_IMAGESHEEP (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_imagesheep_finalize (GdvImageSheep *this)
{
        g_return_if_fail (GDV_IS_IMAGESHEEP (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Private init */
static gboolean                 private_init (GdvImageSheep *this, GdvImageSheepPrivate *private)
{
        g_return_val_if_fail (GDV_IS_IMAGESHEEP (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);

        private->BufferLength = 0;
        private->FrameDimensions = NULL;
        private->MasterCaps = NULL;
        private->Filename = NULL;
        private->MasterBuffer = NULL;
        
        return TRUE;
}

/* Private set */
static gboolean                 private_set (GdvImageSheepPrivate *private,
                                             GdvProjectFormat *format, gchar *filename)
{
        g_return_val_if_fail (private != NULL, FALSE);

        // Project format
        GdvVideoFormat *vformat = NULL;
        g_object_get (G_OBJECT (format), "videoformat", &vformat, NULL);
        private->ProjectFormat = format;
        g_object_ref (G_OBJECT (format));
        
        // Master caps
        g_object_get (G_OBJECT (vformat), "caps", &private->MasterCaps, NULL);
        g_return_val_if_fail (private->MasterCaps, FALSE);

        // Copy the filename
        private->Filename = g_strdup (filename);
        g_return_val_if_fail (private->Filename != NULL, FALSE);

        // Get the frame dimensions
        g_object_get (G_OBJECT (vformat), "framedimensions", &private->FrameDimensions, NULL);
        g_return_val_if_fail (private->FrameDimensions != NULL, FALSE);

        // Get the buffer length we're streaming
        GdvFraction *fps;
        g_object_get (G_OBJECT (vformat), "fps", &fps, NULL);
        g_return_val_if_fail (fps != NULL, FALSE);
        private->BufferLength = gdv_fraction_fps_frame_duration (fps);

        g_object_unref (vformat);
        return TRUE;
}

/* Prepare the buffer we'll be streaming */
static gboolean                 prepare_buffer_data (GdvImageSheep *this)
{
        g_return_val_if_fail (GDV_IS_IMAGESHEEP (this), FALSE);
        g_return_val_if_fail (this->Private->FrameDimensions != NULL, FALSE);
        g_return_val_if_fail (this->Private->MasterCaps != NULL, FALSE);

        GDV_DEBUG_ELEMENT ("Preparing YUY data from pixbuf", FALSE);

        // Load the gkt image
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (this->Private->Filename,
                                                      NULL);
        g_return_val_if_fail (pixbuf != NULL, FALSE);

        GdvFrameDimensions *pixbufdimensions = gdv_framedimensions_new (gdk_pixbuf_get_width (pixbuf),
                                                                        gdk_pixbuf_get_height (pixbuf));

        GdvFrameDimensions *internaldimensions = gdv_framedimensions_new (1,1);
        GdvFraction *pixbufAspect = gdv_fraction_aspect_new_from_frame (pixbufdimensions);
        GdvFraction *frameAspect = gdv_fraction_aspect_new_from_frame (this->Private->FrameDimensions);
        
        // Do the fitting
        if (gdv_fraction_to_double (pixbufAspect) > 
            gdv_fraction_to_double (frameAspect))
                gdv_framedimensions_fit_w (pixbufdimensions,
                                           this->Private->FrameDimensions->Width,
                                           internaldimensions);
        else
                gdv_framedimensions_fit_h (pixbufdimensions,
                                           this->Private->FrameDimensions->Height,
                                           internaldimensions);
        
        gdv_framedimensions_roundify (internaldimensions, 2, 2);
        
        // Allocate our temp buffer
        gpointer *store = g_malloc (internaldimensions->Width *
                                    internaldimensions->Height * 2);
        g_return_val_if_fail (store != NULL, FALSE);

        // Convert and scale
        if (gdk_pixbuf_get_has_alpha(pixbuf)) {
            gdv_rgba_to_yuy2 (gdk_pixbuf_get_pixels (pixbuf),
                             pixbufdimensions->Width,
                             pixbufdimensions->Height,
                             gdk_pixbuf_get_rowstride (pixbuf), // rgb stride
                             store,
                             internaldimensions->Width,
                             internaldimensions->Height,
                             2 * internaldimensions->Width); // yuy2 stride ignored
        } else {
            gdv_rgb_to_yuy2 (gdk_pixbuf_get_pixels (pixbuf),
                             pixbufdimensions->Width,
                             pixbufdimensions->Height,
                             gdk_pixbuf_get_rowstride (pixbuf), // rgb stride
                             store,
                             internaldimensions->Width,
                             internaldimensions->Height,
                             2 * internaldimensions->Width); // yuy2 stride ignored
        }

        // Allocate our main buffer
        gpointer yuydata  = g_malloc (this->Private->FrameDimensions->Width *
                                      this->Private->FrameDimensions->Height * 2);
        g_return_val_if_fail (yuydata != NULL, FALSE);

        // Fill out with blackness
        int i = 0;
        guchar *yuy = yuydata;
        for (i = 0; i < this->Private->FrameDimensions->Width * this->Private->FrameDimensions->Height * 2; i += 2) {
                yuy [i] = 0;
                yuy [i + 1] = 128;
        }

        // Calculate the x/y vals
        int x = (this->Private->FrameDimensions->Width - internaldimensions->Width) / 2;
        int y = (this->Private->FrameDimensions->Height - internaldimensions->Height) / 2;

        // Now copy the converted to the primary
        gdv_videobuffer_blitter_yuy2 (store, x, y,
                                      internaldimensions->Width,
                                      internaldimensions->Height,
                                      yuydata,
                                      this->Private->FrameDimensions->Width,
                                      this->Private->FrameDimensions->Height,
                                      1.0);

        this->Private->MasterBuffer = gst_buffer_new ();
        gst_buffer_set_caps (this->Private->MasterBuffer, this->Private->MasterCaps);
        gst_buffer_set_data (this->Private->MasterBuffer, yuydata,
                             this->Private->FrameDimensions->Width *
                             this->Private->FrameDimensions->Height * 2);

        GST_BUFFER_MALLOCDATA (this->Private->MasterBuffer) = yuydata;

        // Free
        if (pixbuf != NULL)
                gdk_pixbuf_unref (pixbuf);
        if (store != NULL)
                g_free (store);
        if (internaldimensions != NULL)
                gdv_framedimensions_free (internaldimensions);
        if (pixbufdimensions != NULL)
                gdv_framedimensions_free (pixbufdimensions);
        if (pixbufAspect != NULL)
                gdv_fraction_free (pixbufAspect);
        if (frameAspect != NULL)
                gdv_fraction_free (frameAspect);

        return TRUE;
}

static void                     private_dispose (GdvImageSheepPrivate *private)
{
        // FIXME: !!!
}

/* GstElement state change */
static GstStateChangeReturn     change_state (GdvImageSheep *this, GstStateChange transition)
{
        g_return_val_if_fail (GDV_IS_IMAGESHEEP (this), GST_STATE_CHANGE_FAILURE);
        GDV_DEBUG_ELEMENT ("Changing state", NULL);
        
        GstStateChangeReturn ret;
        
        switch (transition) {
                
                case GST_STATE_CHANGE_READY_TO_PAUSED: {
                        prepare_buffer_data (this);
                        g_return_val_if_fail (this->Private->MasterBuffer != NULL, GST_STATE_CHANGE_FAILURE);
                } break;

                case GST_STATE_CHANGE_PAUSED_TO_READY: {
                        if (this->Private->MasterBuffer != NULL) {
                                gst_buffer_unref (this->Private->MasterBuffer);
                                this->Private->MasterBuffer = NULL;
                        }
                } break;
                        
                default: 
                break;
        }
        
        ret = GST_ELEMENT_CLASS (ParentClass)->change_state (GST_ELEMENT (this), 
                                                             transition);
        return ret;
}

/* GObject get property */
static void                     gdv_imagesheep_get_property (GdvImageSheep *this, guint propid,
                                                             GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_IMAGESHEEP (this));

        switch (propid) {
                
                case ARG_BASE_FILE:
                        g_value_set_string (value, this->Private->Filename);
                        break;

                case ARG_PROJECTFORMAT:
                        g_assert (this->Private->ProjectFormat != NULL);
                        g_value_set_object (value, this->Private->ProjectFormat);
                        break;
                        
                default:
                        G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                        break;
        }
}
