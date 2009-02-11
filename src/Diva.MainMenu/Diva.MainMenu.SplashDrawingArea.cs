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

/* A DrawingArea to draw the splash screen. Splash is fixed to be 320x380 px
 * Additional a version information is printed using Pango stuff 
 */

namespace Diva.MainMenu {

        using System;
        using Gdk;
        using Pango;
        using Util;

        public sealed class SplashDrawingArea : Gtk.DrawingArea {
                
                // Fields ///////////////////////////////////////////////////
                
                Gdk.Pixbuf splashPixbuf;   // A pixbuf to draw

                // Public methods ///////////////////////////////////////////
 
                /* Constructor */
                public SplashDrawingArea (Gdk.Pixbuf pixbuf) : base ()
                {
                        splashPixbuf = pixbuf;
                }
                
                // Events ///////////////////////////////////////////////////

                protected override void OnSizeRequested (ref Gtk.Requisition req)
                {
                        // Out splash dimensions
                        req.Width = 320;
                        req.Height = 380;
                }
                
                protected override bool OnExposeEvent (Gdk.EventExpose evnt)
                {
                        // Draw the intersting part of the pixbuf
                        GdkWindow.DrawPixbuf (Style.ForegroundGC (Gtk.StateType.Normal), splashPixbuf,
                                              evnt.Region.Clipbox.Left, evnt.Region.Clipbox.Top, 
                                              evnt.Region.Clipbox.Left, evnt.Region.Clipbox.Top, 
                                              evnt.Region.Clipbox.Width, evnt.Region.Clipbox.Height, 
                                              Gdk.RgbDither.Normal, 0, 0);
                        
                        // Draw the version information
                        string text = String.Format ("<b>ver {0}\n{1}</b>", 
                                                     VersionFu.GetCallingVersion(),
                                                     VersionFu.GetCallingSlugline());

                        Pango.Layout layout = new Pango.Layout (PangoContext);
                        // Minus 10 to get some margin
                        layout.Width = Pango.Units.FromPixels (310);
                        layout.Alignment = Pango.Alignment.Right;
                        layout.SetMarkup (text);

                        int pWidth, pHeight;
                        layout.GetSize (out pWidth, out pHeight);
                        pHeight = Pango.Units.ToPixels (pHeight);

                        GdkWindow.DrawLayout (Style.BlackGC, 0,
                                              (380 - pHeight - 5),
                                              layout);
                        
                        return true;
                }
        }

}
