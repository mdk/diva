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

namespace Diva.Editor.Gui {

        using System;
        using Gtk;
        using Gdk;
        using Gdv;
        using Util;
        using Cairo;
        using TimeSpan = Gdv.TimeSpan;
        using Mono.Unix;
        
        public class StripeHintDrawingArea : Gtk.DrawingArea {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string textSS = Catalog.GetString 
                        ("is {0}");

                // Fields //////////////////////////////////////////////////////

                Gdv.Color color;
                int w;
                int h;
                Gdv.Time time;
                int textSize;
                Model.Root modelRoot;

                // Properties //////////////////////////////////////////////////

                public Gdv.Color Color {
                        get { return color; }
                        set {
                                color = value;
                                QueueDraw ();
                        }
                }

                public Gdv.Time Time {
                        get { return time; }
                        set {
                                time = value;
                                QueueDraw ();
                        }
                }

                // Public methods //////////////////////////////////////////////
                                
                /* CONSTRUCTOR */
                public StripeHintDrawingArea (Model.Root root) : base ()
                {
                        this.modelRoot = root;
                        ReCalculate ((double) modelRoot.Timeline.ZoomLevel);
                                                
                        modelRoot.Timeline.DisplayedTimeSpanChange += OnDisplayedTimeSpanChanged;
                }
                
                // Private /////////////////////////////////////////////////////

                void OnDisplayedTimeSpanChanged (object o, Model.TimeSpanArgs args)
                {
                        ReCalculate ((double) modelRoot.Timeline.ZoomLevel);
                }

                void ReCalculate (double zoomLevel)
                {
                        if (zoomLevel >= 250) {
                                time = Gdv.Time.OneSecond / 25;
                                color = Gdv.Color.FromTango (ColorTango.Orange2);
                        } else if (zoomLevel >= 25) {
                                time = Gdv.Time.OneSecond;
                                color = Gdv.Color.FromTango (ColorTango.SkyBlue2);
                        } else if (zoomLevel >= 8) {
                                time = Gdv.Time.OneSecond * 4;
                                color = Gdv.Color.FromTango (ColorTango.Plum2);
                        } else {
                                time = Gdv.Time.OneSecond * 10;
                                color = Gdv.Color.FromTango (ColorTango.Aluminium5);
                        }

                        color.Alpha = 0.5f;
                        QueueDraw ();
                }
                
                protected override bool OnConfigureEvent (Gdk.EventConfigure conf)
                {
                        w = conf.Width;
                        h = conf.Height;
                        
                        return true;
                }

                protected override void OnSizeRequested (ref Requisition req)
                {
                        string txt = String.Format (textSS, TimeFu.ToLongString (time));
                        
                        Pango.Layout layout = new Pango.Layout (PangoContext);
                        layout.SetText (txt);

                        int pWidth, pHeight;
                        layout.GetSize (out pWidth, out pHeight);

                        pHeight = Pango.Units.ToPixels (pHeight);
                        pWidth = Pango.Units.ToPixels (pWidth);

                        req.Width = pWidth + 4 + pHeight;
                        req.Height = pHeight;
                }
                
                protected override bool OnExposeEvent (Gdk.EventExpose evnt)
                {
                        Graphics graphics = new Graphics (GdkWindow, Style, PangoContext);
                        string txt = String.Format (textSS, TimeFu.ToLongString (time));
                        
                        Pango.Layout layout = new Pango.Layout (PangoContext);
                        layout.SetText (txt);

                        int pWidth, pHeight;
                        layout.GetSize (out pWidth, out pHeight);

                        pHeight = Pango.Units.ToPixels (pHeight);
                        pWidth = Pango.Units.ToPixels (pWidth);

                        Cairo.Draw.RoundedSolid (graphics, 0, (h - pHeight) / 2, pHeight, pHeight, 3, color);
                        
                        GdkWindow.DrawLayout (Style.BlackGC, pHeight + 4,
                                              (h - pHeight) / 2,
                                              layout);
                        
                        graphics.Dispose ();
                        graphics = null;
                        return true;
                }
                
        }
        
}