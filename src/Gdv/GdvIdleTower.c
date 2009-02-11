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

#include "GdvIdleTower.h"
#include "GdvIdleTowerPrivate.h"

static GObjectClass*            ParentClass = NULL;

/* GObject type */
GType                           gdv_idletower_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvIdleTowerClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) gdv_idletower_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvIdleTower),
                        8, 
                        (GInstanceInitFunc) gdv_idletower_init,
                };
                objecttype = g_type_register_static (G_TYPE_OBJECT, "GdvIdleTower", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_idletower_class_init (GdvIdleTowerClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (G_TYPE_OBJECT);
        
        gobjectclass->dispose = (gpointer) gdv_idletower_dispose;
        gobjectclass->finalize = (gpointer) gdv_idletower_finalize;
}

/* GObject dispose */
static void                     gdv_idletower_dispose (GdvIdleTower *this)
{
        g_return_if_fail (GDV_IS_IDLETOWER (this));
        GDV_DEBUG_EXT ("Disposing", NULL);

        if (this->Private != NULL) {

                if (this->Private->Mutex != NULL && this->Private->ItemQueue != NULL &&
                    this->Private->FinalizeQueue != NULL)
                        gdv_idletower_wipe_and_finalize (this);
                
                private_dispose (this->Private);

        }
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_idletower_finalize (GdvIdleTower *this)
{
        g_return_if_fail (GDV_IS_IDLETOWER (this));
        GDV_DEBUG_EXT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject init */
static void                     gdv_idletower_init (GdvIdleTower *this)
{
        GDV_DEBUG_EXT ("Initializing fresh GdvIdleTower object", NULL);
        
        g_return_if_fail (GDV_IS_IDLETOWER (this));
        this->Private = NULL;

        GdvIdleTowerPrivate *priv = g_new0 (GdvIdleTowerPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
                this->Private = NULL;
        }
}

/* Private init */
static gboolean                 private_init (GdvIdleTowerPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        private->Mutex = g_mutex_new ();
        g_return_val_if_fail (private->Mutex != NULL, FALSE);
        
        private->ItemQueue = g_queue_new ();
        g_return_val_if_fail (private->ItemQueue != NULL, FALSE);

        private->FinalizeQueue = g_queue_new ();
        g_return_val_if_fail (private->FinalizeQueue != NULL, FALSE);

        private->IdlesSent = 0;
        private->IdlesReceived = 0;
        private->CurrentId = 0;
                
        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvIdleTowerPrivate *private)
{
        g_return_if_fail (private != NULL);

        if (private->Mutex != NULL) {
                g_mutex_free (private->Mutex);
                private->Mutex = NULL;
        }

        if (private->ItemQueue != NULL) {
                g_queue_free (private->ItemQueue);
                private->ItemQueue = NULL;
        }

        if (private->FinalizeQueue != NULL) {
                g_queue_free (private->FinalizeQueue);
                private->FinalizeQueue = NULL;
        }

}

/* Create a new instance */
GdvIdleTower*                   gdv_idletower_new (void)
{
        GDV_DEBUG_EXT ("Creating a new GdvIdleTower", NULL);
        
        GdvIdleTower *this = g_object_new (GDV_TYPE_IDLETOWER, NULL);

        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }
        
        return this;
}

gint32                          gdv_idletower_add_item (GdvIdleTower *this, gint32 gid,
                                                        GdvIdleTowerActionFunc action,
                                                        GdvIdleTowerDisposeFunc dispose, GdvIdleTowerFinalizeFunc finalize,
                                                        gpointer data)
{
        g_return_val_if_fail (GDV_IS_IDLETOWER (this), -1);
        g_return_val_if_fail (action != NULL, -1);
        g_return_val_if_fail (gid >= 0, -1);

        // Create new item 
        TowerItem *item = g_new (TowerItem, 1);
        gint32 id;
        
        item->GId = gid;
        item->ActionFunc = action;
        item->DisposeFunc = dispose;
        item->FinalizeFunc = finalize;
        item->CustomData = data;

        TOWER_LOCK (this);
        item->Id = this->Private->CurrentId++;
        id = item->Id;

        GDV_DEBUG_EXT ("Adding a new item to GdvIdleTower with %d:%d", id, gid);

        g_queue_push_tail (this->Private->ItemQueue, item);

        // Schedlue idle
        g_idle_add ((GSourceFunc) idle_func, this);
        this->Private->IdlesSent++;

        TOWER_UNLOCK (this);

        return id;
}

static gboolean                 idle_func (GdvIdleTower *this)
{
        g_return_val_if_fail (GDV_IS_IDLETOWER (this), FALSE);

        GDV_DEBUG_EXT ("Idle callback for IdleTower", NULL);

        TOWER_LOCK (this);
        this->Private->IdlesReceived++;

        // Sanity check
        if (this->Private->IdlesReceived > this->Private->IdlesSent)
                GDV_WARNING_EXT ("Received idle count (%d) exceeds the amount of schedlued idles (%d). "
                                 "There must be a bug somewhere",
                                 this->Private->IdlesSent, this->Private->IdlesReceived);

        // Check if it's empty
        if (g_queue_is_empty (this->Private->ItemQueue)) {
                GDV_DEBUG_EXT ("Empty queue, idle aborting", NULL);
                TOWER_UNLOCK (this);
                return FALSE;
        }

        // Get and extract
        TowerItem *item = g_queue_pop_head (this->Private->ItemQueue);
        TOWER_UNLOCK (this);

        g_return_val_if_fail (item->ActionFunc != NULL, FALSE);

        // Execute
        GDV_DEBUG_EXT ("Executing item idle action", NULL);
        g_debug ("Idle func!");
        item->ActionFunc (item->CustomData);
        g_debug ("Idle func DONE!");

        // Dispose
        dispose_item (this, item, TRUE);
        
        return FALSE;
}

static void                     dispose_item (GdvIdleTower *this, TowerItem *item, gboolean lock)
{
        g_return_if_fail (GDV_IS_IDLETOWER (this));
        g_return_if_fail (item != NULL);

        if (item->DisposeFunc != NULL) {
                if (item->DisposeFunc (item->CustomData) == FALSE) {
                        GDV_DEBUG_EXT ("Adding item to finalize queue", NULL);
                        
                        if (lock)
                                TOWER_LOCK (this);
                        
                        g_queue_push_tail (this->Private->FinalizeQueue, item);
                        
                        if (lock)
                                TOWER_UNLOCK (this);
                } else {
                        GDV_DEBUG_EXT ("Item disposed, freeing", NULL);
                        g_free (item);
                }
        } else {
                GDV_DEBUG_EXT ("Item non-disposable, freeing", NULL);
                g_free (item);
        }
}

int                             gdv_idletower_abort_by_gid (GdvIdleTower *this, gint32 gid)
{
        g_return_val_if_fail (GDV_IS_IDLETOWER (this), 0);

        GDV_DEBUG_EXT ("Aborting all actions with gid == %d", gid);

        TOWER_LOCK (this);
        gint32 aborted = 0;

        // Let's operate on a copied queue
        GQueue *queuecopy = g_queue_copy (this->Private->ItemQueue);

        // Now let's run the whole queue possibly removing items
        // from the main queue
        
        while (g_queue_is_empty (queuecopy) == FALSE) {
                TowerItem *item = g_queue_pop_head (queuecopy);
                g_assert (item != NULL);

                if (item->GId == gid) {
                        aborted++;
                        g_queue_remove (this->Private->ItemQueue, item);
                }

        }

        g_queue_free (queuecopy);
        GDV_DEBUG_EXT ("Aborted %d items/actions (by gid)", aborted);

        TOWER_UNLOCK (this);

        return aborted;
}

void                            gdv_idletower_wipe_and_finalize (GdvIdleTower *this)
{
        g_return_if_fail (GDV_IS_IDLETOWER (this));

        GDV_DEBUG_EXT ("Wiping and finalizing the IdleTower", NULL);

        TOWER_LOCK (this);

        // Wipe
        int k = 0;
        while (g_queue_is_empty (this->Private->ItemQueue) == FALSE) {
                TowerItem *item = g_queue_pop_head (this->Private->ItemQueue);
                dispose_item (this, item, FALSE);
                k++;
        }

        GDV_DEBUG_EXT ("Wiped/disposed %d items", k);

        // Finalize
        int p = 0;
        while (g_queue_is_empty (this->Private->FinalizeQueue) == FALSE) {
                TowerItem *item = g_queue_pop_head (this->Private->FinalizeQueue);
                if (item->FinalizeFunc != NULL)
                        item->FinalizeFunc (item->CustomData);

                g_free (item);
                p++;
        }

        GDV_DEBUG_EXT ("Finalized %d items", p);

        TOWER_UNLOCK (this);
}

gboolean                        gdv_idletower_has_gid (GdvIdleTower *this, gint32 gid)
{
        g_return_val_if_fail (GDV_IS_IDLETOWER (this), FALSE);

        GDV_DEBUG_EXT ("Finding if has %d gid", gid);

        TOWER_LOCK (this);

        gboolean ret;

        if (g_queue_find_custom (this->Private->ItemQueue, &gid, (GCompareFunc) queue_find_by_gid) != NULL)
                ret = TRUE;
        else
                ret = FALSE;

        TOWER_UNLOCK (this);
        return ret;
}

static int                      queue_find_by_gid (const TowerItem *item, const gint32 *gid)
{
        g_return_val_if_fail (item != NULL, -1);
        g_return_val_if_fail (gid != NULL, -1);

        if (item->GId == *gid)
                return 0;
        else 
                return -1;
}
