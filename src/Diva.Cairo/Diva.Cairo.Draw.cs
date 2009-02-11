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
        using Gdk;
        using Gtk;
        using System.Runtime.InteropServices;
        
        public static class Draw {

                // Imports /////////////////////////////////////////////////////

                [DllImport ("divacairo")]
                internal static extern void diva_cairo_draw_line
                        (IntPtr handle, double x0, double y0, double x1, double y1,
                         ref Gdv.Color color, double lw, bool correct);
                
                [DllImport ("divacairo")]
                internal static extern void diva_cairo_draw_rounded_gradient
                        (IntPtr handle, double x0, double y0, double width, double height,
                         double radius, ref Gdv.Color color1, ref Gdv.Color color2, int cut);
                
                [DllImport ("divacairo")]
                internal static extern void diva_cairo_draw_rounded_solid
                        (IntPtr handle, double x0, double y0, double width, double height,
                         double radius, ref Gdv.Color color, int cut);
                
                [DllImport ("divacairo")]
                internal static extern void diva_cairo_draw_rounded_outline
                        (IntPtr handle, double x0, double y0, double width, double height,
                         double radius, int outline, int cut);

                [DllImport ("divacairo")]
                internal static extern void diva_cairo_draw_solid_rect
                        (IntPtr handle, double x0, double y0, double width, double height,
                         ref Gdv.Color color);

                // Static methods //////////////////////////////////////////////
                
                public static void Line (Graphics graphics, double x0, double y0, double x1, double y1,
                                         Gdv.Color color, double lw, bool correct)
                {
                        diva_cairo_draw_line (graphics.Handle, x0, y0, x1, y1, ref color, lw, correct);
                }
                
                public static void RoundedGradient (Graphics graphics, double x0, double y0, double width, double height,
                                                    double radius, Gdv.Color color1, Gdv.Color color2, CairoCut cut)
                {
                        diva_cairo_draw_rounded_gradient (graphics.Handle, x0, y0, width, height, radius,
                                                          ref color1, ref color2, (int) cut);
                }
                
                public static void RoundedGradient (Graphics graphics, double x0, double y0, double width, double height,
                                                    double radius, Gdv.Color color1, Gdv.Color color2)
                {
                        RoundedGradient (graphics, x0, y0, width, height,
                                         radius, color1, color2, CairoCut.None);
                }
                
                
                public static void RoundedSolid (Graphics graphics, double x0, double y0, double width, double height,
                                                 double radius, Gdv.Color color, CairoCut cut)
                {
                        diva_cairo_draw_rounded_solid (graphics.Handle, x0, y0, width, height, radius,
                                                       ref color, (int) cut);
                }
                
                public static void RoundedSolid (Graphics graphics, double x0, double y0, double width, double height,
                                                 double radius, Gdv.Color color)
                {
                        RoundedSolid (graphics, x0, y0, width, height,
                                      radius, color, CairoCut.None);
                }
                
                public static void RoundedOutline (Graphics graphics, double x0, double y0, double width, double height,
                                                   double radius, CairoOutline outline, CairoCut cut)
                {
                        diva_cairo_draw_rounded_outline (graphics.Handle, x0, y0, width, height, radius,
                                                         (int) outline, (int) cut);
                }
                
                public static void RoundedOutline (Graphics graphics, double x0, double y0, double width, double height,
                                                   double radius, CairoOutline outline)
                {
                        RoundedOutline (graphics, x0, y0, width, height,
                                        radius, outline, CairoCut.None);
                }

                public static void SolidRect (Graphics graphics, double x0, double y0, double width, double height,
                                              Gdv.Color color)
                {
                        diva_cairo_draw_solid_rect (graphics.Handle, x0, y0, width, height, ref color);
                }
                
        }

}
