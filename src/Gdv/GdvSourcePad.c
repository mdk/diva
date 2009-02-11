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

#include "GdvSourcePad.h"
#include "GdvSourcePadPrivate.h"

/* Creates a asrc sourcepad without a target */
GdvSourcePad*                   gdv_sourcepad_new_no_target (const gchar *name)
                                                      
{
        g_return_val_if_fail (name != NULL, NULL);

        GDV_DEBUG_EXT ("Creating new SourcePad '%s'", name);
        GstPad *pad = NULL;

        // Create
        pad = gst_ghost_pad_new_no_target (name, GST_PAD_SRC);
        g_return_val_if_fail (pad != NULL, NULL);
        
        GdvSourcePadPrivate *private = g_new (GdvSourcePadPrivate, 1);
        private->OrigLinkFunc = pad->linkfunc;
        private->OrigQueryFunc = pad->queryfunc;
        private->SourceLength = GDV_ZEROTIME;
        private->Caps = NULL;
        
        // Attach priv to pad
        gst_pad_set_element_private (pad, private);
        
        // Set custom link function
        gst_pad_set_link_function (pad, (GstPadLinkFunction) link_func);
        gst_pad_set_query_function (pad, (GstPadQueryFunction) query_func);
        gst_pad_set_getcaps_function (pad, (GstPadGetCapsFunction) get_caps_func);

        return (GdvSourcePad *) pad;
}

/* Set the caps on the pad to match a given videoformat. In other words -- 
 * make it a vsrc pad */
gboolean                        gdv_sourcepad_set_videoformat (GdvSourcePad *this, 
                                                               GdvVideoFormat *videoformat)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (GDV_IS_VIDEOFORMAT (videoformat), FALSE);
        GDV_DEBUG_EXT ("Setting VideoFormat/GstCaps on SourcePad '%s'", GST_PAD_NAME (this));

        // Set caps
        GstCaps *caps = NULL;
        g_object_get (G_OBJECT (videoformat), "caps", &caps, NULL);
        g_assert (caps != NULL);
        
        GDV_DEBUG_EXT ("Setting %s caps on %s", gst_caps_to_string (caps), GST_PAD_NAME (this));

        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);
        
        if (private->Caps != NULL) 
                gst_caps_unref (private->Caps);
        
        private->Caps = caps;
        gst_caps_ref (caps);

        return TRUE;
}

/* Set the caps on the pad to match a given audioformat. In other words -- 
 * make it a asrc pad */
gboolean                        gdv_sourcepad_set_audioformat (GdvSourcePad *this,
                                                               GdvAudioFormat *audioformat)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (GDV_IS_AUDIOFORMAT (audioformat), FALSE);
        GDV_DEBUG_EXT ("Setting AudioFormat/GstCaps on SourcePad '%s'", GST_PAD_NAME (this));

        // Set caps
        GstCaps *caps = NULL;
        g_object_get (G_OBJECT (audioformat), "caps", &caps, NULL);
        g_assert (caps != NULL);

        GDV_DEBUG_EXT ("Setting %s caps on %s", gst_caps_to_string (caps), GST_PAD_NAME (this));

        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);
        
        if (private->Caps != NULL) 
                gst_caps_unref (private->Caps);

        private->Caps = caps;
        gst_caps_ref (caps);
        
        return TRUE;
}

/* Set the target of the SourcePad */
gboolean                        gdv_sourcepad_set_target (GdvSourcePad *this, GstPad *target)
{
        g_return_val_if_fail (this != NULL, FALSE);
        g_return_val_if_fail (target != NULL, FALSE);
        
        GDV_DEBUG_EXT ("Setting SourcePad '%s' target to real pad '%s'", GST_PAD_NAME (this),
                       GST_PAD_NAME (target));

        if (! (gst_ghost_pad_set_target (GST_GHOST_PAD (this), target)))
                GDV_WARNING_EXT ("Setting target failed", NULL);
        
        // Execute postponed linking?
        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);
        
        if (GST_PAD_IS_LINKED (this)) {
                GDV_DEBUG_EXT ("Executing postponed linking to pad '%s'", GST_PAD_NAME (GST_PAD_PEER (this)));
                private->OrigLinkFunc ((GstPad *) this, GST_PAD_PEER (this));
        }
        
        return TRUE;
}

/* Set the length of the source this pad controls. We use this value to answer the duration queries */
gboolean                        gdv_sourcepad_set_source_length (GdvSourcePad *this, GdvTime length)
{
        g_return_val_if_fail (this != NULL, FALSE);
        
        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);
        
        private->SourceLength = length;
        return TRUE;
}
            
/* Check if the given SourcePad has a target already */
gboolean                         gdv_sourcepad_has_target (GdvSourcePad *this)
{
        GstPad *pad = gst_ghost_pad_get_target ((GstGhostPad *) this);

        if (pad == NULL)
                return FALSE;
        else {
                gst_object_unref ((GstPad *) pad);
                return TRUE;
        }
}

/* A custom linking function for the pad. We retun "OK" of we don't have the target, 
 * and than we execute the postponed link when the target is actually set */
static GstPadLinkReturn         link_func (GdvSourcePad *this, GstPad *peer)
{
        g_return_val_if_fail (this != NULL, GST_PAD_LINK_REFUSED);
        g_return_val_if_fail (peer != NULL, GST_PAD_LINK_REFUSED);
        
        GDV_DEBUG_EXT ("Linking function for SourcePad '%s'", GST_PAD_NAME (this));
        
        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);

        if (gdv_sourcepad_has_target (this) == TRUE)
                return private->OrigLinkFunc ((GstPad *) this, peer);
        else 
                return GST_PAD_LINK_OK;
        
        // FIXME: Actually we should check caps her
        // FIXME: Really?e
}

/* A custom query func. We use the source element to return the length */
static gboolean                 query_func (GdvSourcePad *this, GstQuery *query)
{
        g_return_val_if_fail (this != NULL, FALSE);
        
        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);
        
        GDV_DEBUG_EXT ("GdvSourcePad '%s' gets query '%d'", GST_PAD_NAME (this),
                       GST_QUERY_TYPE (query));
        
        gboolean answered = FALSE;
        
        if (GST_QUERY_TYPE (query) == GST_QUERY_DURATION && private->SourceLength != GDV_ZEROTIME) {
                // We can try answering this query
                GstFormat format;
                gst_query_parse_duration (query, &format, NULL);
                
                if (format == GST_FORMAT_TIME) {
                        GDV_DEBUG_EXT ("Answering duration query with time %s", 
                                       gdv_time_to_string (private->SourceLength));
                        gst_query_make_writable (query);
                        gst_query_set_duration (query, format, private->SourceLength);
                        answered = TRUE;
                } else {
                        GDV_WARNING_EXT ("Can't answer the query, requesting non-time format", NULL);
                        answered = TRUE;
                }
        }
        
        // Chain to the original function if we couldn't answer the query
        if (answered == FALSE)
                return private->OrigQueryFunc (GST_PAD (this), query);
        else 
                return TRUE;
}

gboolean                        gdv_sourcepad_untarget (GdvSourcePad *this)
{
        g_return_val_if_fail (this != NULL, FALSE);
       
        GDV_DEBUG_EXT ("Setting SourcePad '%s' target NULL", GST_PAD_NAME (this));

        GstPad *peer = NULL; 
        if (GST_PAD_IS_LINKED (this)) {
                peer = GST_PAD_PEER (this);
                gst_pad_unlink ((GstPad *) this, peer);
        }

        if (! (gst_ghost_pad_set_target (GST_GHOST_PAD (this), NULL)))
                GDV_WARNING_EXT ("Unsetting target failed", NULL);
       
        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_assert (private != NULL);
        GstPad *pad = (GstPad *) this;

        // Set custom link functions, just to be sure
        gst_pad_set_link_function (pad, (GstPadLinkFunction) link_func);
        gst_pad_set_query_function (pad, (GstPadQueryFunction) query_func);
        gst_pad_set_getcaps_function (pad, (GstPadGetCapsFunction) get_caps_func);

        if (peer != NULL)
                return gst_pad_link ((GstPad *) this, peer);
        else
                return TRUE;
}

/* Loose the private part of the pad. That usually happens during disposing, ect */
void                            gdv_sourcepad_loose (GdvSourcePad *this)
{
        g_return_if_fail (this != NULL);
        
        GDV_DEBUG_EXT ("Loosing the pad '%s'", GST_PAD_NAME (this));
        
        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_return_if_fail (private != NULL);

        if (private->Caps != NULL)
                gst_caps_unref (private->Caps);
        
        g_free (private);
        gst_pad_set_element_private (GST_PAD (this), NULL);
}

/* An overriden getcaps function. We just forward the caps of the target pad */
static GstCaps*                 get_caps_func (GdvSourcePad *this)
{
        g_return_val_if_fail (this != NULL, NULL);

        GstObject *parent = gst_pad_get_parent (this);
        
        GDV_DEBUG_EXT ("Get caps function for '%s:%s'", 
                       GST_OBJECT_NAME (parent), GST_PAD_NAME (this));

        GdvSourcePadPrivate *private = (GdvSourcePadPrivate *) gst_pad_get_element_private ((GstPad *) this);
        g_return_val_if_fail (private != NULL, NULL);
        
        gst_object_unref (parent);

        if (private->Caps == NULL) {
                GDV_DEBUG_EXT ("Returning empty caps", NULL);
                return gst_caps_new_empty ();
        } else
                return gst_caps_ref (private->Caps);
}
