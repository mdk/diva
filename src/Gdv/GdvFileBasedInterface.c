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

/* Signifies that a given object is based on a media file */

#include "GdvFileBasedInterface.h"
#include "GdvFileBasedInterfacePrivate.h"

/* GObject type */
GType                           gdv_file_based_get_type (void)
{
        static GType interfacetype = 0;

        if (! interfacetype) {

                static const GTypeInfo info = {
                        sizeof (GdvFileBasedInterface),
                        (GBaseInitFunc) gdv_file_based_base_init,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        0,
                        0,
                        NULL,
                };
                interfacetype = g_type_register_static
                    (G_TYPE_INTERFACE, "GdvFileBasedInterface", &info, 0);
        }
        return interfacetype;
}

/* GObject base init */
static void                     gdv_file_based_base_init (GdvFileBasedInterface *interface)
{
          static gboolean initialized = FALSE;
        
          if (! initialized) {
                                    
                  g_object_interface_install_property ((gpointer) interface,
                                                       g_param_spec_string 
                                                       ("basefile", "BaseFile",
                                                        "The media file we're based on",
                                                        NULL,
                                                        G_PARAM_READABLE));
                  
                  initialized = TRUE;
          }
}
