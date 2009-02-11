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

/* Handy functions to automatic mediaitem generation. This is mostly intended
 * to work with files (decodebin) sources. Special item (stock) will need their
 * own mediaitem implementations.
 */

#include "GdvItemGenerator.h"
#include "GdvItemGeneratorPrivate.h"

/* Generatate a mediaitem by analyzing the source */
GdvMediaItem*                   gdv_itemgenerator_from_source (GdvSource *source)
{
        // FIXME: Implement
        g_assert_not_reached ();
}

/* Generate a mediaitem by analyzing an inspector. This is more robust */
GdvMediaItem*                   gdv_itemgenerator_from_inspector (GdvProjectFormat *format, GdvInspector *inspector)
{
        // FIXME: Debug messages
        // FIXME: Should return/wrap the error
        g_assert (GDV_IS_INSPECTOR (inspector));
        g_assert (GDV_IS_PROJECTFORMAT (format));
        
        gboolean hasaudio, hasvideo;
        gchar *filename, *mime;
        GdvTime len;

        // Get basic params
        g_object_get (G_OBJECT (inspector), "hasvideo", &hasvideo, "hasaudio", &hasaudio,
                      "url", &filename, "mime", &mime, "length", &len, NULL);

        // Get formats, if needed
        GdvVideoFormat *vformat = NULL;
        GdvAudioFormat *aformat = NULL;

        if (hasvideo) {
                g_object_get (G_OBJECT (inspector), "videoformat", &vformat, NULL);
                g_assert (vformat != NULL);
        }

        if (hasaudio) {
                g_object_get (G_OBJECT (inspector), "audioformat", &aformat, NULL);
                g_assert (aformat != NULL);
        }
        
        GdvMediaItem *item = NULL;

        // Now let's start our fun...

        // PhotoItem
        if (hasvideo && (g_strrstr (mime, "image/jpeg") != NULL ||
                         g_strrstr (mime, "image/png") != NULL)) {
                GDV_DEBUG_EXT ("Creating a photo item", NULL);
                item = (GdvMediaItem *) gdv_photoitem_new (format, filename);
                goto done;
        }

        // AVItem
        if (hasvideo) {
                GDV_DEBUG_EXT ("Creating an AV item", NULL);
                item = (GdvMediaItem *) gdv_avitem_new ( GDV_SOURCE (gdv_decodebin_new (filename, vformat, aformat,
                                                                                        len)));

                goto done;
        }

        // Sound item
        if (hasaudio) {
                GDV_DEBUG_EXT ("Creating an audio item", NULL);
                item = (GdvMediaItem *) gdv_audioitem_new (GDV_SOURCE(gdv_decodebin_new (filename, NULL, aformat,
                                                                                         len)));
                goto done;
        }

        return NULL;
        g_assert_not_reached ();

        done:
        g_object_set (G_OBJECT (item), "name", get_tagged_name (inspector), NULL);

        // FIXME: Free the mime, filename, etc. Bullshit code, really

        // Unref
        if (vformat != NULL)
                g_object_unref (vformat);
        if (aformat != NULL)
                g_object_unref (aformat);
        
        return item;
}

/* Check what kind of item an item is. This is mostly for the bindings, to 
 * map MediaItem to a more specific object type */
GdvItemType                     gdv_itemgenerator_get_item_type (GdvMediaItem *item)
{
        if (GDV_IS_MEDIAITEM (item) == FALSE)
                return GDV_ITEM_NONE;
        
        if (GDV_IS_AVITEM (item) == TRUE)
                return GDV_ITEM_AV;
        
        if (GDV_IS_AUDIOITEM (item) == TRUE)
                return GDV_ITEM_AUDIO;
        
        if (GDV_IS_PHOTOITEM (item) == TRUE)
                return GDV_ITEM_PHOTO;
        
        return GDV_ITEM_NONE;
}

/* Check what kind of source the given on is. This is, again, for the bindings only.
 * I put it here for conveinence */
GdvSourceType                   gdv_itemgenerator_get_source_type (GdvSource *source)
{
        if (source == NULL)
                GDV_WARNING_EXT ("Trying to identify NULL source", NULL);
        
        if (GDV_IS_SOURCE (source) == FALSE)
                return GDV_SOURCE_NONE;
        
        if (GDV_IS_DECODEBIN (source) == TRUE)
                return GDV_SOURCE_DECODEBIN;
        
        if (GDV_IS_SHEEPSOURCE (source) == TRUE)
                return GDV_SOURCE_SHEEPSOURCE;
        
        return GDV_SOURCE_NONE;
}

/* Strip the path part and the extension from the diven string */
static gchar*                   get_stripped_name (const gchar *filename)
{
        gchar *str = g_path_get_basename (filename);
        gchar *dor = g_strrstr (str, ".");
        if (dor == NULL)
                return str;
        
        *dor = (gchar) NULL;
        gchar *out = g_strdup (str);
        g_free (str);
        return out;
}

/* Try to generate a tag-aware name */
static gchar*                   get_tagged_name (GdvInspector *inspector)
{
        g_return_val_if_fail (GDV_IS_INSPECTOR (inspector), NULL);
        gchar *currentname = NULL;

        // Artist
        gchar *artist = NULL;
        gdv_inspector_get_string_tag (inspector, "artist", &artist);
        if (artist != NULL) {
                append_tag (&currentname, artist);
                g_free (artist);
        }

        // Album
        gchar *album = NULL;
        gdv_inspector_get_string_tag (inspector, "album", &album);
        if (album != NULL) {
                append_tag (&currentname, album);
                g_free (album);
        }

        // Title
        gchar *title = NULL;
        gdv_inspector_get_string_tag (inspector, "title", &title);
        if (title != NULL) {
                append_tag (&currentname, title);
                g_free (title);
        }

        // No tag was availible. Fallback to a filename-based name
        if (currentname == NULL) {
                gchar *filename;
                g_object_get (G_OBJECT (inspector), "url", &filename, NULL);
                currentname = get_stripped_name (filename);
                g_free (filename);
        }
        
        return currentname;
}

/* Append a string to another string, tag-style */
static void                     append_tag (gchar **str, gchar *append)
{
        g_return_if_fail (append != NULL);
        
        gchar *newstr = NULL;

        if (*str == NULL)
                newstr = g_strdup_printf ("%s", append);
        else
                newstr = g_strdup_printf ("%s - %s", *str, append);

        if (str != NULL)
                g_free (*str);

        *str = newstr;
}
