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

        public static class ReadyMade {

                // Imports /////////////////////////////////////////////////////
                
                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_timeline_rect 
                         (IntPtr handle, ref Gdk.Rectangle rect, ref Gdk.Rectangle totalRect);

                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_lock_rect 
                         (IntPtr handle, ref Gdk.Rectangle rect);
                                
                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_clip
                         (IntPtr handle, double x0, double y0, double width, 
                          ref Gdv.Color color, 
                          IntPtr markupPtr, IntPtr pixbuf, int cut, int state);
                
                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_shield
                         (IntPtr handle, double x0, double y0, int state);

                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_tag
                         (IntPtr handle, double x0, double y0, IntPtr textPtr);

                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_tag_get_size
                         (IntPtr handle, IntPtr textPtr, out Int32 width, out Int32 height);

                [DllImport ("divacairo")]
                internal static extern void diva_cairo_readymade_time_stripe 
                         (IntPtr handle, Int32 startx, Int32 width, ref Gdv.Color color,
                          ref Gdk.Rectangle exposeRect, ref Gdk.Rectangle totalRect);

                // Static methods //////////////////////////////////////////////
                
                public static void Clip (Graphics gr, double x0, double y0, double width,
                                         Gdv.Color color, string markup, 
                                         Gdk.Pixbuf pixbuf, CairoCut cut, CairoState state)
                {
                        IntPtr strPtr = GLib.Marshaller.StringToPtrGStrdup (markup);
                        diva_cairo_readymade_clip (gr.Handle, x0, y0, width, 
                                                   ref color, strPtr, pixbuf.Handle, (int) cut, (int) state);
                        GLib.Marshaller.Free (strPtr);
                }
                
                public static void Clip (Graphics gr, double x0, double y0, double width,
                                         Gdv.Color color, string markup, 
                                         CairoCut cut, CairoState state)
                {
                        IntPtr strPtr = GLib.Marshaller.StringToPtrGStrdup (markup);
                        diva_cairo_readymade_clip (gr.Handle, x0, y0, width, 
                                                   ref color, strPtr, IntPtr.Zero, (int) cut, (int) state);
                        GLib.Marshaller.Free (strPtr);
                }

                public static void TimelineRect (Graphics gr, Gdk.Rectangle rect, Gdk.Rectangle totalRect)
                {
                        diva_cairo_readymade_timeline_rect (gr.Handle, ref rect, ref totalRect);
                }

                public static void LockRect (Graphics gr, Gdk.Rectangle rect)
                {
                        diva_cairo_readymade_lock_rect (gr.Handle, ref rect);
                }
                
                public static void Shield (Graphics gr, double x0, double y0, CairoState state)
                {
                        diva_cairo_readymade_shield (gr.Handle, x0, y0, (int) state);
                }

                public static void Tag (Graphics gr, double x0, double y0, string text)
                {
                        IntPtr strPtr = GLib.Marshaller.StringToPtrGStrdup (text);
                        diva_cairo_readymade_tag (gr.Handle, x0, y0, strPtr);
                        GLib.Marshaller.Free (strPtr);
                }
                
                public static void TagGetSize (Graphics gr, string text,
                                               out int width, out int height)
                {
                        IntPtr strPtr = GLib.Marshaller.StringToPtrGStrdup (text);
                        Int32 a, b;
                        diva_cairo_readymade_tag_get_size (gr.Handle, strPtr, out a, out b);
                        GLib.Marshaller.Free (strPtr);

                        width = a;
                        height = b;
                }

                public static void TimeStripe (Graphics gr, int startx, int width, Gdv.Color color,
                                               Gdk.Rectangle exposeRect, Gdk.Rectangle totalRect)
                {
                        diva_cairo_readymade_time_stripe (gr.Handle, startx, width, ref color, 
                                                          ref exposeRect, ref totalRect);
                }
                
        }

}
