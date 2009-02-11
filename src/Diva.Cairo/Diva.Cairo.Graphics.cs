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

namespace Diva.Cairo {

        using System;
        using System.Runtime.InteropServices;
        
        public sealed class Graphics : IDisposable {
                                
                // Imports /////////////////////////////////////////////////////

                [DllImport ("divacairo")]
                internal static extern IntPtr diva_cairo_graphics_new
                        (IntPtr window, IntPtr style, IntPtr context);
                
                [DllImport ("divacairo")]
                internal static extern IntPtr diva_cairo_graphics_dispose
                         (IntPtr handle);
                
                [DllImport ("divacairo")]
                internal static extern IntPtr diva_cairo_graphics_finalize
                         (IntPtr handle);
                
                [DllImport ("divacairo")]
                internal static extern IntPtr diva_cairo_graphics_get_window
                         (IntPtr handle);
                
                [DllImport ("divacairo")]
                internal static extern IntPtr diva_cairo_graphics_get_style
                         (IntPtr handle);
                
                [DllImport ("divacairo")]
                internal static extern IntPtr diva_cairo_graphics_get_context
                         (IntPtr handle);
                
                // Fields //////////////////////////////////////////////////////
                
                IntPtr handle; // Unmanaged handle
                
                // Properties //////////////////////////////////////////////////

                public IntPtr Handle {
                        get { return handle; }
                }
                
                public Gdk.Window Window {
                        get {
                                IntPtr ptr = diva_cairo_graphics_get_window (handle);
                                if (ptr == IntPtr.Zero)
                                        throw new Exception ("Failed to get GdkWindow");
                                
                                return new Gdk.Window (ptr);
                        }
                }
                
                public Gtk.Style Style {
                        get {
                                IntPtr ptr = diva_cairo_graphics_get_style (handle);
                                if (ptr == IntPtr.Zero)
                                        throw new Exception ("Failed to get Style");
                                
                                return new Gtk.Style (ptr);
                        }
                }
                
                public Pango.Context Context {
                        get {
                                IntPtr ptr = diva_cairo_graphics_get_context (handle);
                                if (ptr == IntPtr.Zero)
                                        throw new Exception ("Failed to get Context");
                                
                                return new Pango.Context (ptr);
                        }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Graphics (Gdk.Window window, Gtk.Style style, Pango.Context context)
                {
                        handle = diva_cairo_graphics_new (window.Handle, style.Handle, context.Handle);
                        if (handle == IntPtr.Zero)
                                throw new Exception ("Failed to initialized Cairo graphics for the given window");
                }
                
                public void Dispose ()
                {
                        diva_cairo_graphics_dispose (handle);
                }
                
                /* FINALIZER */
                ~Graphics ()
                {
                        diva_cairo_graphics_finalize (handle);
                }

        }
        
}
                
