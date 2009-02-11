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

/* This is a base class for MediaItem. MediaItem is something that can be used in
 * a project (don't confuse with Source!). The base class has just some common & 
 * generic params. All the rest is provided by interfaces. 
 * A file in a project is imported as a MediaItem. But MediaItem can represent a 
 * stock item as well (a stock playing "black" or something).
 */

#include "GdvMediaItem.h"

enum {
        ARG_0,
        ARG_NAME,
        ARG_HASVIDEO,
        ARG_HASAUDIO,
        ARG_VIDEOFORMAT,
        ARG_AUDIOFORMAT,
        ARG_BASE_SOURCE
};

/* GObject type */
GType                   gdv_mediaitem_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvMediaItemClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_mediaitem_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvMediaItem), 
                        32, 
                        (GInstanceInitFunc) gdv_mediaitem_init,
                };
                objecttype = g_type_register_static (G_TYPE_OBJECT, "GdvMediaItem",
                                                     &objectinfo, 0);
        }
        return objecttype;
} 

/* GObject class init*/
void                    gdv_mediaitem_class_init (GdvMediaItemClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        gobjectclass->get_property = (gpointer) gdv_mediaitem_get_property;
        gobjectclass->set_property = (gpointer) gdv_mediaitem_set_property;
        
        g_object_class_install_property (gobjectclass, ARG_NAME,
					 g_param_spec_string
					 ("name", "Name",
					  "The name of this item",
					  "Noname", G_PARAM_READWRITE));
        
        g_object_class_install_property (gobjectclass, ARG_HASAUDIO,
                                         g_param_spec_boolean 
                                         ("hasaudio", "HasAudio",
                                          "If this item has audio",
                                          FALSE, G_PARAM_READABLE));
        
        g_object_class_install_property (gobjectclass, ARG_HASVIDEO,
                                         g_param_spec_boolean 
                                         ("hasvideo", "HasVideo",
                                          "If this item has video",
                                          FALSE, G_PARAM_READABLE));
                
        g_object_class_install_property (gobjectclass, ARG_VIDEOFORMAT,
                                         g_param_spec_object
                                         ("videoformat", "VideoFormat",
                                          "VideoFormat object of this source",
                                          GDV_TYPE_VIDEOFORMAT,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_AUDIOFORMAT,
                                         g_param_spec_object
                                         ("audioformat", "AudioFormat",
                                          "AudioFormat object of this source",
                                          GDV_TYPE_AUDIOFORMAT,
                                          G_PARAM_READABLE));
       
        g_object_class_install_property (gobjectclass, ARG_BASE_SOURCE,
                                         g_param_spec_object
                                         ("basesource", "BaseSource",
                                          "The source used as the base for the item",
                                          GDV_TYPE_SOURCE,
                                          G_PARAM_READABLE));
  
        // FIXME Really needed?
        g_type_class_ref (G_TYPE_OBJECT);
        gobjectclass->dispose = (gpointer) gdv_mediaitem_dispose;

        klass->GetFreshSourceFunc = (GdvMediaItemSourceFunc) gdv_mediaitem_get_fresh_source_stock;
}

/* GObject init */
void                    gdv_mediaitem_init (GdvMediaItem *this)
{
        this->_Name = g_strdup (gettext ("Undefined"));
        this->_VideoFormat = NULL;
        this->_AudioFormat = NULL;
        this->_HasVideo = FALSE;
        this->_HasAudio = FALSE;
        this->_BaseSource = NULL;
}

/* GObject get property */
void                    gdv_mediaitem_get_property (GdvMediaItem *this, guint propid, 
                                                    GValue *value, GParamSpec *pspec)
{
        g_assert (GDV_IS_MEDIAITEM (this));

        switch (propid) {
                                
                case ARG_NAME:
                g_assert (this->_Name != NULL);
                g_value_set_string (value, this->_Name);
                break;
                
                case ARG_HASVIDEO:
                g_value_set_boolean (value, this->_HasVideo);
                break;

                case ARG_HASAUDIO:
                g_value_set_boolean (value, this->_HasAudio);
                break;

                case ARG_VIDEOFORMAT:
                g_assert (this->_VideoFormat != NULL);
                g_value_set_object (value, this->_VideoFormat);
                break;

                case ARG_AUDIOFORMAT:
                g_assert (this->_AudioFormat != NULL);
                g_value_set_object (value, this->_AudioFormat);
                break;

                case ARG_BASE_SOURCE:
                g_assert (this->_BaseSource != NULL);
                g_value_set_object (value, this->_BaseSource);
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (this, propid, pspec);
                break;
        }
}

/* GObject set property */
void                    gdv_mediaitem_set_property (GdvMediaItem *this, guint propid, 
                                                    GValue *value, GParamSpec *pspec)
{
        g_assert (GDV_IS_MEDIAITEM (this));

        switch (propid) {
                                
                case ARG_NAME:
                if (this->_Name != NULL)
                        g_free (this->_Name);
                this->_Name = g_value_dup_string (value);
                break;
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (this, propid, pspec);
                break;
        }
}

/* GObject dispose */
void                    gdv_mediaitem_dispose (GdvMediaItem *this)
{
        g_assert (GDV_IS_MEDIAITEM (this));

        GDV_DEBUG_EXT ("Disposing... [%s]", this->_Name);

        if (this->_Name != NULL) {
                g_free (this->_Name);
                this->_Name = NULL;
        }
        
        if (this->_VideoFormat != NULL) {
                g_object_unref (this->_VideoFormat);
                this->_VideoFormat = NULL;
        }
        
        if (this->_AudioFormat != NULL) {
                g_object_unref (this->_AudioFormat);
                this->_AudioFormat = NULL;
        }

        if (this->_BaseSource != NULL) {
                g_object_unref (G_OBJECT (this->_BaseSource));
                this->_BaseSource = NULL;
        }
}

/* Get a fresh copy of the source for this MediaItem */
GdvSource               *gdv_mediaitem_get_fresh_source (GdvMediaItem *this)
{
        g_assert (GDV_IS_MEDIAITEM (this));
                        
        GdvMediaItemClass *klass = (GdvMediaItemClass *) G_OBJECT_GET_CLASS(this);
        g_assert (klass != NULL);
        g_return_val_if_fail (klass->GetFreshSourceFunc != NULL, NULL);
        
        GDV_DEBUG_EXT ("Getting a fresh source for GdvMediaItem", NULL);
        return klass->GetFreshSourceFunc (this);
}

/* This is a stock implementation of this function. Should work fine in most
 * cases. I don't thing we need to go anymore than that */
GdvSource               *gdv_mediaitem_get_fresh_source_stock (GdvMediaItem *this)
{
        g_assert (GDV_IS_MEDIAITEM (this));
        g_return_val_if_fail (this->_BaseSource != NULL, NULL);
        GDV_DEBUG_EXT ("Getting a fresh source for GdvMediaItem, cloning '%s'", 
                       GST_OBJECT_NAME (this->_BaseSource));
        
        return gdv_source_clone (this->_BaseSource);
}
