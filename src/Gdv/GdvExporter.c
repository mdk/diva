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

#include "GdvExporter.h"
#include "GdvExporterPrivate.h"

static GstBinClass*             ParentClass = NULL;

/* GObject type */
GType                           gdv_exporter_get_type (void)
{
        static GType objecttype = 0;
        
        if (!objecttype) {

                static const GTypeInfo objectinfo = {
                        sizeof (GdvExporterClass), 
                        NULL,  
                        NULL,
                        (GClassInitFunc) gdv_exporter_class_init, 
                        NULL,
                        NULL, 
                        sizeof (GdvExporter),
                        2, 
                        (GInstanceInitFunc) gdv_exporter_init,
                };
                objecttype = g_type_register_static (GST_TYPE_BIN, "GdvExporter", 
                                                     &objectinfo, G_TYPE_FLAG_ABSTRACT);
        }
        return objecttype;
}

/* GObject class init */
static void                     gdv_exporter_class_init (GdvExporterClass *klass)
{
        g_return_if_fail (klass != NULL);
        
        GObjectClass *gobjectclass = (GObjectClass *) klass;
        
        // Ref stuff 
        ParentClass = g_type_class_ref (GST_TYPE_BIN);
        
        gobjectclass->dispose = (gpointer) gdv_exporter_dispose;
        gobjectclass->finalize = (gpointer) gdv_exporter_finalize;
}

/* GObject dispose */
static void                     gdv_exporter_dispose (GdvExporter *this)
{
        g_return_if_fail (GDV_IS_EXPORTER (this));
        GDV_DEBUG_ELEMENT ("Disposing", NULL);
        
        if (this->Private != NULL)
                private_dispose (this, this->Private);
}

/* GObject finalize */
static void                     gdv_exporter_finalize (GdvExporter *this)
{
        g_return_if_fail (GDV_IS_EXPORTER (this));
        GDV_DEBUG_ELEMENT ("Finalizing", NULL);
        
        if (this->Private != NULL)
                g_free (this->Private);
}

/* GObject init */
static void                     gdv_exporter_init (GdvExporter *this)
{
        g_return_if_fail (GDV_IS_EXPORTER (this));
        this->Private = NULL;

        GdvExporterPrivate *priv = g_new0 (GdvExporterPrivate, 1);
        g_return_if_fail (priv != NULL);
        
        if (private_init (this, priv) == TRUE)
                this->Private = priv;
        else {
                private_dispose (this, priv);
                g_free (priv);
        }
}

/* Private init */
static gboolean                 private_init (GdvExporter *this, GdvExporterPrivate *private)
{
        g_return_val_if_fail (private != NULL, FALSE);

        // Create our video ghost pad
        private->VSink = gst_ghost_pad_new_no_target ("vsink", GST_PAD_SINK); 
        g_return_val_if_fail (private->VSink != NULL, FALSE);
        gst_element_add_pad (GST_ELEMENT (this), private->VSink);

        // Create our audio ghost pad
        private->ASink = gst_ghost_pad_new_no_target ("asink", GST_PAD_SINK); 
        g_return_val_if_fail (private->ASink != NULL, FALSE);
        gst_element_add_pad (GST_ELEMENT (this), private->ASink);
        
        return TRUE;
}

/* Private dispose
 * No need to dispose anything here, GstBin handles the added pads */
static void                     private_dispose (GdvExporter *this, GdvExporterPrivate *private)
{
        g_return_if_fail (private != NULL);
        g_return_if_fail (GDV_IS_EXPORTER (this));
}

void                            gdv_exporter_get_pads (GdvExporter *this,
                                                       GstPad **vsink, GstPad **asink)
{
        g_return_if_fail (GDV_IS_EXPORTER (this));

        g_return_if_fail (this->Private->VSink != NULL);
        g_return_if_fail (this->Private->ASink != NULL);

        *asink = this->Private->ASink;
        *vsink = this->Private->VSink;
}
