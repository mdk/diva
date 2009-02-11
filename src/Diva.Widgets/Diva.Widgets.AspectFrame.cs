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

namespace Diva.Widgets {

        using System;
        using System.Runtime.InteropServices;
        using GLib;
        using Gdv;
        
        public class AspectFrame : Gtk.DrawingArea {
                
                // Imports ////////////////////////////////////////////////////
                
                [DllImport ("divawidgets")]
                internal static extern IntPtr diva_widgets_aspect_frame_new
                         ();
                
                // Propeties //////////////////////////////////////////////////

                public Fraction PixelAspect {
                        get { return (Fraction) GetProperty ("pixelaspect"); }
                        set { SetProperty ("pixelaspect", new Value(value)); }
                }

                public Fraction Fps {
                        get { return (Fraction) GetProperty ("fps"); }
                        set { SetProperty ("fps", new Value(value)); }
                }

                public FrameDimensions Dimensions {
                        get { return (FrameDimensions) GetProperty ("framedimensions"); }
                        set { SetProperty ("framedimensions", new Value(value)); }
                }
                
                // Public methods /////////////////////////////////////////////
                
                public AspectFrame () : base (IntPtr.Zero)
                {
                        IntPtr ptr = diva_widgets_aspect_frame_new ();
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        else
                                Raw = ptr;
                }
                
        }
        
}
                
