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

/* An interface that defines a media item that can generate frame previews/thumbnails
 * The process of acquiring the frames is pretty complicated and involves jobs, jobprocessor,
 * framerunner etc. This is very implementation-specific, and some items might actually
 * return a static pixbuf as the frame thumbnail 
 */

#include "GdvFrameableInterface.h"
#include "GdvFrameableInterfacePrivate.h"

/* GObject type */
GType                           gdv_frameable_get_type (void)
{
        static GType interfacetype = 0;

        if (! interfacetype) {

                static const GTypeInfo info = {
                        sizeof (GdvFrameableInterface),
                        (GBaseInitFunc) gdv_frameable_base_init,
                        NULL,
                        NULL,
                        NULL,
                        NULL,
                        0,
                        0,
                        NULL,
                };
                interfacetype = g_type_register_static
                    (G_TYPE_INTERFACE, "GdvFrameableInterface", &info, 0);
                
                
        }
        return interfacetype;
}

/* GObject base init */
static void                     gdv_frameable_base_init (GdvFrameableInterface *interface)
{
          static gboolean initialized = FALSE;
        
          if (! initialized) {
                  // Nothing to do here. Leaving this for future
                  initialized = TRUE;
          }
}

/* Wrapper function */
GdvToken                        gdv_frameable_get_frame (GdvFrameableInterface *this, GdvTime attime, 
                                                         GdvFrameDimensions *dim, GdvToken *settoken, 
                                                         GSourceFunc callback)
{
        g_return_val_if_fail (GDV_IS_FRAMEABLE (this), -1);
        g_return_val_if_fail (this->GetFrameFunc != NULL, -1);
        
        return this->GetFrameFunc ((GdvMediaItem *) this, attime, dim,
                                   settoken, callback);
}


