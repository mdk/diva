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

#include "GdvProjectFormat.h"

/* Global */

static GObjectClass*            ParentClass;

/* Public */

enum {
        ARG_0, 
        ARG_VIDEO_FORMAT,
        ARG_AUDIO_FORMAT
};

/* GObject type */
GType                           gdv_projectformat_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {
                
                static const GTypeInfo objectinfo = {
                        sizeof (GdvProjectFormatClass),
                        NULL,
                        NULL,
                        (GClassInitFunc) gdv_projectformat_class_init,
                        NULL,
                        NULL,
                        sizeof (GdvProjectFormat),
                        2,
                        (GInstanceInitFunc) gdv_projectformat_init,
                };
                objecttype = g_type_register_static
                        (G_TYPE_OBJECT, "GdvProjectFormat", &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
void                            gdv_projectformat_class_init (GdvProjectFormatClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;

        ParentClass = g_type_class_ref (G_TYPE_OBJECT);
        
        gobjectclass->get_property = (gpointer) gdv_projectformat_get_property;

        g_object_class_install_property (gobjectclass, ARG_VIDEO_FORMAT, 
                                         g_param_spec_object 
                                         ("videoformat", "VideoFormat",
                                          "The video format of this project",
                                          GDV_TYPE_VIDEOFORMAT,
                                          G_PARAM_READABLE));

        g_object_class_install_property (gobjectclass, ARG_AUDIO_FORMAT, 
                                         g_param_spec_object 
                                         ("audioformat", "AudioFormat",
                                          "The video format of this project",
                                          GDV_TYPE_AUDIOFORMAT,
                                          G_PARAM_READABLE));
        
        gobjectclass->dispose = (gpointer) gdv_projectformat_dispose;
}

/* GObject init */
void                            gdv_projectformat_init (GdvProjectFormat *this)
{
        GDV_DEBUG_EXT ("Initializing fresh GdvProjectFormat object", NULL);

        this->_VFormat = gdv_videoformat_new_default ();
        this->_AFormat = gdv_audioformat_new_default ();
}

/* Create a new instance */
GdvProjectFormat*               gdv_projectformat_new (void)
{
        GdvProjectFormat *this = g_object_new (GDV_TYPE_PROJECTFORMAT, NULL);
        return this;
}

/* Alternative initializer */
GdvProjectFormat*               gdv_projectformat_new_from_formats (GdvVideoFormat *vformat,
                                                                    GdvAudioFormat *aformat)
{
        GdvProjectFormat *this = g_object_new (GDV_TYPE_PROJECTFORMAT, NULL);

        g_object_unref (this->_VFormat);
        g_object_unref (this->_AFormat);
        
        this->_VFormat = vformat;
        this->_AFormat = aformat;
        
        g_object_ref (vformat);
        g_object_ref (aformat);

        return this;
}

/* GObject dispose */
void                            gdv_projectformat_dispose (GdvProjectFormat *this)
{
        GDV_DEBUG_EXT ("Disposing...", NULL);
        
        if (this->_VFormat != NULL) {
                g_object_unref (this->_VFormat);
                this->_VFormat = NULL;
        }

        if (this->_AFormat != NULL) {
                g_object_unref (this->_AFormat);
                this->_AFormat = NULL;
        }

        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject get property */
void                            gdv_projectformat_get_property (GdvProjectFormat *this, guint propid,
                                                                GValue *value, GParamSpec *pspec)
{
        g_return_if_fail (GDV_IS_PROJECTFORMAT (this));
        
        switch (propid) {

	        case ARG_VIDEO_FORMAT: {
                        g_assert (this->_VFormat != NULL);
                        g_value_set_object (value, this->_VFormat);
                } break;

                case ARG_AUDIO_FORMAT: {
                        g_assert (this->_AFormat != NULL);
                        g_value_set_object (value, this->_AFormat);
                } break;
                        
                default:
                G_OBJECT_WARN_INVALID_PROPERTY_ID (G_OBJECT (this), propid, pspec);
                break;
        }
}
