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

// FIXME: Some functions here return GdvClip with a ref, some don't. Standarize
// that! The "with ref" was introduced to keep managed happy. 

#include "GdvClipStore.h"
#include "GdvClipStorePrivate.h"

static GstElementClass*         ParentClass = NULL;

/* GObject type */
GType                           gdv_clipstore_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvClipStoreClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) gdv_clipstore_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvClipStore),
                        8, 
                        (GInstanceInitFunc) gdv_clipstore_init,
                };
                objecttype = g_type_register_static (G_TYPE_OBJECT, "GdvClipStore", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_clipstore_class_init (GdvClipStoreClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (G_TYPE_OBJECT);
        
        gobjectclass->dispose = (gpointer) gdv_clipstore_dispose;
        gobjectclass->finalize = (gpointer) gdv_clipstore_finalize;
}

/* GObject dispose */
static void                     gdv_clipstore_dispose (GdvClipStore *this)
{
        g_return_if_fail (GDV_IS_CLIPSTORE (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_clipstore_finalize (GdvClipStore *this)
{
        g_return_if_fail (GDV_IS_CLIPSTORE (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject init */
static void                     gdv_clipstore_init (GdvClipStore *this)
{
        g_return_if_fail (GDV_IS_CLIPSTORE (this));
        this->Private = NULL;

        GdvClipStorePrivate *priv = g_new0 (GdvClipStorePrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Private init */
static gboolean                 private_init (GdvClipStorePrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);
        
        // Preset count to 0
        private->Count = 0;
        
        // Reset the list
        private->ClipList = NULL;
        
        // Mutex
        private->Mutex = g_mutex_new ();
        g_return_val_if_fail (private->Mutex != NULL, FALSE);
        
        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvClipStore *this, GdvClipStorePrivate *private)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (GDV_IS_CLIPSTORE (this));
        
        // Lock if we still have mutex
        if (private->Mutex != NULL)
                CLIPSTORE_LOCK(this);
        
        // First iterate over all elements and unref them
        if (private->ClipList != NULL) {
                GList *iter;
                for (iter = private->ClipList; iter; iter = g_list_next (iter)) {
                        ClipStoreData *data = iter->data;
                        if (data->ChangedSignal != 0)
                                g_signal_handler_disconnect (data->Clip, data->ChangedSignal);
                        if (data->Clip != NULL)
                                gst_object_unref (data->Clip);
                        g_free (data);
                }
        }
                
        // Let's remove all instances from our list
        g_list_free (private->ClipList);
        private->ClipList = NULL;
        
        // Reset count
        private->Count = 0;
        
        // Unlock if we still have mutex and free it
        if (private->Mutex != NULL) {
                CLIPSTORE_UNLOCK(this);
                g_mutex_free (private->Mutex);
        }
}

/* Create a new instance */
GdvClipStore*                   gdv_clipstore_new ()
{
        GDV_DEBUG_EXT ("Creating a new GdvClipStore", NULL);
        
        GdvClipStore *this = g_object_new (GDV_TYPE_CLIPSTORE, NULL);
        if (this == NULL)
                return NULL;
        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }

        return this;
}

/* Check if a given timespan is currently occupied by something. We iterate over
 * elements using the fact that they're sorted */
gboolean                        gdv_clipstore_is_span_empty (GdvClipStore *this,
                                                             GdvTimeSpan *span)
{
        return gdv_clipstore_is_span_empty_excluding (this, span, NULL);
}

/* Check if a given timespan is currently occupied by something. Exclude the specified
 * clip assuming it's the one we 'own'. If excluded == NULL, do not exclude anything from check.
 * We iterate over elements using the fact that they're sorted */
gboolean                        gdv_clipstore_is_span_empty_excluding (GdvClipStore *this,
                                                                       GdvTimeSpan *span,
                                                                       GdvClip *excluded)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), FALSE);
        g_return_val_if_fail (span != NULL, FALSE);
        
        GDV_DEBUG_EXT ("Checking if %s is empty on the clip store", 
                       gdv_timespan_to_string (span));
        
        if (gdv_timespan_is_empty (span))
                return FALSE;
        
        CLIPSTORE_LOCK (this);
        
        if (this->Private->Count == 0)
                goto empty;
        
        GList *iter;
        for (iter = this->Private->ClipList; iter; iter = g_list_next (iter)) {
                ClipStoreData *data = (ClipStoreData *) iter->data;
                
                // Sanity check
                GdvClip *clip = GDV_CLIP (data->Clip);
                if (clip == NULL) 
                        goto error;

                GdvTimeSpan *clipspan = NULL;
                
                if (excluded != NULL && clip == excluded)
                        goto next;
                
                // Get the timespan

                g_object_get (G_OBJECT (clip), "timelinespan", &clipspan, NULL);
                if (clipspan == NULL)
                        goto error;
                                        
                if (clipspan->End < span->Start)
                        goto next;
                
                if (clipspan->Start > span->End) {
                        // We can assume area is empty
                        gdv_timespan_free (clipspan);
                        goto empty;
                }
                
                // Perform check
                if (gdv_timespan_intersects_with (clipspan, span)) {
                        gdv_timespan_free (clipspan);
                        goto notempty;
                }
                
                next:
                if (clipspan != NULL)
                        gdv_timespan_free (clipspan);
        }
        
        empty:
        CLIPSTORE_UNLOCK (this);
        return TRUE;
        
        notempty:
        CLIPSTORE_UNLOCK (this);
        return FALSE;
                
        error:
        GDV_WARNING_EXT ("Error while processing ClipStore", NULL);
        CLIPSTORE_UNLOCK (this);
        return FALSE;
}


/* Add new clip/timespan to the clip store */
gboolean                        gdv_clipstore_add_clip (GdvClipStore *this, GdvClip *clip)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), FALSE);
        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);
        
        GdvTimeSpan *clipspan = NULL;
        
        g_object_get (G_OBJECT (clip), "timelinespan", &clipspan, NULL);
        if (clipspan == NULL)
                g_return_val_if_reached (FALSE);
        
        // If the clip's timespan is empty it cannot be added
        if (gdv_timespan_is_empty (clipspan)) {
                gdv_timespan_free (clipspan);
                g_return_val_if_reached (FALSE);
        }
        
        GDV_DEBUG_EXT ("Adding clip %s at %s to the ClipStore", GST_OBJECT_NAME (clip),
                       gdv_timespan_to_string (clipspan));
        
        // Check if we're empty
        if (! gdv_clipstore_is_span_empty (this, clipspan)) {
                GDV_DEBUG_EXT ("Area not empty, returning FALSE", NULL);
                gdv_timespan_free (clipspan);
                return FALSE;
        } else
                gdv_timespan_free (clipspan);
        
        // FIXME: There is a possible tiny race condition here. We should perform
        // the operation of checking and adding in ONE lock holding 
        
        CLIPSTORE_LOCK (this);

        ClipStoreData *newdata = g_new (ClipStoreData, 1);
        newdata->Clip = clip;
        gst_object_ref (clip);

        // Connect the changed signal
        newdata->ChangedSignal = g_signal_connect (G_OBJECT (clip), "changed",
                                                   (GCallback) on_clip_changed,
                                                   this);
        
        this->Private->ClipList =
                g_list_insert_sorted (this->Private->ClipList, newdata, (GCompareFunc) clip_compare_func);

        this->Private->Count++;
        CLIPSTORE_UNLOCK (this);
        
        GDV_DEBUG_EXT ("Added %s to the ClipStore", GST_OBJECT_NAME (clip));
        return TRUE;
}

gboolean                        gdv_clipstore_remove_clip (GdvClipStore *this, 
                                                           GdvClip *clip)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), FALSE);
        g_return_val_if_fail (GDV_IS_CLIP (clip), FALSE);

        GDV_DEBUG_EXT ("Removing clip %s from the ClipStore", GST_OBJECT_NAME (clip));

        CLIPSTORE_LOCK (this);

        // Find it using a custom func
        GList *found = g_list_find_custom (this->Private->ClipList, clip, (GCompareFunc) clip_find_func);
        if (found == NULL)
                goto error;
        ClipStoreData *data = (ClipStoreData *) found->data;
        
        this->Private->ClipList =
                g_list_remove (this->Private->ClipList, data);
        
        // Disconnect the signal
        if (data->ChangedSignal != 0)
                g_signal_handler_disconnect (data->Clip, data->ChangedSignal);

        // Unref the clip
        gst_object_unref (data->Clip);
        this->Private->Count--;
        
        g_free (data);
        
        CLIPSTORE_UNLOCK (this);
        return TRUE;
        
 error:
        CLIPSTORE_UNLOCK (this);
        g_return_val_if_reached (FALSE);
}

/* A comparison function. Comparison is based on the timespan of the
 * clip. We assume that clips DO NOT intersect. */
static gint                     clip_compare_func (const ClipStoreData *dataa, const ClipStoreData *datab)
{
        g_return_val_if_fail (dataa != NULL && datab != NULL, 0);

        GdvClip *a = dataa->Clip;
        GdvClip *b = datab->Clip;
                
        g_return_val_if_fail (GDV_IS_CLIP (a) && GDV_IS_CLIP (b), 0);
                        
        GdvTimeSpan *aspan = NULL;
        GdvTimeSpan *bspan = NULL;
        gint res = 0;
        
        g_object_get (G_OBJECT (a), "timelinespan", &aspan, NULL);
        g_object_get (G_OBJECT (b), "timelinespan", &bspan, NULL);
        
        // Error #1
        if (aspan == NULL || bspan == NULL) {
                GDV_WARNING_EXT ("Spans broken", NULL);
                goto finish;
        }
        
        // Error #2
        if (gdv_timespan_intersects_with (aspan, bspan)) {
                GDV_WARNING_EXT ("Spans can't intersect!", NULL);
                goto finish;
        }
                
        if (gdv_timespan_is_equal (aspan, bspan))
                res = 0;
        else {
                if (aspan->Start < bspan->Start)
                        res = -1;
                else
                        res = 1;
        }
        
        finish:
        if (aspan != NULL) 
                gdv_timespan_free (aspan);
        if (bspan != NULL) 
                gdv_timespan_free (bspan);
        
        return res;
}

/* Get a clip that plays at a given time. Returns NULL if there is no clip at time
 * The clip is returned WITHOUT a ref on it */
GdvClip*                        gdv_clipstore_get_clip_at_time (GdvClipStore *this, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), NULL);

        GDV_DEBUG_EXT ("Looking for clip at time %s", gdv_time_to_string (time));
        
        GdvClip *result = NULL;
        CLIPSTORE_LOCK (this);
        
        if (this->Private->ClipList == NULL) {
                GDV_DEBUG_EXT ("No clips in ClipStore yet", NULL);
                goto done;
        }
        
        GList *iter;
        for (iter = this->Private->ClipList; iter; iter = g_list_next (iter)) {
                ClipStoreData *data = (ClipStoreData *) iter->data;
                
                // Sanity check
                GdvClip *clip = GDV_CLIP (data->Clip);
                if (clip == NULL) 
                        goto error;
                
                // Get the timespan
                GdvTimeSpan *timespan = NULL;
                g_object_get (G_OBJECT (clip), "timelinespan", &timespan, NULL);
                if (timespan == NULL)
                        goto error;
                
                if (timespan->End < time)
                        goto next;
                                
                if (timespan->Start > time) {
                        // We can assume it was not found (and will not be found)
                        gdv_timespan_free (timespan);
                        goto done;
                }
                
                // Perform check
                if (gdv_timespan_contains (timespan, time))
                        result = clip;
                        
                next:
                gdv_timespan_free (timespan);
                if (result != NULL)
                        goto done;
        }
        
        done:
        CLIPSTORE_UNLOCK (this);
        if (result != NULL)
                g_object_ref (result);
        return result;
        
        error:
        CLIPSTORE_UNLOCK (this);
        g_return_val_if_reached (NULL);
}
        
/* Get the first clip appearing after given time */
GdvClip*                        gdv_clipstore_get_clip_after_time (GdvClipStore *this, 
                                                                   GdvTime time)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), NULL);
        
        GDV_DEBUG_EXT ("Looking for clip after time %s", gdv_time_to_string (time));
        
        GdvClip *result = NULL;
        CLIPSTORE_LOCK (this);
        
        if (this->Private->ClipList == NULL) {
                GDV_DEBUG_EXT ("No clips in ClipStore yet", NULL);
                goto done;
        }
        
        GList *iter;
        for (iter = this->Private->ClipList; iter; iter = g_list_next (iter)) {
                ClipStoreData *data = (ClipStoreData *) iter->data;
                
                // Sanity check
                GdvClip *clip = GDV_CLIP (data->Clip);
                if (clip == NULL) 
                        goto error;
                
                // Get the timespan
                GdvTimeSpan *timespan = NULL;
                g_object_get (G_OBJECT (clip), "timelinespan", &timespan, NULL);
                if (timespan == NULL)
                        goto error;
                
                if (timespan->End < time)
                        goto next;
                                
                if (timespan->Start > time) 
                        result = clip;
                
                next:
                gdv_timespan_free (timespan);
                if (result != NULL)
                        goto done;
        }
        
        done:
        CLIPSTORE_UNLOCK (this);
        if (result != NULL)
                g_object_ref (result);
        return result;
        
        error:
        CLIPSTORE_UNLOCK (this);
        g_return_val_if_reached (NULL);
}

/* Get the first clip appearing before given time. WARNING: Can report "clip at time" actually.
 *  This shoud be fixed */
GdvClip*                        gdv_clipstore_get_clip_before_time (GdvClipStore *this, 
                                                                    GdvTime time)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), NULL);
        
        GDV_DEBUG_EXT ("Looking for clip before time %s", gdv_time_to_string (time));
        
        GdvClip *result = NULL;
        CLIPSTORE_LOCK (this);
        
        if (this->Private->ClipList == NULL) {
                GDV_DEBUG_EXT ("No clips in ClipStore yet", NULL);
                goto done;
        }
        
        GList *iter;
        for (iter = this->Private->ClipList; iter; iter = g_list_next (iter)) {
                ClipStoreData *data = (ClipStoreData *) iter->data;
                
                // Sanity check
                GdvClip *clip = GDV_CLIP (data->Clip);
                if (clip == NULL) 
                        goto error;
                
                // Get the timespan
                GdvTimeSpan *timespan = NULL;
                g_object_get (G_OBJECT (clip), "timelinespan", &timespan, NULL);
                if (timespan == NULL)
                        goto error;
                
                if (timespan->End < time)
                        result = clip;
                                
                if (timespan->Start > time) 
                        goto done;
                
                gdv_timespan_free (timespan);
        }
        
        done:
        CLIPSTORE_UNLOCK (this);
        if (result != NULL)
                g_object_ref (result);
        return result;
        
        error:
        CLIPSTORE_UNLOCK (this);
        g_return_val_if_reached (NULL);
}

/* Try to automatically suggest the best position to place the span in the current store.
 * We try to see what's currently occupying the span, and we try to "stick" to it in front
 * or in the back. Exclude the specified clip from check, assuming we 'own' it.  */
gboolean                        gdv_clipstore_suggest_position_excluding (GdvClipStore *this,
                                                                          GdvTimeSpan *span, GdvTimeSpan *out,
                                                                          GdvTime cursor, GdvClip *exclude)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), FALSE);
        
        GDV_DEBUG_EXT ("Suggesting position for %s on the clipstore", gdv_timespan_to_string (span));

        // If it's empty we pass it simply
        if (gdv_clipstore_is_span_empty_excluding (this, span, exclude) == TRUE) {
                out->Start = span->Start;
                out->End = span->End;
                return TRUE;
        }

        // Let's find the clip that's blocking us
        GdvClip *blockingclip = NULL;
        GdvTimeSpan *clipspan;
        GdvTime halftime;
        gboolean ret = FALSE; 
        
        blockingclip = gdv_clipstore_get_clip_at_time (this, span->Start);
        
        if (blockingclip == NULL)
                blockingclip = gdv_clipstore_get_clip_at_time (this, span->End);

        // FIXME: This is borked. We miss some possible suggestions
        if (blockingclip == NULL)
                return FALSE;

        g_object_get (G_OBJECT (blockingclip), "timelinespan", &clipspan, NULL);
        g_assert (clipspan != NULL);
        halftime = (clipspan->Start + clipspan->End) / 2;

        if (cursor < halftime) {
                if (gdv_timespan_duration (span) > clipspan->Start)
                        goto done;
                
                GdvTimeSpan *testspan = gdv_timespan_new (clipspan->Start - gdv_timespan_duration (span),
                                                          clipspan->Start);
                if (gdv_clipstore_is_span_empty_excluding (this, testspan, exclude)) {
                        ret = TRUE;
                        out->Start = testspan->Start;
                        out->End = testspan->End;
                }
                
                gdv_timespan_free (testspan);
                goto done;
        } else {
                GdvTimeSpan *testspan = gdv_timespan_new (clipspan->End,
                                                          clipspan->End + gdv_timespan_duration (span));
                if (gdv_clipstore_is_span_empty_excluding (this, testspan, exclude)) {
                        out->Start = testspan->Start;
                        out->End = testspan->End;
                        ret = TRUE;
                }
                
                gdv_timespan_free (testspan);
                goto done;
        }
        
 done:
        
        if (clipspan != NULL)
                gdv_timespan_free (clipspan);
        
        return ret;
}

/* Without exclusion helper */
gboolean                        gdv_clipstore_suggest_position (GdvClipStore *this,
                                                                GdvTimeSpan *span, GdvTimeSpan *out,
                                                                GdvTime cursor)
{
        return gdv_clipstore_suggest_position_excluding (this, span, out, cursor, NULL);
}

/* A signal emitted by the clip when it's position/params where changed. We resort our list */
static void                     on_clip_changed (gpointer a, GdvClipStore *this)
{
        g_return_if_fail (GDV_IS_CLIPSTORE (this));

        GDV_DEBUG_EXT ("Clip changed, resorting clips list", NULL);
        this->Private->ClipList = g_list_sort (this->Private->ClipList, (GCompareFunc) clip_compare_func);
}

static gint                     clip_find_func (const ClipStoreData *data, const GdvClip *clip)
{
        g_return_val_if_fail (data != NULL, -1);

        if (data->Clip == clip)
                return 0;
        else
                return -1;
}

/* Get a clip that is before the given clip. The clip is returned WITH a ref */
GdvClip*                        gdv_clipstore_get_clip_before_clip (GdvClipStore *this, 
                                                                    GdvClip *clip)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), NULL);

        GList *found = g_list_find_custom (this->Private->ClipList, clip, (GCompareFunc) clip_find_func);
        g_return_val_if_fail (found != NULL, NULL);

        GList *prev = g_list_previous (found);
        if (prev == NULL)
                return NULL;
        
        ClipStoreData *data = (ClipStoreData *) prev->data;

        g_object_ref (data->Clip);
        return data->Clip;
}

/* Get a clip that is after the given clip. The clip is returned WITH a ref */
GdvClip*                        gdv_clipstore_get_clip_after_clip (GdvClipStore *this, 
                                                                   GdvClip *clip)
{
        g_return_val_if_fail (GDV_IS_CLIPSTORE (this), NULL);

        GList *found = g_list_find_custom (this->Private->ClipList, clip, (GCompareFunc) clip_find_func);
        g_return_val_if_fail (found != NULL, NULL);

        GList *next = g_list_next (found);
        if (next == NULL)
                return NULL;
        
        ClipStoreData *data = (ClipStoreData *) next->data;

        g_object_ref (data->Clip);
        return data->Clip;
}
