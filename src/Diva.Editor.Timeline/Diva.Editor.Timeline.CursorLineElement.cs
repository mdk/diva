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
        using Cairo;
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;

        public class CursorLineElement : ViewElement {

                // Fields //////////////////////////////////////////////////////

                int currentPixel;
                Time currentTime;
                
                // Properties //////////////////////////////////////////////////
                
                public override Gdk.Rectangle DrawingRect {
                        get {
                                // If out of view range, return zero
                                if (currentPixel == -1)
                                        return Gdk.Rectangle.Zero;
                                else 
                                        return new Gdk.Rectangle (currentPixel,
                                                                  16,
                                                                  1,
                                                                  modelRoot.Timeline.ViewportRectangle.Height);
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CursorLineElement (Model.Root modelRoot) :
                base (modelRoot, ElementLayer.TickerLine)
                {
                        currentPixel = -1;
                        currentTime = Time.Zero;
                                                
                        // Model bind
                        modelRoot.Pipeline.Ticker += OnTicker;
                        modelRoot.Pipeline.NextSeekTicker += OnNextSeekTicker;
                        modelRoot.Timeline.DisplayedTimeSpanChange += OnDisplayedTimeSpanChanged;
                }

                public override void Draw (Graphics gr, Gdk.Rectangle rect)
                {
                        Cairo.Draw.Line (gr, DrawingRect.Left, DrawingRect.Top, DrawingRect.Left, DrawingRect.Bottom,
                                         new Gdv.Color (0.9f, 0.0f, 0.0f, 1.0f), 1.0, true);

                        base.Draw (gr, rect);
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnTicker (object o, Model.PipelineTickerArgs args)
                {
                        currentTime = args.Time;
                        CalculatePixel (args.Time);
                }

                void OnNextSeekTicker (object o, Model.PipelineTickerArgs args)
                {
                        currentTime = args.Time;
                        CalculatePixel (args.Time);
                }

                void OnDisplayedTimeSpanChanged (object o, Model.TimeSpanArgs args)
                {
                        CalculatePixel (currentTime);
                }

                void CalculatePixel (Gdv.Time time)
                {
                        int newPixel;
                        
                        if (! modelRoot.Timeline.DisplayedTimeSpan.Contains (time)) 
                                newPixel = -1;
                        else   
                                newPixel = modelRoot.Timeline.TimeToPixel (time);
                        
                        if (newPixel != currentPixel) {
                                currentPixel = newPixel;
                                Invalidate ();
                        }
                }
                
        }
        
}
