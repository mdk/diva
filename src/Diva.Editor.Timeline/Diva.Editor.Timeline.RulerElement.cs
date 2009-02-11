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

namespace Diva.Editor.Timeline {

        using System;
        using Gdk;
        using Gtk;
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;
        using Util;
        using Core;
        using Cairo;
        
        public class RulerElement : ViewElement {

                // Fields //////////////////////////////////////////////////////
               
                Time smallUnit;  // Unit for spacing (small tick)
                Time bigUnit;    // Unit for spacing (big tick)
                Time stringUnit; // Unit for spacing (the string with time)
                
                // Properties //////////////////////////////////////////////////
                
                public override Rectangle DrawingRect {
                        get { 
                                Rectangle rect = modelRoot.Timeline.ViewportRectangle;
                                return new Rectangle (rect.Left, rect.Top,
                                                      rect.Width, 16);
                        }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public RulerElement (Model.Root root) : base (root, ElementLayer.Ruler)
                {
                        smallUnit = Time.FromSeconds (0.5);
                        bigUnit = Time.FromSeconds (1);
                        stringUnit = Time.FromSeconds (4);
                }
                
                /* This our core thing 
                 * FIXME: We should move this function (drawing) to the C code ReadyMades */
                public override void Draw (Graphics gr, Gdk.Rectangle rect)
                {
                        // FIXME: Totally temporary! (unnefficient)
                        RecalculateUnits ((double) modelRoot.Timeline.ZoomLevel);
                        
                        // Draw the background
                        gr.Window.DrawRectangle (gr.Style.LightGC (StateType.Insensitive), true,
                                                 rect);
                        
                        // FIXME: Why should have separate rects here for various elements to 
                        // check what we really need to draw
                        // FIXME: Why rect is not working here?! Why we need mainRect?!
                        // FIXME: This stuff needs to definitely go into the unmanaged code. It's a
                        //        serious performance bottleneck
                        
                        // Now for the ticks...
                        
                        Time currentPos;
                        TimeSpan timelineSpan = modelRoot.Timeline.DisplayedTimeSpan;
                        
                        // Small ticks
                        if (smallUnit.NSeconds != 0) {
                                currentPos = timelineSpan.Start.Round (smallUnit);

                                if (currentPos < timelineSpan.Start) 
                                        currentPos += smallUnit;

                                while (currentPos < timelineSpan.End) {
                                        int x = modelRoot.Timeline.TimeToPixel (currentPos);
                                        
                                        if (x >= rect.Left && x <= rect.Right) {
                                                gr.Window.DrawLine (gr.Style.ForegroundGC (StateType.Normal),
                                                                    x, DrawingRect.Bottom, 
                                                                    x, DrawingRect.Bottom - 2);
                                        }
                                        currentPos += smallUnit;
                                }
                        }
                        
                        // Big ticks
                        if (bigUnit.NSeconds != 0) {
                                currentPos = timelineSpan.Start.Round (bigUnit);

                                if (currentPos < timelineSpan.Start) 
                                        currentPos += bigUnit;

                                while (currentPos < timelineSpan.End) {
                                        int x = modelRoot.Timeline.TimeToPixel (currentPos);
                                        
                                        if (x >= rect.Left && x <= rect.Right) {
                                                gr.Window.DrawLine (gr.Style.ForegroundGC (StateType.Normal), 
                                                                    x, DrawingRect.Bottom,
                                                                    x, DrawingRect.Bottom - 5);
                                        }
                                        currentPos += bigUnit;
                                }
                        }
                        
                        // Strings - this is tricky!
                        if (stringUnit.NSeconds != 0) {
                                currentPos = timelineSpan.Start.Round (stringUnit);
                                
                                if (currentPos < timelineSpan.Start) 
                                        currentPos += stringUnit;
                                
                                while (currentPos < timelineSpan.End) {
                                        int ww = TimeFu.ToPixels (stringUnit, (double) modelRoot.Timeline.ZoomLevel);
                                        int x = modelRoot.Timeline.TimeToPixel (currentPos);
                                        
                                        Gdk.Rectangle tempRect = new Gdk.Rectangle (x, DrawingRect.Top, 
                                                                                    ww, DrawingRect.Bottom);
                                        
                                        if (tempRect.IntersectsWith (rect) && x > DrawingRect.Left) {
                                                x -= ww / 2;
                                                Pango.Layout layout = new Pango.Layout (gr.Context);
                                                layout.Width = Pango.Units.FromPixels (ww);
                                                layout.Alignment = Pango.Alignment.Center;
                                                layout.Ellipsize = Pango.EllipsizeMode.End;
                                                layout.SetText (TimeFu.ToSMPTE (currentPos,
                                                                                modelRoot.ProjectDetails.Format.VideoFormat.Fps,
                                                                                false));
                                                                                                
                                                Pango.FontDescription fD = new Pango.FontDescription(); 
                                                fD.Size = Pango.Units.FromPixels (7);
                                                layout.FontDescription = fD;
                                                
                                                gr.Window.DrawLayout (gr.Style.TextGC (StateType.Normal),
                                                                      x, DrawingRect.Top - 1,
                                                                      layout);
                                                
                                        }
                                        
                                        currentPos += stringUnit;
                                }
                        }

                        base.Draw (gr, rect);
                }

                // Private methods /////////////////////////////////////////////

                void RecalculateUnits (double zoomLevel)
                {
                        // Single frame view
                        if (zoomLevel >= 250) {
                                smallUnit = Gdv.Time.OneSecond / 25;
                                bigUnit = Gdv.Time.OneSecond;
                                stringUnit = Gdv.Time.OneSecond * 2;
                                return;
                        }

                        if (zoomLevel >= 25) {
                                smallUnit = Gdv.Time.OneSecond;
                                bigUnit = Gdv.Time.OneSecond * 2;
                                stringUnit = Gdv.Time.OneSecond * 3;
                                return;
                        }

                        if (zoomLevel >= 8) {
                                smallUnit = Gdv.Time.OneSecond * 2;
                                bigUnit = Gdv.Time.OneSecond * 4;
                                stringUnit = Gdv.Time.OneSecond * 10;
                                return;
                        }

                        // Really wide mode
                        smallUnit = Gdv.Time.OneSecond * 1;
                        bigUnit = Gdv.Time.OneSecond * 10;
                        stringUnit = Gdv.Time.OneSecond * 20;
                        return;
                }
                
        }

}


