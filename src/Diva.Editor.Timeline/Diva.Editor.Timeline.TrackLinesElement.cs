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

        public class TrackLinesElement : ViewElement {
                
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
                public TrackLinesElement (Model.Root modelRoot) :
                base (modelRoot, ElementLayer.Background)
                {
                }

                public override void Draw (Graphics gr, Rectangle rect)
                {
                        // Draw using a nice ready made
                        ReadyMade.TimelineRect (gr, rect, DrawingRect);

                        // Draw the lines
                        // FIXME: TRACKS!
                        double y = DrawingRect.Top;
                        for (int i = 0; i < modelRoot.Tracks.Count; i++) {
                                if (y >= rect.Y && y <= rect.Y + rect.Height)
                                        // FIXME: I think the cairo-pixel correction causes a 1 px shift here 
                                        // henece the rect.X -1 thing that solves #66
                                        Cairo.Draw.Line (gr, rect.X - 1, y, rect.X + rect.Width, y, 
                                                         Gdv.Color.FromConst (ColorConst.Black), 1.0, true);
                                y += 31;
                        }
                        
                        base.Draw (gr, rect);
                }
                
        }
        
}
