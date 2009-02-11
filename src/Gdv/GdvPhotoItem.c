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

/* This is an item to represent a video source without audio. The source doesn't have
 * a specified length. It has a thumbnail. One would typically want to use that for
 * photos (jpeg/png) files. We're using direct gtk pixbuf loading to limit the 
 * Gst load */

#include "GdvPhotoItem.h"

enum {
        ARG_0,
        ARG_PIXBUF,
        ARG_SMALLPIXBUF,
        ARG_BASE_FILE,
        ARG_PROJECTFORMAT,
};

/* GObject type */
GType                   gdv_photoitem_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvPhotoItemClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_photoitem_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvPhotoItem), 
                        32, 
                        (GInstanceInitFunc) gdv_photoitem_init,
                };
                
                // Thumbnailable interface
                static const GInterfaceInfo thumbnailableinfo = {
                        NULL,
                        NULL,
                        NULL
                };

                // FileBased interface
                static const GInterfaceInfo filebasedinfo = {
                        NULL,
                        NULL,
                        NULL
                };
                
                objecttype = g_type_register_static (GDV_TYPE_MEDIAITEM, "GdvPhotoItem",
                                                     &objectinfo, 0);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_FILE_BASED,
                                             &filebasedinfo);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_THUMBNAILABLE,
                                             &thumbnailableinfo);
        }
        return objecttype;
} 

/* GObject class init*/
void                    gdv_photoitem_class_init (GdvPhotoItemClass *klass)
{
        g_type_class_ref (GDV_TYPE_MEDIAITEM);
        GdvMediaItemClass *mediaitemclass = (GdvMediaItemClass *) klass;
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        gobjectclass->get_property = (gpointer) gdv_photoitem_get_property;

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
        
        g_object_class_install_property (gobjectclass, ARG_BASE_FILE,
                                         g_param_spec_string
                                         ("basefile", "BaseFile",
                                          "The media file we're based on",
                                          NULL, G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_PROJECTFORMAT, 
                                         g_param_spec_object 
                                         ("projectformat", "ProjectFormat",
                                          "Project format we used as a base",
                                          GDV_TYPE_PROJECTFORMAT,
                                          G_PARAM_READABLE));
        
        gobjectclass->dispose = (gpointer) gdv_photoitem_dispose;
        mediaitemclass->GetFreshSourceFunc = (gpointer) gdv_photoitem_get_fresh_source;
}

/* GObject init */
void                    gdv_photoitem_init (GdvPhotoItem *this)
{
        this->_Thumbnail = NULL;
        this->_ProjectFormat = NULL;
        this->_Filename = NULL;
}

/* GObject get property */
void                    gdv_photoitem_get_property (GdvPhotoItem *this, guint propid, 
                                                    GValue *value, GParamSpec *pspec)
{
        GDV_DEBUG_EXT ("Getting the property %d", propid);
        g_assert (GDV_IS_PHOTOITEM (this));

        switch (propid) {
                
                case ARG_PIXBUF:
                        g_assert (this->_Thumbnail != NULL);
                        g_value_set_object (value, this->_Thumbnail);
                        break;
                
                case ARG_SMALLPIXBUF:
                        g_assert (this->_SmallThumbnail != NULL);
                        g_value_set_object (value, this->_SmallThumbnail);
                        break;

                case ARG_BASE_FILE:
                        g_value_set_string (value, this->_Filename);
                        break;

                case ARG_PROJECTFORMAT:
                        g_assert (this->_ProjectFormat != NULL);
                        g_value_set_object (value, this->_ProjectFormat);
                        break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GObject dispose */
void                    gdv_photoitem_dispose (GdvPhotoItem *this)
{
        g_assert (GDV_IS_PHOTOITEM (this));
        GDV_DEBUG_EXT ("Disposing... [%s]", ((GdvMediaItem *) this)->_Name);
        
        if (this->_Thumbnail != NULL) {
                gdk_pixbuf_unref (this->_Thumbnail);
                this->_Thumbnail = NULL;
        }

        // FIXME: Proper dispose code goes here!
}

/* Create a new instance from a given source */
GdvPhotoItem            *gdv_photoitem_new (GdvProjectFormat *format, const gchar *filename)
{
        g_assert (filename != NULL);
        
        GstElement *sheep = (GstElement *) gdv_imagesheep_new (format, (gchar *) filename);
        GdvSource *source = (GdvSource *) gdv_sheepsource_new (sheep);
        gst_object_unref (sheep);
                
        gboolean hasvideo;
        g_object_get (G_OBJECT (source), "hasvideo", &hasvideo, NULL);
        if (hasvideo == FALSE) {
                GDV_WARNING_EXT ("Can't build item because source has no video", NULL);
                return NULL;
        }

        GDV_DEBUG_EXT ("Creating new GdvPhotoItem", hasvideo);
        GdvPhotoItem *this = g_object_new (GDV_TYPE_PHOTOITEM, NULL);
        g_assert (this != NULL);

        // Save our params for cloning etc.
        this->_ProjectFormat = format;
        g_object_ref (format);
        this->_Filename = g_strdup (filename);

        // Video format
        GDV_DEBUG_EXT ("Stealing video format", NULL);
        GdvVideoFormat *videoformat = NULL;
        g_object_get (G_OBJECT (source), "videoformat", &videoformat, NULL);
        g_assert (videoformat != NULL);
        ((GdvMediaItem *) this)->_VideoFormat = videoformat;
        ((GdvMediaItem *) this)->_HasVideo = TRUE;
        ((GdvMediaItem *) this)->_HasAudio = FALSE;
                
        // Ref
        g_object_ref (G_OBJECT (source));
        ((GdvMediaItem *) this)->_BaseSource = source;

        // Thumbnail
        gint width, height;
        gdk_pixbuf_get_file_info (filename, &width, &height);
        GdvFrameDimensions *orig_dimensions = gdv_framedimensions_new (width, height);
        GdvFrameDimensions *full_dimensions = gdv_framedimensions_new (0, 0);
        GdvFrameDimensions *small_dimensions = gdv_framedimensions_new (0, 0);        
        gdv_framedimensions_fit_h (orig_dimensions, 32, small_dimensions);
        gdv_framedimensions_fit_h (orig_dimensions, 50, full_dimensions);
        
        GDV_DEBUG_EXT ("Preparing thumbnail from file '%s'", filename);
        
        this->_Thumbnail = gdk_pixbuf_new_from_file_at_scale (filename,
                                                              full_dimensions->Width, 
                                                              full_dimensions->Height,
                                                              FALSE, NULL);
        
        this->_SmallThumbnail = gdk_pixbuf_new_from_file_at_scale (filename,
                                                                   small_dimensions->Width, 
                                                                   small_dimensions->Height,
                                                                   FALSE, NULL);
        
        if (this->_Thumbnail == NULL || this->_SmallThumbnail == NULL) {
                GDV_WARNING_EXT ("Can't build item because I couldn't load/scale thumbnail", NULL);
                g_object_unref (G_OBJECT (this));
        }
        
        gdv_framedimensions_free (orig_dimensions);
        gdv_framedimensions_free (small_dimensions);
        gdv_framedimensions_free (full_dimensions);
        return this;
}

GdvSource               *gdv_photoitem_get_fresh_source (GdvPhotoItem *this)
{
        GstElement *sheep = (GstElement *) gdv_imagesheep_new (this->_ProjectFormat, this->_Filename);
        GdvSource *source = (GdvSource *) gdv_sheepsource_new (sheep);
        //gst_object_unref (sheep);

        return source;
}
