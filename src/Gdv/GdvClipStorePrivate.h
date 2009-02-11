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

struct                          _ClipStoreData
{
        GdvClip *Clip;
        guint ChangedSignal;
        
} typedef ClipStoreData;

struct                          _GdvClipStorePrivate
{
        gint32 Count;    // Number of stored clips
        GList *ClipList; // Data struct
        GMutex *Mutex;   // Our mutex to sync access to the list
};

#define CLIPSTORE_LOCK(obj)     (g_mutex_lock (this->Private->Mutex))

#define CLIPSTORE_UNLOCK(obj)   (g_mutex_unlock (this->Private->Mutex))

static void                     gdv_clipstore_class_init (GdvClipStoreClass *klass);

static void                     gdv_clipstore_init (GdvClipStore *this);

static void                     gdv_clipstore_dispose (GdvClipStore *this);

static void                     gdv_clipstore_finalize (GdvClipStore *this);

static gboolean                 private_init (GdvClipStorePrivate *private);

static void                     private_dispose (GdvClipStore *this, 
                                                 GdvClipStorePrivate *private);

static gint                     clip_compare_func (const ClipStoreData *dataa, const ClipStoreData *datab);

static gint                     clip_find_func (const ClipStoreData *data, const GdvClip *clip);

static void                     on_clip_changed (gpointer a, GdvClipStore *this);

