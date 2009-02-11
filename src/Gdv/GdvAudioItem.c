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

/* AVItem is a typicall item (usually based on a file) that HAS audio and 
 * doesn't have video. It's used to represent mp3's etc. It has a fixed length.
 */

#include "GdvAudioItem.h"

enum {
        ARG_0,
        ARG_LENGTH,
};

/* GObject type */
GType                   gdv_audioitem_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvAudioItemClass), 
                        NULL, 
                        NULL,
                        (GClassInitFunc) gdv_audioitem_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvAudioItem), 
                        32, 
                        (GInstanceInitFunc) gdv_audioitem_init,
                };
                
                // Timeable interface
                static const GInterfaceInfo timeableinfo = {
                        NULL,
                        NULL,
                        NULL
                };
                
                objecttype = g_type_register_static (GDV_TYPE_MEDIAITEM, "GdvAudioItem",
                                                     &objectinfo, 0);
                
                g_type_add_interface_static (objecttype,
                                             GDV_TYPE_TIMEABLE,
                                             &timeableinfo);
                
        }
        return objecttype;
} 

/* GObject class init*/
void                    gdv_audioitem_class_init (GdvAudioItemClass *klass)
{
        g_type_class_ref (GDV_TYPE_MEDIAITEM);
        //GdvMediaItemClass *mediaitemclass = (GdvMediaItemClass *) klass;
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        gobjectclass->get_property = (gpointer) gdv_audioitem_get_property;

        g_object_class_install_property (gobjectclass, ARG_LENGTH,
                                         g_param_spec_uint64
                                         ("length", "Length",
                                          "Length in nanoseconds",
                                          0, G_MAXUINT64, 0, G_PARAM_READABLE));
        
        gobjectclass->dispose = (gpointer) gdv_audioitem_dispose;
}

/* GObject init */
void                    gdv_audioitem_init (GdvAudioItem *this)
{
}

/* GObject get property */
void                    gdv_audioitem_get_property (GdvAudioItem *this, guint propid, 
                                                    GValue *value, GParamSpec *pspec)
{
        GDV_DEBUG_EXT ("Getting the property %d", propid);
        g_assert (GDV_IS_AUDIOITEM (this));

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
                
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}

/* GObject dispose */
void                    gdv_audioitem_dispose (GdvAudioItem *this)
{
        g_assert (GDV_IS_AUDIOITEM (this));
        GDV_DEBUG_EXT ("Disposing... [%s]", ((GdvMediaItem *) this)->_Name);
}

/* Create a new instance from a given source */
GdvAudioItem            *gdv_audioitem_new (GdvSource *source)
{
        g_assert (GDV_IS_SOURCE (source));
        gboolean hasaudio;
        g_object_get (G_OBJECT (source), "hasaudio", &hasaudio, NULL);
        if (hasaudio == FALSE) {
                GDV_WARNING_EXT ("Can't build item because source has no audio", NULL);
                return NULL;
        }

        GDV_DEBUG_EXT ("Creating new GdvAudioItem", NULL);
        GdvAudioItem *this = g_object_new (GDV_TYPE_AUDIOITEM, NULL);
        g_assert (this != NULL);

        // Audio format
        GDV_DEBUG_EXT ("Stealing audio format", NULL);
        GdvAudioFormat *audioformat = NULL;
        g_object_get (G_OBJECT (source), "audioformat", &audioformat, NULL);
        g_assert (audioformat != NULL);
        ((GdvMediaItem *) this)->_AudioFormat = audioformat;
        ((GdvMediaItem *) this)->_HasAudio = TRUE;
        
        // Ref
        g_object_ref (G_OBJECT (source));
        ((GdvMediaItem *) this)->_BaseSource = source;

        return this;
}
