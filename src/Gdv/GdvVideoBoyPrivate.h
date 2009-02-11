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

enum                            _RndrMode
{
        RNDR_MODE_INVALID,
        RNDR_MODE_EMPTY_MASTER,
        RNDR_MODE_REUSE_FIRST,
        RNDR_MODE_REUSE_LAST,
        RNDR_MODE_COMPOSE_FROM_SCRATCH
        
} typedef RndrMode;

enum                            _BoyMode
{
        BOY_MODE_EMPTY,
        BOY_MODE_COMP
        
} typedef BoyMode;

struct                          _ComposeData
{
        GstBuffer *Buffer;              // Original buffer. If any
        GdvFrameDimensions *Dimensions; // Pixel dimensions
        GdvFraction *PixelAspect;       // Pixel aspect ratio
        gdouble Opacity;                // meta - opacity
        gint32 Layer;                   // meta - layer
        gpointer PixelData;             // Raw pointer to data
        guint32 FourCC;                 // The fourcc
        
} typedef ComposeData;

struct                          _GdvVideoBoyPrivate
{
        GstPad *Pad;                          // Output pad. For mem allocations
        GMutex *Mutex;                        // Op mutex
        BoyMode Mode;                         // The current mode we're in
        GList *DataList;                      // List of ComposeData
        gint32 ValidBuffers;                  // Numbers of accumulated (added) valid buffers
        gboolean UsePadAlloc;                 // Should we use the native pad alloc func

        GstCaps *MasterCaps;                  // The master caps we're rendering
        GdvFrameDimensions *MasterDimensions; // The master dimensions
        GdvFraction *MasterPixelAspect;       // The master pixel aspect
        gint32 MasterSize;                    // The master buffer size
        guint32 MasterCC;                     // The master fourcc we're using
        
};

#define BOY_LOCK(obj)           (g_mutex_lock(obj->Private->Mutex))

#define BOY_UNLOCK(obj)         (g_mutex_unlock(obj->Private->Mutex))

static void                     gdv_videoboy_class_init (GdvVideoBoyClass *klass);

static void                     gdv_videoboy_init (GdvVideoBoy *this);

static void                     gdv_videoboy_dispose (GdvVideoBoy *this);

static void                     gdv_videoboy_finalize (GdvVideoBoy *this);

static gboolean                 private_init (GdvVideoBoyPrivate *private);

static void                     private_dispose (GdvVideoBoy *this, 
                                                 GdvVideoBoyPrivate *private);

static gboolean                 private_set (GdvVideoBoyPrivate *private, GstPad *pad, GstCaps *caps,
                                             gboolean padalloc);

static void                     free_data_list (GdvVideoBoy *this);

static gint                     compose_data_compare_func (const ComposeData *a, const ComposeData *b);

static gboolean                 composing_required (ComposeData *data);

static gboolean                 fits_master (GdvVideoBoy *this, ComposeData *data);

static RndrMode                 inspect_mode (GdvVideoBoy *this);

static gchar*                   rndr_mode_to_string (RndrMode mode);

static gboolean                 has_one_total_buffer (GdvVideoBoy *this);

static void                     render_data (GdvVideoBoy *this, GstBuffer *master,
                                             ComposeData *data);
