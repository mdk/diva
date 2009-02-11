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
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;
        using Util;

        public sealed class PanAreaController : DragController {

                // Fields //////////////////////////////////////////////////////

                bool hidden;
                
                // Properties //////////////////////////////////////////////////
                
                public override Gdk.Rectangle DragRect {
                        get {
                                return baseElement.DrawingRect;
                        }
                }

                public bool Hidden {
                        get { return hidden; }
                        set { hidden = value; }
                }

                public override bool LeftActivated {
                        get {
                                if (hidden)
                                        return false;
                                else
                                        return true;
                        }
                }

                public override bool RightActivated {
                        get { return true; }
                }
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public PanAreaController (Model.Root modelRoot, ViewElement baseElement) :
                        base (modelRoot, ElementLayer.PanArea, new Gdk.Cursor (Gdk.CursorType.Fleur), baseElement)
                {
                        hidden = false;
                }
                
                public override void DragStart (int x, int y)
                {
                        dragStartX = x;
                        dragStartY = y;
                        dragStartTime = modelRoot.Timeline.DisplayedTimeSpan.Start;

                        if (hidden)
                                token = modelRoot.Timeline.PushCursor (cursor, (int) Layer);
                }
                
                public override void DragStop (int x, int y)
                {
                        dragStartTime = Time.Empty;
                        dragStartX = -1;
                        dragStartY = -1;

                        if (hidden && token != -1) {
                                modelRoot.Timeline.RemoveCursor (token);
                                token = -1;
                        }
                }
                
                public override void DragAbort ()
                {
                        if (hidden && token != -1) {
                                modelRoot.Timeline.RemoveCursor (token);
                                token = -1;
                        }
                }
                
                public override void DragMouse (int x, int y)
                {
                        int distance = dragStartX - x;
                        Time distanceTime = TimeFu.FromPixels (Math.Abs (distance), 
                                                               (double) modelRoot.Timeline.ZoomLevel);

                        TimeSpan currentSpan = modelRoot.Timeline.DisplayedTimeSpan;
                        
                        if (distance > 0) {
                                if (dragStartTime < distanceTime)
                                        distanceTime = dragStartTime;

                                currentSpan.MoveTo (dragStartTime - distanceTime);
                                modelRoot.Timeline.DisplayedTimeSpan = currentSpan;
                                        
                        } else {
                                currentSpan.MoveTo (dragStartTime + distanceTime);
                                modelRoot.Timeline.DisplayedTimeSpan = currentSpan;
                        }
                }

                public override void FollowMouse (int x, int y)
                {
                        if (! hidden)
                                base.FollowMouse (x, y);
                }
                
                                        
        }
        
}