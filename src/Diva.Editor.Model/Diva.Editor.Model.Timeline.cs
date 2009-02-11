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

namespace Diva.Editor.Model {
        
        using System;
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;
        using Util;
        
        public sealed class Timeline : IModelPart {
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;
                TimeSpan displayedTimeSpan = TimeSpan.Empty;
                Time mouseOverTime = Time.Empty;
                CursorStack cursorStack = null;
                                                
                // Events /////////////////////////////////////////////////////
                
                public event TimeHandler MouseOverTimeChange;
                
                public event TimeSpanHandler DisplayedTimeSpanChange;
                
                public event RectangleHandler ViewportRectangleRequest;
                
                public event RectangleHandler InvalidationRequest;
                
                public event CursorHandler CursorChange;

                // Properties /////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                /* What's the timespan we're currently displaying */
                public TimeSpan DisplayedTimeSpan {
                        get { return displayedTimeSpan; }
                        set { 
                                displayedTimeSpan = value;
                                if (DisplayedTimeSpanChange != null)
                                        DisplayedTimeSpanChange (modelRoot, 
                                                                 new TimeSpanArgs (value));
                        }
                }
                
                /* Where is the mouse pointer */
                public Time MouseOverTime {
                        get { return mouseOverTime; }
                        set { 
                                mouseOverTime = value;
                                if (MouseOverTimeChange != null)
                                        MouseOverTimeChange (modelRoot, 
                                                             new TimeArgs (value));
                        }
                }
                
                /* What's the main area for drawing */
                public Gdk.Rectangle ViewportRectangle {
                        get {
                                Gdk.Rectangle rect = Gdk.Rectangle.Zero;
                                RectangleArgs args = new RectangleArgs (rect);
                                if (ViewportRectangleRequest != null)
                                        ViewportRectangleRequest (modelRoot, args);
                                
                                return args.Rectangle;
                        }
                }
                
                /* Zoom level in pixels per second (how many pixels one second is */
                public Gdv.Fraction ZoomLevel {
                        get {
                                return new Gdv.Fraction (ViewportRectangle.Width * 10000, 
                                                         (int) DisplayedTimeSpan.Duration.Seconds * 10000);
                        }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Timeline (Root root)
                {
                        modelRoot = root;
                        displayedTimeSpan = new TimeSpan (Time.FromSeconds (0), 
                                                          Time.FromSeconds (10));
                        mouseOverTime = Time.Empty;
                        ResetCursors ();
                }
                
                /* Reset the cursor stack. This ususally should happen after 
                 * tool change etc. */
                public void ResetCursors ()
                {
                        if (CursorChange != null)
                                CursorChange (modelRoot, new CursorArgs (null));
                        
                        cursorStack = new CursorStack ();
                        cursorStack.CursorChange += OnCursorChange;
                }
                
                public long PushCursor (Gdk.Cursor cursor, int layer)
                {
                        return cursorStack.PushCursor (cursor, layer);
                }
                
                public void RemoveCursor (long token)
                {
                        cursorStack.RemoveCursor (token);
                }
                
                /* Zoom by a given fraction */
                public void Zoom (int x, Fraction zoomVal)
                {
                        if (! AreBoundriesValid (zoomVal))
                                return;
                        
                        Time zoomCenter = PixelToTime (x);
                        Time newDuration = Time.FromNSeconds ((ulong) ((double) zoomVal * (ulong) displayedTimeSpan.Duration));
                        Time divisor = newDuration / 2;
                        Time leftCut = (zoomCenter >= divisor) ? divisor : zoomCenter;

                        TimeSpan newTimeSpan = new TimeSpan (zoomCenter - leftCut,
                                                             zoomCenter + divisor + (divisor - leftCut));
                        
                        DisplayedTimeSpan = newTimeSpan;
                }

                public void ZoomShifty (int x, Fraction zoomVal)
                {
                        if (! AreBoundriesValid (zoomVal))
                                return;
                        
                        Gdv.Fraction currentZoomLevel = ZoomLevel;
                        Time zoomCenter = PixelToTime (x);

                        Time newDuration = Time.FromNSeconds ((ulong) ((double) zoomVal * (ulong) displayedTimeSpan.Duration));
                        Gdv.Fraction newZoomLevel = new Gdv.Fraction (ViewportRectangle.Width * 10000, 
                                                                      (int) newDuration.Seconds * 10000);
                        
                        Time leftCut = TimeFu.FromPixels (x, (double) newZoomLevel);

                        if (leftCut > zoomCenter)
                                leftCut = zoomCenter;

                        Time newStart = zoomCenter - leftCut;
                        Time newEnd = zoomCenter + (newDuration - leftCut);
                        
                        DisplayedTimeSpan = new TimeSpan (newStart, newEnd);
                }
                
                /* Get time from a given pixel coord on timeline */
                public Time PixelToTime (int x)
                {
                        // FIXME: Something is broken here
                        Time t = TimeFu.FromPixels (x, (double) ZoomLevel);
                        return displayedTimeSpan.Start.RelativeTo (t);
                }
                
                /* Get the pixel from the given time on the timeline */
                public int TimeToPixel (Time t)
                {
                        // FIXME: Something is broken here
                        Time relTime = t.AbsoluteTo (displayedTimeSpan.Start);
                        return TimeFu.ToPixels (relTime, (double) ZoomLevel);
                }
                
                /* Request the given rectangle to be invalidated (redrawn) on the main 
                 * timeline area */
                public void InvalidateRectangle (Gdk.Rectangle rect)
                {
                        if (InvalidationRequest != null) {
                                RectangleArgs args = new RectangleArgs (rect);
                                InvalidationRequest (modelRoot, args);
                        }
                }

                /* Force the emission of the displayed timespan change. This might be emitted in
                 * example when the size of the window changes, etc. */
                public void ForceDisplayedTimespanChange ()
                {
                        if (DisplayedTimeSpanChange != null)
                                DisplayedTimeSpanChange (modelRoot, 
                                                         new TimeSpanArgs (displayedTimeSpan));
                }
                
                // Private methods /////////////////////////////////////////////
        
                void OnCursorChange (object o, CursorArgs args)
                {
                        if (CursorChange != null)
                                CursorChange (modelRoot, args);
                }

                bool AreBoundriesValid (Gdv.Fraction zoomVal)
                {
                        if ((double) zoomVal < 1.0 && (double) ZoomLevel > 250.0)
                                return false;

                        if ((double) zoomVal > 1.0 && (double) ZoomLevel < 5.0)
                                return false;

                        return true;
                }
                
        }
        
}
