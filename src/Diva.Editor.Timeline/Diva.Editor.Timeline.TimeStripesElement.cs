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
        using Gtk;
        using Gdk;
        using Diva;
        using Gdv;
        using Cairo;
        using Util;
        using TimeSpan = Gdv.TimeSpan;

        public class TimeStripesElement : ViewElement {

                // Fields //////////////////////////////////////////////////////

                Time width;
                int pixelWidth;
                Gdv.Color color;
                
                // Properties //////////////////////////////////////////////////
                
                public override Rectangle DrawingRect {
                        get { 
                                Gdk.Rectangle full = modelRoot.Timeline.ViewportRectangle;
                                return new Gdk.Rectangle (full.Left, full.Top + 16,
                                                          full.Width, full.Height - 16);
                        }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TimeStripesElement (Model.Root modelRoot) :
                base (modelRoot, ElementLayer.Stripes)
                {
                }

                public override void Draw (Graphics gr, Rectangle rect)
                {
                        ReCalculate ((double) modelRoot.Timeline.ZoomLevel);

                        TimeSpan timelineSpan = modelRoot.Timeline.DisplayedTimeSpan;
                        Time currentPos = timelineSpan.Start.Round (width * 2);
                        
                        while (currentPos < timelineSpan.End) {

                                int x = 0;
                                int ww = 0;

                                if (currentPos < timelineSpan.Start) {
                                        
                                        Time cutted = timelineSpan.Start - currentPos;
                                        int tempWidth = TimeFu.ToPixels (cutted, (double) modelRoot.Timeline.ZoomLevel);
                                        if (tempWidth > pixelWidth) {
                                                currentPos += (width * 2);
                                                continue;
                                        } else {
                                                x = 0;
                                                ww = pixelWidth - tempWidth;
                                        }
                                } else {
                                        x = modelRoot.Timeline.TimeToPixel (currentPos);
                                        ww = pixelWidth;
                                }
                                
                                ReadyMade.TimeStripe (gr, x, ww, color, rect, DrawingRect);
                                currentPos += (width * 2);
                        }
                        
                        base.Draw (gr, rect);
                }

                // Private methods /////////////////////////////////////////////

                // FIXME: This should get moved to a separate helper clas. Esentially it's same
                // code as in Ruler element
                void ReCalculate (double zoomLevel)
                {
                        if (zoomLevel >= 250) {
                                width = Gdv.Time.OneSecond / 25;
                                color = Gdv.Color.FromTango (ColorTango.Orange2);
                        } else if (zoomLevel >= 25) {
                                width = Gdv.Time.OneSecond;
                                color = Gdv.Color.FromTango (ColorTango.SkyBlue2);
                        } else if (zoomLevel >= 8) {
                                width = Gdv.Time.OneSecond * 4;
                                color = Gdv.Color.FromTango (ColorTango.Plum2);
                        } else {
                                width = Gdv.Time.OneSecond * 10;
                                color = Gdv.Color.FromTango (ColorTango.Aluminium5);
                        }

                        color.Alpha = 0.15f;
                        pixelWidth = TimeFu.ToPixels (width, zoomLevel);
                }
                
        }
        
}