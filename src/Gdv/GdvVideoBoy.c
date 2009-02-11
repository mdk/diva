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

#include "GdvVideoBoy.h"
#include "GdvVideoBoyPrivate.h"

static GObjectClass*            ParentClass = NULL;

/* GObject type */
GType                           gdv_videoboy_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvVideoBoyClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) gdv_videoboy_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvVideoBoy),
                        8, 
                        (GInstanceInitFunc) gdv_videoboy_init,
                };
                objecttype = g_type_register_static (G_TYPE_OBJECT, "GdvVideoBoy", 
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_videoboy_class_init (GdvVideoBoyClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (G_TYPE_OBJECT);
        
        gobjectclass->dispose = (gpointer) gdv_videoboy_dispose;
        gobjectclass->finalize = (gpointer) gdv_videoboy_finalize;
}

/* GObject dispose */
static void                     gdv_videoboy_dispose (GdvVideoBoy *this)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        GDV_DEBUG_EXT ("Disposing", NULL);

        if (this->Private != NULL &&
            this->Private->Mode != BOY_MODE_EMPTY)
                GDV_WARNING_EXT ("Disposing in mode != empty", NULL);
        
        if (this->Private != NULL) 
                private_dispose (this, this->Private);
        
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_videoboy_finalize (GdvVideoBoy *this)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        GDV_DEBUG_EXT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }
        
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* GObject init */
static void                     gdv_videoboy_init (GdvVideoBoy *this)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        this->Private = NULL;

        GdvVideoBoyPrivate *priv = g_new0 (GdvVideoBoyPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Private init */
static gboolean                 private_init (GdvVideoBoyPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        private->Pad = NULL;
        private->Mode = BOY_MODE_EMPTY;
        private->DataList = NULL;
        private->ValidBuffers = 0;
        private->UsePadAlloc = FALSE;
        private->MasterCaps = NULL;
        private->MasterSize = 0;
        private->MasterCC = 0;
        
        private->Mutex = g_mutex_new ();
        g_return_val_if_fail (private->Mutex != NULL, FALSE);
                
        return TRUE;
}

static gboolean                 private_set (GdvVideoBoyPrivate *private, GstPad *pad, GstCaps *caps,
                                             gboolean padalloc)
{
        g_return_val_if_fail (GST_IS_PAD (pad), FALSE);
        g_return_val_if_fail (caps != NULL, FALSE);

        private->Pad = pad;
        private->UsePadAlloc = padalloc;
        private->MasterCaps = caps;
        gst_caps_ref (private->MasterCaps);

        // Extract some data from caps
        GstStructure *capsstruct = gst_caps_get_structure (caps, 0);
        g_return_val_if_fail (capsstruct != NULL, FALSE);

        int w, h;

        gst_structure_get_int (capsstruct, "width", &w);
        gst_structure_get_int (capsstruct, "height", &h);
        private->MasterDimensions = gdv_framedimensions_new (w, h);
        private->MasterPixelAspect = gdv_fraction_aspect_new_from_gst (capsstruct);
        gst_structure_get_fourcc (capsstruct, "format", &private->MasterCC);

        private->MasterSize = w * h * 2; // FIXME: Totally ugly!
        return TRUE;
}

/* Private dispose */
static void                     private_dispose (GdvVideoBoy *this, GdvVideoBoyPrivate *private)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (GDV_IS_VIDEOBOY (this));

        if (private->Mutex != NULL) {
                g_mutex_free (private->Mutex);
                private->Mutex = NULL;
        }

        if (private->DataList != NULL)
                free_data_list (this);

        if (private->Pad != NULL)
                private->Pad = NULL;

        if (private->MasterCaps != NULL) {
                gst_caps_unref (private->MasterCaps);
                private->MasterCaps = NULL;
        }

        if (private->MasterPixelAspect != NULL) {
                gdv_fraction_free (private->MasterPixelAspect);
                private->MasterPixelAspect = NULL;
        }

        if (private->MasterDimensions != NULL) {
                gdv_framedimensions_free (private->MasterDimensions);
                private->MasterDimensions = NULL;
        }
                
}

/* Create a new instance */
GdvVideoBoy*                    gdv_videoboy_new (GstPad *pad, GstCaps *caps, gboolean padalloc)
{
        g_return_val_if_fail (GST_IS_PAD (pad), NULL);
        
        GDV_DEBUG_EXT ("Creating a new GdvVideoBoy", NULL);
        
        GdvVideoBoy *this = g_object_new (GDV_TYPE_VIDEOBOY, NULL);

        if (this == NULL)
                return NULL;

        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }
        
        if (! private_set (this->Private, pad, caps, padalloc)) {
                g_object_unref (this);
                return NULL;
        }
        
        return this;
}

/* Free the compose list */
static void                     free_data_list (GdvVideoBoy *this)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        GDV_DEBUG_EXT ("Freeing compose list", NULL);

        GList *iterator;
        
        for (iterator = this->Private->DataList; iterator; iterator = g_list_next (iterator)) {
                ComposeData *comp = (ComposeData *) iterator->data;

                if (comp != NULL) {
                        if (comp->Buffer != NULL)
                                gst_buffer_unref (comp->Buffer);

                        if (comp->Dimensions != NULL)
                                gdv_framedimensions_free (comp->Dimensions);

                        if (comp->PixelAspect != NULL)
                                gdv_fraction_free (comp->PixelAspect);
                }
        }

        g_list_free (this->Private->DataList);
        this->Private->DataList = NULL;
        this->Private->ValidBuffers = 0;
}

void                            gdv_videoboy_start_composing (GdvVideoBoy *this)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        GDV_DEBUG_EXT ("Starting composing", NULL);

        BOY_LOCK (this);
        if (this->Private->Mode != BOY_MODE_EMPTY)
                goto error;
                
        this->Private->Mode = BOY_MODE_COMP;
        this->Private->ValidBuffers = 0;
        goto done;

 error:
        GDV_WARNING_EXT ("Invalid mode", NULL);
        
 done:
        BOY_UNLOCK (this);
}

void                            gdv_videoboy_abort_composing (GdvVideoBoy *this)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        GDV_DEBUG_EXT ("Aborting composing", NULL);

        BOY_LOCK (this);
        
        if (this->Private->Mode != BOY_MODE_COMP)
                goto error;

        this->Private->Mode = BOY_MODE_EMPTY;

        if (this->Private->DataList != NULL)
                free_data_list (this);
        goto done;

 error:
        GDV_WARNING_EXT ("Invalid mode", NULL);
                
 done:
        BOY_UNLOCK (this);
}

gboolean                        gdv_videoboy_add_buffer (GdvVideoBoy *this, GstBuffer *buffer)
{
        g_return_val_if_fail (GDV_IS_VIDEOBOY (this), FALSE);
        g_return_val_if_fail (buffer != NULL, FALSE);
        
        // Params
        int bufferw, bufferh;
        GdvFraction *aspect = NULL;
        GdvFrameDimensions *dimensions = NULL;
        guint32 buffercc;
        int layer;
        double opacity;

        // Get caps
        GstCaps *buffercaps = GST_BUFFER_CAPS (buffer);
        g_return_val_if_fail (buffercaps != NULL, FALSE);

        GstStructure *bufferstruct = gst_caps_get_structure (buffercaps, 0);
        g_return_val_if_fail (bufferstruct != NULL, FALSE);

        // Get meta-params
        if (GDV_IS_VIDEOBUFFER (buffer)) {

                GdvVideoBuffer *videobuffer = (GdvVideoBuffer *) buffer;

                // We skip it completely
                if (gdv_videobuffer_meta_get_null (videobuffer) == TRUE)
                        goto done;
                
                layer = gdv_videobuffer_meta_get_layer (videobuffer);
                opacity = gdv_videobuffer_meta_get_opacity (videobuffer);
        } else {
                layer = 1;
                opacity = 1.0;
        }

        // Get params
        gst_structure_get_int (bufferstruct, "width", &bufferw);
        gst_structure_get_int (bufferstruct, "height", &bufferh);
        dimensions = gdv_framedimensions_new (bufferw, bufferh);
        gst_structure_get_fourcc (bufferstruct, "format", &buffercc);
        aspect = gdv_fraction_aspect_new_from_gst (bufferstruct);

        g_return_val_if_fail (aspect != NULL, FALSE);
        g_return_val_if_fail (dimensions != NULL, FALSE);

        GDV_DEBUG_EXT ("Adding %s [%"
                       GST_FOURCC_FORMAT
                       "] px-aspect: %s, "
                       " opacity: %.2f, "
                       " layer: %d",
                       gdv_framedimensions_to_string (dimensions),
                       GST_FOURCC_ARGS (buffercc),
                       gdv_fraction_to_string (aspect),
                       opacity,
                       layer);

        // Create the structure
        ComposeData *data = g_new (ComposeData, 1);
        data->Buffer = buffer;
        data->Dimensions = dimensions;
        data->PixelAspect = aspect;
        data->Layer = layer;
        data->Opacity = opacity;
        data->PixelData = GST_BUFFER_DATA (buffer);
        data->FourCC = buffercc;
        
        // Add it
        BOY_LOCK (this);
        this->Private->DataList = g_list_insert_sorted (this->Private->DataList, (gpointer) data,
                                                        (GCompareFunc) compose_data_compare_func);
        this->Private->ValidBuffers++;
        BOY_UNLOCK (this);
        
        gst_buffer_ref (buffer);

 done:
        return TRUE;
}

/* A comparison function. Using the layer parameter */
static gint                     compose_data_compare_func (const ComposeData *a, const ComposeData *b)
{
        g_return_val_if_fail (a != NULL && b != NULL, 0);
                        
        if (a->Layer < b->Layer)
                return -1;
        else if (b->Layer > a->Layer)
                return 1;
        else
                return 0;
}

/* Check if composing for this data IS required as a result of meta params */
static gboolean                 composing_required (ComposeData *data)
{
        g_return_val_if_fail (data != NULL, FALSE);

        if (data->Opacity != 1.0)
                return TRUE;

        return FALSE;        
}

/* Check if the given data fits the master exactly -- and can be (supposingly) forwarded
   without any extra rendering */
static gboolean                 fits_master (GdvVideoBoy *this, ComposeData *data)
{
        g_return_val_if_fail (GDV_IS_VIDEOBOY (this), FALSE);
        g_return_val_if_fail (data != NULL, FALSE);

        if (gdv_framedimensions_compare (data->Dimensions, this->Private->MasterDimensions) != 0)
                return FALSE;

        if (gdv_fraction_compare (data->PixelAspect, this->Private->MasterPixelAspect) != 0) 
                return FALSE;

        if (data->FourCC != this->Private->MasterCC) 
                return FALSE;
        
        return TRUE;
}

/* Create a master buffer to compose to */
static GstBuffer*               get_master (GdvVideoBoy *this, gboolean fill)
{
        g_return_val_if_fail (GDV_IS_VIDEOBOY (this), NULL);

        GstBuffer *master;

        if (this->Private->UsePadAlloc == TRUE) {
                GDV_DEBUG_EXT ("Creating a master render buffer using pad alloc", NULL);
                gst_pad_alloc_buffer (this->Private->Pad, 0,
                                      this->Private->MasterSize, this->Private->MasterCaps,
                                      &master);
        } else {
                GDV_DEBUG_EXT ("Creating a master render buffer using raw alloc", NULL);
                master = gst_buffer_new_and_alloc (this->Private->MasterSize);
                gst_buffer_set_caps (master, this->Private->MasterCaps);
        }

        g_assert (master != NULL);

        // FIXME: Use oil memset here to get that done faster!
        if (fill == TRUE) {
                guchar *buf_iterator = GST_BUFFER_DATA (master);
                int i = 0;
                // FIXME: Total crap below to reset the buf
                for (i = 0; i < this->Private->MasterSize; i+=2) {
                        buf_iterator[i] = 0;
                        buf_iterator[i + 1]  = 128;
                }
        }
        
        return master;
}

static RndrMode                 inspect_mode (GdvVideoBoy *this)
{
        g_return_val_if_fail (GDV_IS_VIDEOBOY (this), RNDR_MODE_INVALID);
        
        GDV_DEBUG_EXT ("Inspecting the mode", NULL);

        // Let's analyze us...
        if (this->Private->ValidBuffers == 0)
                return RNDR_MODE_EMPTY_MASTER;

        // Check if we can just blit the first one
        if (this->Private->ValidBuffers == 1 &&
            fits_master (this, (ComposeData *) this->Private->DataList->data) &&
            ! composing_required ((ComposeData *) this->Private->DataList->data))
                return RNDR_MODE_REUSE_FIRST;

        // Check if we can just blit the last one
        if (this->Private->ValidBuffers > 1) {
                ComposeData *comp = (ComposeData *) g_list_last (this->Private->DataList)->data;
                if (fits_master (this, comp) &&
                    ! composing_required (comp))
                        return RNDR_MODE_REUSE_LAST;
        }
        
        return RNDR_MODE_COMPOSE_FROM_SCRATCH;
}

static gchar*                   rndr_mode_to_string (RndrMode mode)
{
        switch (mode) {
                
                case RNDR_MODE_REUSE_FIRST:
                        return "Reuse first buffer, fast";
                        break;

                case RNDR_MODE_REUSE_LAST:
                        return "Reuse last buffer, fast";
                        break;
                        
                case RNDR_MODE_EMPTY_MASTER:
                        return "Generate an empty master buffer";
                        break;

                case RNDR_MODE_COMPOSE_FROM_SCRATCH:
                        return "Compose from scratch, slow";
                        break;
                        
                default:
                        return "Invalid";
                        break;
                        
        }
}

GstBuffer*                      gdv_videoboy_get_rendered_buffer (GdvVideoBoy *this)
{
        g_return_val_if_fail (GDV_IS_VIDEOBOY (this), NULL);
        BOY_LOCK (this);

        GstBuffer *outbuffer = NULL;

        GDV_DEBUG_EXT ("Getting the rendered buffer out", NULL);

        if (this->Private->Mode != BOY_MODE_COMP) {
                GDV_WARNING_EXT ("Bad mode", NULL);
                goto done;
        }

        // First let's get the mode
        RndrMode rndrmode = inspect_mode (this);
        GDV_DEBUG_EXT ("Render more is: %s", rndr_mode_to_string (rndrmode));

        switch (rndrmode) {

                case RNDR_MODE_EMPTY_MASTER: {
                        outbuffer = get_master (this, TRUE);
                } break;

                case RNDR_MODE_REUSE_FIRST: {
                        ComposeData *comp = (ComposeData *) this->Private->DataList->data;
                        outbuffer = comp->Buffer;
                        gst_buffer_ref (outbuffer);
                } break;

                case RNDR_MODE_REUSE_LAST: {
                        ComposeData *comp = (ComposeData *) g_list_last (this->Private->DataList)->data;
                        outbuffer = comp->Buffer;
                        gst_buffer_ref (outbuffer);
                } break;

                case RNDR_MODE_COMPOSE_FROM_SCRATCH: {
                        outbuffer = get_master (this, has_one_total_buffer (this));
                        GList *iterator;

                        // Iterate through all inputs
                        for (iterator = this->Private->DataList; iterator; iterator = g_list_next (iterator)) {
                                ComposeData *comp = iterator->data;
                                render_data (this, outbuffer, comp);
                        }
                        
                } break;
                        
                default:
                        GDV_WARNING_ELEMENT ("Unsupported mode", NULL);
                        g_assert_not_reached ();
        }

        GDV_DEBUG_EXT ("Done composing, freeing", NULL);
        this->Private->Mode = BOY_MODE_EMPTY;
        if (this->Private->DataList != NULL)
                free_data_list (this);
        
        g_assert (outbuffer != NULL);
                
 done:
        BOY_UNLOCK (this);
        return outbuffer;
}

/* Check if we have at least one buffer that fills the whole output AND sets all the
 * pixels without blending. This is used ie. to check if we can get the master buffer without
 * any filling */
static gboolean                 has_one_total_buffer (GdvVideoBoy *this)
{
        g_return_val_if_fail (GDV_IS_VIDEOBOY (this), FALSE);
        GDV_DEBUG_EXT ("Checking if we have one total buffer", NULL);
        
        if (this->Private->ValidBuffers == 0)
                return FALSE;
        
        GList *iterator;
        
        for (iterator = this->Private->DataList; iterator; iterator = g_list_next (iterator)) {
                ComposeData *comp = (ComposeData *) iterator->data;

                if (comp != NULL) {
                        if (comp->Opacity >= 1.0)
                                return TRUE;
                }
        }

        return FALSE;
}

static void                     render_data (GdvVideoBoy *this, GstBuffer *master,
                                             ComposeData *data)
{
        g_return_if_fail (GDV_IS_VIDEOBOY (this));
        g_return_if_fail (master != NULL);
        g_return_if_fail (data != NULL);

        GDV_DEBUG_EXT ("Adding %s [%"
                       GST_FOURCC_FORMAT
                       "] px-aspect: %s, "
                       " opacity: %.2f, "
                       " layer: %d",
                       gdv_framedimensions_to_string (data->Dimensions),
                       GST_FOURCC_ARGS (data->FourCC),
                       gdv_fraction_to_string (data->PixelAspect),
                       data->Opacity,
                       data->Layer);

        gpointer pixels = NULL;

        // FIXME: Performance shit here
        if (data->FourCC != this->Private->MasterCC) {

                pixels = g_malloc (data->Dimensions->Width * data->Dimensions->Height * 2);
                
                GDV_DEBUG_EXT ("Converting [%"
                               GST_FOURCC_FORMAT
                               "] to [%",
                               GST_FOURCC_ARGS (data->FourCC),
                               GST_FOURCC_ARGS (this->Private->MasterCC));
                
                gdv_i420_to_yuy2 (data->PixelData,
                                  data->Dimensions->Width, data->Dimensions->Height, 0,
                                  pixels, 
                                  data->Dimensions->Width, data->Dimensions->Height, 0);
        } else
                pixels = data->PixelData;

        g_assert (pixels != NULL);
        gdv_videobuffer_scale_compose_yuy2 (pixels,
                                            data->Dimensions->Width, data->Dimensions->Height,
                                            GST_BUFFER_DATA (master), 
                                            this->Private->MasterDimensions->Width,
                                            this->Private->MasterDimensions->Height,
                                            data->Opacity);

        // Mem freeing
        if (pixels != data->PixelData)
                g_free (pixels);
}


