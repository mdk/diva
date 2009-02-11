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

/* AVItem is a typicall item (usually based on a file) that HAS video and CAN have
 * audio. It can provide video frame cache and audio cache. It has a fixed length.
 */

#include "GdvAVItem.h"

enum {
        ARG_0,
        ARG_LENGTH,
        ARG_PIXBUF,
        ARG_SMALLPIXBUF
};

/* GObject type */
GType                   gdv_avitem_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvAVItemClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_avitem_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvAVItem), 
                        32, 
                        (GInstanceInitFunc) gdv_avitem_init,
                };
                
                // Frameable interface
                static const GInterfaceInfo timeableinfo = {
                        NULL,
                        NULL,             
                        NULL
                };
                
                // Timeable interface
                static const GInterfaceInfo frameableinfo = {
                        (GInterfaceInitFunc) gdv_avitem_frameable_init,
                        NULL,
                        NULL
                };
                
                // Thumbnailable interface
                static const GInterfaceInfo thumbnailableinfo = {
                        NULL,
                        NULL,
                        NULL
                };
                
                objecttype = g_type_register_static (GDV_TYPE_MEDIAITEM, "GdvAVItem",
                                                     &objectinfo, 0);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_TIMEABLE,
                                             &timeableinfo);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_FRAMEABLE,
                                             &frameableinfo);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_THUMBNAILABLE,
                                             &thumbnailableinfo);
        }
        return objecttype;
} 

/* GObject class init*/
void                    gdv_avitem_class_init (GdvAVItemClass *klass)
{
        g_type_class_ref (GDV_TYPE_MEDIAITEM);
        //GdvMediaItemClass *mediaitemclass = (GdvMediaItemClass *) klass;
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        gobjectclass->get_property = (gpointer) gdv_avitem_get_property;

        g_object_class_install_property (gobjectclass, ARG_LENGTH,
                                         g_param_spec_uint64
                                         ("length", "Length",
                                          "Length in nanoseconds",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_PIXBUF, 
                                         g_param_spec_object 
                                         ("thumbnail", "Thumbnail",
                                          "Thumbnail of the item",
                                          GDK_TYPE_PIXBUF,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_SMALLPIXBUF, 
                                         g_param_spec_object 
                                         ("smallthumbnail", "SmallThumbnail",
                                          "Small thumbnail of the item",
                                          GDK_TYPE_PIXBUF,
                                          G_PARAM_READABLE));
        
        gobjectclass->dispose = (gpointer) gdv_avitem_dispose;
}

/* GObject init */
void                    gdv_avitem_init (GdvAVItem *this)
{
        this->_Runner = NULL;
        this->_Thumbnail = NULL;
}

/* GObject get property */
void                    gdv_avitem_get_property (GdvAVItem *this, guint propid, 
                                                 GValue *value, GParamSpec *pspec)
{
        GDV_DEBUG_EXT ("Getting the property %d", propid);
        g_assert (GDV_IS_AVITEM (this));

        switch (propid) {
                
                case ARG_LENGTH: 
                {
                        // We return the length of the base source
                        g_assert (((GdvMediaItem *) this)->_BaseSource != NULL);
                        guint64 length = 0;
                        g_object_get (G_OBJECT (((GdvMediaItem *) this)->_BaseSource), "length", &length, NULL);
                        g_value_set_uint64 (value, length);
                }
                break;
                                
                case ARG_PIXBUF:
                GDV_DEBUG_EXT ("Accesing the item thumbnail", NULL);
                g_value_set_object (value, this->_Thumbnail);
                break;

                case ARG_SMALLPIXBUF:
                GDV_DEBUG_EXT ("Accesing the item small thumbnail", NULL);
                g_value_set_object (value, this->_SmallThumbnail);
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

// FIXME: Not needed at the moment (REMOVE?). Leaving for future
/* GObject set property */
void                    gdv_avitem_set_property (GdvAVItem *this, guint propid, 
                                                 const GValue *value, GParamSpec *pspec)
{
        // FIXME: Some kind of lock/mutex?!
        g_object_freeze_notify (G_OBJECT (this));
        
        switch (propid) {
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
        
        g_object_thaw_notify (G_OBJECT (this));
}

/* GObject dispose */
void                    gdv_avitem_dispose (GdvAVItem *this)
{
        g_assert (GDV_IS_AVITEM (this));
        GDV_DEBUG_EXT ("Disposing... [%s]", ((GdvMediaItem *) this)->_Name);

        // FIXME: Runner & Pixbuf
}

/* Create a new instance from a given source */
GdvAVItem               *gdv_avitem_new (GdvSource *source)
{
        g_assert (GDV_IS_SOURCE (source));
        gboolean hasvideo, hasaudio;
        g_object_get (G_OBJECT (source), "hasaudio", &hasaudio, "hasvideo", &hasvideo, NULL);
        if (hasvideo == FALSE) {
                GDV_WARNING_EXT ("Can't build item because source has no video", NULL);
                return NULL;
        }

        GDV_DEBUG_EXT ("Creating new GdvAVItem (hasaudio: %d hasvideo: %d)", hasaudio, hasvideo);
        GdvAVItem *this = g_object_new (GDV_TYPE_AVITEM, NULL);
        g_assert (this != NULL);

        // Video format
        GDV_DEBUG_EXT ("Stealing video format", NULL);
        GdvVideoFormat *videoformat = NULL;
        g_object_get (G_OBJECT (source), "videoformat", &videoformat, NULL);
        g_assert (videoformat != NULL);
        ((GdvMediaItem *) this)->_VideoFormat = videoformat;
        ((GdvMediaItem *) this)->_HasVideo = TRUE;
        
        // Audio format
        if (hasaudio == TRUE) {
                GDV_DEBUG_EXT ("Stealing audio format", NULL);
                GdvAudioFormat *audioformat = NULL;
                g_object_get (G_OBJECT (source), "audioformat", &audioformat, NULL);
                g_assert (audioformat != NULL);
                ((GdvMediaItem *) this)->_AudioFormat = audioformat;
                ((GdvMediaItem *) this)->_HasAudio = TRUE;
        }
        
        // Runner
        GDV_DEBUG_EXT ("Creating frame runner", NULL);
        this->_Runner = gdv_framerunner_new (source);
        if (this->_Runner != NULL) {
                GDV_DEBUG_EXT ("Getting the thumbnail from the runner", NULL);
                // Get the thumbnails
                this->_Thumbnail = gdv_framerunner_get_pixbuf (this->_Runner);

                GdvFrameDimensions *smalldimensions = gdv_framedimensions_new (0,0);
                gdv_source_get_small_thumb_dimensions (source, smalldimensions);
                
                this->_SmallThumbnail = gdk_pixbuf_scale_simple (this->_Thumbnail,
                                                                 smalldimensions->Width,
                                                                 smalldimensions->Height,
                                                                 GDK_INTERP_BILINEAR);
                gdv_framedimensions_free (smalldimensions);
                
        } else {
                this->_Thumbnail = NULL;
                this->_SmallThumbnail = NULL;
        }
        
        // Ref
        g_object_ref (G_OBJECT (source));
        ((GdvMediaItem *) this)->_BaseSource = source;

        return this;
}

/* Initializes the GdvFrameableInterface part of this object */
void                    gdv_avitem_frameable_init (GdvFrameableInterface *interface, gpointer data)
{
        GDV_DEBUG_EXT ("Initializing Frameable interface", NULL);
        interface->GetFrameFunc = (gpointer) gdv_avitem_frameable_get_frame;
}

/* FrameableInterface function that gets a frame. Actually it registers an 
 * idle function to call when frame will be ready. We just use the FrameRunner 
 * here */
GdvToken                gdv_avitem_frameable_get_frame (GdvAVItem *this, GdvTime attime,
                                                        GdvFrameDimensions *dim, GdvToken *settoken, 
                                                        GSourceFunc callback)
{
        g_assert (GDV_IS_AVITEM (this));
        
        GDV_DEBUG_EXT ("Scheduling a new job for the item FrameRunner", NULL);
        
        return gdv_framerunner_job_schedule (this->_Runner, attime, dim, settoken, callback);
}

