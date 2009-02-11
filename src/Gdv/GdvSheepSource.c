///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//      Senko Rasic <senko@senko.net>                                        //
//                                                                           //
// Permission is hereby granted, free of charge, to any person obtaining a   //
// copy of this software and associated documentation files (the             //
// "Software"), to deal in the Software without restriction, including       //
// without limitation the rights to use, copy, modify, merge, publish,       //
// distribute, sublicense, and/or sell copies of the Software, and to permit //
// persons to whom the Software is furnished to do so, subject to the        //
// following conditions:                                                     //
//                                                                           //
// The above copyright notice and this permission notice shall be included   //
// in all copies or substantial portions of the Software.                    //
//                                                                           //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include "GdvSheepSource.h"
#include "GdvSheepSourcePrivate.h"

static                          GdvSourceClass *ParentClass = NULL;

/* GObject get type */
GType                           gdv_sheepsource_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvSheepSourceClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_sheepsource_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvSheepSource),
                        8,
                        (GInstanceInitFunc) gdv_sheepsource_init,
                };
                objecttype = g_type_register_static (GDV_TYPE_SOURCE, "GdvSheepSource", &objectinfo, 0);
        }

        return objecttype;
}

/* GObject class init */
static void                     gdv_sheepsource_class_init (GdvSheepSourceClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GdvSourceClass *sourceclass = (GdvSourceClass *) klass;
        GstElementClass *elementclass = (GstElementClass *) klass;

        ParentClass = g_type_class_ref (GDV_TYPE_SOURCE);

        gobjectclass->get_property = (gpointer) (gdv_sheepsource_get_property);

        g_object_class_install_property (gobjectclass, ARG_SHEEP,
                                         g_param_spec_object
                                         ("sheep", "Sheep",
                                          "The sheep we're based on",
                                          GDV_TYPE_SHEEP, G_PARAM_READABLE));
        
        gobjectclass->dispose = (gpointer) (gdv_sheepsource_dispose);
        gobjectclass->finalize = (gpointer) (gdv_sheepsource_finalize);
        sourceclass->CloneFunc = (GdvSourceCloneFunc) clone;
        elementclass->send_event = (gpointer) send_event;
}

/* GObject init */
static void                     gdv_sheepsource_init (GdvSheepSource *this)
{
        g_return_if_fail (GDV_IS_SHEEPSOURCE (this));
        GDV_DEBUG_ELEMENT ("Initializing fresh GdvSheepSource object", NULL);

        GdvSheepSourcePrivate *priv = g_new (GdvSheepSourcePrivate, 1);
        g_return_if_fail (priv != NULL);

        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* GObject finalize */
static void                     gdv_sheepsource_finalize (GdvSheepSource *this)
{
        g_return_if_fail (GDV_IS_SHEEPSOURCE (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject dispose */
static void                     gdv_sheepsource_dispose (GdvSheepSource *this)
{
        g_return_if_fail (GDV_IS_SHEEPSOURCE (this));
        GDV_DEBUG_ELEMENT ("Disposing...", NULL);

        if (this->Private != NULL)
                private_dispose (this->Private);

        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* Create a new object */
GdvSheepSource*                 gdv_sheepsource_new (GstElement *sheep)
{
        g_return_val_if_fail (GDV_IS_SHEEP (sheep), NULL);
        GDV_DEBUG_EXT ("Creating a new GdvSheepSource for %s sheep", GST_OBJECT_NAME (sheep));
        
        GdvSheepSource *this = NULL;
        
        this = g_object_new (GDV_TYPE_SHEEPSOURCE, NULL);
        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL)
                goto MrFailure;

        if (private_set (this, GDV_SHEEP (sheep)) == FALSE)
                goto MrFailure;

        // Create a video format based on caps of the sheep
        GstCaps *caps = gst_pad_get_caps (gst_element_get_pad (GST_ELEMENT (sheep),
                                                               "src"));
        g_return_val_if_fail (caps != NULL, NULL);

        GDV_DEBUG_ELEMENT ("Sheep caps are %s", gst_caps_to_string (caps));
        GdvVideoFormat *vformat = gdv_videoformat_new (caps);
        
        // Format set
        gdv_source_set_videoformat (GDV_SOURCE (this), vformat);
        gdv_source_set_video_target (GDV_SOURCE (this), gst_element_get_pad (sheep, "src"));
        gdv_source_set_length (GDV_SOURCE (this), 0);
        
        // FIXME: Unref pad
        // FIXME: Unref caps

        return this;

  MrFailure:
        if (this != NULL) {
                g_object_unref (this);
                this = NULL;
        }

        GDV_WARNING_EXT ("Failed to create new GdvSheepSource", NULL);
        return NULL;
}

/* Clone the source. We use the filename and the current params to do the trick */
static GdvSheepSource*          clone (GdvSheepSource *this)
{
        // Not clonable
        g_assert_not_reached ();
}

static gboolean                 send_event (GdvSheepSource *this, GstEvent *event)
{
        g_return_val_if_fail (GDV_IS_SHEEPSOURCE (this), FALSE);

        switch (GST_EVENT_TYPE (event)) {
                
                case GST_EVENT_SEEK: {
                        GDV_DEBUG_ELEMENT ("Got seek event. Passing it directly to the Parser element",
                                           NULL);
                        return gst_element_send_event (this->Private->Sheep, event);
                }

                default: {
                        return gdv_source_send_event_stock (GDV_SOURCE (this), event);
                } break;

        }
        
}

/* GObject get property */
static void                     gdv_sheepsource_get_property (GdvSheepSource *this, guint propid,
                                                              GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_SHEEPSOURCE (this));

        switch (propid) {

                case ARG_SHEEP:
                g_assert (this->Private->Sheep != NULL);
                g_value_set_object (value, this->Private->Sheep);
                break;

                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

static gboolean                 private_init (GdvSheepSourcePrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        private->Sheep = NULL;

        return TRUE;
}

static gboolean                 private_set (GdvSheepSource *this, GdvSheep *sheep)
{
        g_return_val_if_fail (GDV_IS_SHEEPSOURCE (this), FALSE);
        g_return_val_if_fail (GDV_IS_SHEEP (sheep), FALSE);

        g_return_val_if_fail (gst_bin_add (GST_BIN (this), GST_ELEMENT (sheep)), FALSE);
        this->Private->Sheep = (GstElement *) sheep;
        gst_object_ref (sheep);

        return TRUE;
}

static void                     private_dispose (GdvSheepSourcePrivate *private)
{
        g_return_if_fail (private != NULL);

        if (private->Sheep != NULL) {
                gst_object_unref (private->Sheep);
                private->Sheep = NULL;
        }
}

