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

struct                          _TowerItem
{
        gint32 Id;
        gint32 GId;
        gpointer CustomData;
        GdvIdleTowerActionFunc ActionFunc;
        GdvIdleTowerDisposeFunc DisposeFunc;
        GdvIdleTowerFinalizeFunc FinalizeFunc;
} typedef TowerItem;

struct                          _GdvIdleTowerPrivate
{
        GMutex *Mutex;         // Op mutex
        GQueue *ItemQueue;     // The main queue of the tower items
        GQueue *FinalizeQueue; // Queue with the items to be finalized
        gint32 IdlesSent;      // The amount of idles sent
        gint32 IdlesReceived;  // The amount of idles received
        gint32 CurrentId;      // The current assignable id
};

#define TOWER_LOCK(obj)         (g_mutex_lock(obj->Private->Mutex))

#define TOWER_UNLOCK(obj)       (g_mutex_unlock(obj->Private->Mutex))

static void                     gdv_idletower_class_init (GdvIdleTowerClass *klass);

static void                     gdv_idletower_init (GdvIdleTower *this);

static void                     gdv_idletower_dispose (GdvIdleTower *this);

static void                     gdv_idletower_finalize (GdvIdleTower *this);

static gboolean                 private_init (GdvIdleTowerPrivate *private);

static void                     private_dispose (GdvIdleTowerPrivate *private);

static gboolean                 idle_func (GdvIdleTower *this);

static void                     dispose_item (GdvIdleTower *tower, TowerItem *item, gboolean lock);

static int                      queue_find_by_gid (const TowerItem *item, const gint32 *gid);

