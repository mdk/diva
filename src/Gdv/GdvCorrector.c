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

#include "GdvCorrector.h"
#include "GdvCorrectorPrivate.h"

static GstBaseTransformClass*   ParentClass = NULL;

static GstStaticPadTemplate     SinkFactory = GST_STATIC_PAD_TEMPLATE ("sink",
                                                                       GST_PAD_SINK,
                                                                       GST_PAD_ALWAYS,
                                                                       GST_STATIC_CAPS_ANY);

static GstStaticPadTemplate     SrcFactory = GST_STATIC_PAD_TEMPLATE  ("src",
                                                                       GST_PAD_SRC,
                                                                       GST_PAD_ALWAYS,
                                                                       GST_STATIC_CAPS_ANY);

/* Base init to all the BaseTransform pads */
void                            gdv_corrector_base_init (gpointer klass)
{
        GstElementClass *elementclass = GST_ELEMENT_CLASS (klass);
        
        gst_element_class_add_pad_template (elementclass, 
                                            gst_static_pad_template_get (&SinkFactory));
        
        gst_element_class_add_pad_template (elementclass,
                                            gst_static_pad_template_get (&SrcFactory));
}

/* GObject type */
GType                           gdv_corrector_get_type (void)
{
        static GType objecttype = 0;

        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvCorrectorClass), 
                        gdv_corrector_base_init, 
                        NULL,
                        (GClassInitFunc) gdv_corrector_class_init,
                        NULL,
                        NULL, 
                        sizeof (GdvCorrector), 
                        4, 
                        (GInstanceInitFunc) gdv_corrector_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BASE_TRANSFORM, "GdvCorrector",
                                                     &objectinfo, 0);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_corrector_class_init (GdvCorrectorClass *klass)
{
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        GstBaseTransformClass *basetransformclass = (GstBaseTransformClass *) klass;
        
        gobjectclass->dispose = (gpointer) gdv_corrector_dispose;
        gobjectclass->finalize = (gpointer) gdv_corrector_finalize;

        basetransformclass->prepare_output_buffer = (gpointer) prepare_output_buffer;
        basetransformclass->transform_ip = (gpointer) transform_ip; 
        
        ParentClass = g_type_class_ref (GST_TYPE_BASE_TRANSFORM);
}

/* GObject init */
static void                     gdv_corrector_init (GdvCorrector *this)
{
        g_return_if_fail (GDV_IS_CORRECTOR (this));
        GDV_DEBUG_ELEMENT ("Initializing new GdvCorrector", NULL);

        this->Private = NULL;

        GdvCorrectorPrivate *priv = g_new0 (GdvCorrectorPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE) 
                this->Private = priv;
        else {
                private_dispose (priv);
                g_free (priv);
                this->Private = NULL;
        }
}

static gboolean                 private_init (GdvCorrector *this, GdvCorrectorPrivate *private)
{
        g_return_val_if_fail (GDV_IS_CORRECTOR (this), FALSE);
        g_return_val_if_fail (private != NULL, FALSE);

        private->Correction = FALSE;
        private->HasCorrection = FALSE;
        private->LastTime = 0;
        private->HasLastTime = FALSE;
        private->Minus = FALSE;
        
        return TRUE;
}

static void                     private_dispose (GdvCorrectorPrivate *private)
{
        g_return_if_fail (private != NULL);
}

/* GObject dispose */
static void                     gdv_corrector_dispose (GdvCorrector *this)
{
        g_return_if_fail (GDV_IS_CORRECTOR (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);

        if (this->Private != NULL)
                private_dispose (this->Private);

        // FIXME: Wrong dispose
        G_OBJECT_CLASS (ParentClass)->dispose (G_OBJECT (this));
}

/* GObject finalize */
static void                     gdv_corrector_finalize (GdvCorrector *this)
{
        g_return_if_fail (GDV_IS_CORRECTOR (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL) {
                g_free (this->Private);
                this->Private = NULL;
        }

        // FIXME: Wrong finalize
        G_OBJECT_CLASS (ParentClass)->finalize (G_OBJECT (this));
}

/* Here we actually modify the metadata of the buffer */
static GstFlowReturn            prepare_output_buffer (GdvCorrector *this, GstBuffer *input,
                                                       gint size, GstCaps *caps, GstBuffer **buf)
{
        g_return_val_if_fail (input != NULL, GST_FLOW_ERROR);
        g_return_val_if_fail (GDV_IS_CORRECTOR (this), GST_FLOW_ERROR);
        
        gst_buffer_ref (input);

        // First try to correct
        GdvTime origtime = GST_BUFFER_TIMESTAMP (input);
        
        if (this->Private->HasCorrection == TRUE)
                GST_BUFFER_TIMESTAMP (input) = correct_stamp (this, GST_BUFFER_TIMESTAMP (input));

        // Now let's try to update the correction settings

        if (this->Private->HasLastTime) {
                if (GST_BUFFER_TIMESTAMP (input) > this->Private->LastTime) {
                        if (GST_BUFFER_TIMESTAMP (input) - this->Private->LastTime > TOLERANCE) {
                                this->Private->Correction = origtime - this->Private->LastTime;
                                this->Private->HasCorrection = TRUE;
                                GDV_DEBUG_ELEMENT ("Adjusting correction to (minus) %s",
                                                   gdv_time_to_string (this->Private->Correction));
                                GST_BUFFER_TIMESTAMP (input) = correct_stamp (this, origtime);
                        }
                } else if (GST_BUFFER_TIMESTAMP (input) < this->Private->LastTime) {
                        if (this->Private->LastTime - GST_BUFFER_TIMESTAMP (input) > TOLERANCE) {
                                this->Private->Correction = this->Private->LastTime - origtime;
                                this->Private->HasCorrection = TRUE;
                                GDV_DEBUG_ELEMENT ("Adjusting correction to (plus) %s",
                                                   gdv_time_to_string (this->Private->Correction));
                                GST_BUFFER_TIMESTAMP (input) = correct_stamp (this, origtime);
                        }
                }
        }
                                
        this->Private->LastTime = GST_BUFFER_TIMESTAMP (input) + GST_BUFFER_DURATION (input);
        this->Private->HasLastTime = TRUE;
        
        *buf = input;
        
        return GST_FLOW_OK;
}

/* A stubby function just to force the _prepare_output_buffer func */
static GstFlowReturn            transform_ip (GdvCorrector *this, GstBuffer *buf)
{
        // No work here. It's all done in _prepare_output_buffer.
        return GST_FLOW_OK;
}

/* Create a new instance */
GdvCorrector*                    gdv_corrector_new (void)
{
        GdvCorrector *this = g_object_new (GDV_TYPE_CORRECTOR, NULL);
        g_return_val_if_fail (this != NULL, NULL);

        if (this->Private == NULL) {
                g_object_unref (this);
                return NULL;
        }
        
        return this;
}

static GdvTime                  correct_stamp (GdvCorrector *this, GdvTime time)
{
        g_return_val_if_fail (GDV_IS_CORRECTOR (this), GDV_ZEROTIME);
        g_return_val_if_fail (this->Private->HasCorrection == TRUE, GDV_ZEROTIME);

        if (this->Private->Minus == TRUE)
                return time - this->Private->Correction;
        else
                return time + this->Private->Correction;
}

void                            gdv_corrector_reset (GdvCorrector *this)
{
        g_return_if_fail (GDV_IS_CORRECTOR (this));

        // FIXME: Mutex here!
        GDV_DEBUG_ELEMENT ("Resetting corrector", NULL);

        this->Private->HasCorrection = FALSE;
        this->Private->HasLastTime = FALSE;
}

