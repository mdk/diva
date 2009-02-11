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
        using Util;
        using Mono.Unix;

        public class ChainAdjustController : DragController {

                readonly static string adjustMessageSS = Catalog.GetString 
                        ("Chain-adjust '{0}' and '{1}'");

                readonly static string adjustInstantMessageSS = Catalog.GetString 
                        ("'{0}' and '{1}' were chain-adjusted");

                // Fields //////////////////////////////////////////////////////

                ClipTrueElement leftElement;
                ClipTrueElement rightElement;
                bool hasTwinView;
                
                // Properties //////////////////////////////////////////////////
                
                public override Gdk.Rectangle DragRect {
                        get {
                                Gdk.Rectangle leftRectangle = leftElement.DrawingRect;
                                Gdk.Rectangle rightRectangle = rightElement.DrawingRect;

                                if (leftRectangle == Gdk.Rectangle.Zero ||
                                    rightRectangle == Gdk.Rectangle.Zero)
                                        return Gdk.Rectangle.Zero;

                                if (leftRectangle.Width <= 10 ||
                                    rightRectangle.Width <= 10)
                                        return Gdk.Rectangle.Zero;

                                return new Gdk.Rectangle (leftRectangle.Right - 10,
                                                          leftRectangle.Top, 
                                                          20,
                                                          leftRectangle.Height);
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ChainAdjustController (Model.Root modelRoot,
                                              ClipTrueElement leftElement, ClipTrueElement rightElement) :
                base (modelRoot, ElementLayer.ChainAdjust, new Gdk.Cursor (Gdk.CursorType.SbHDoubleArrow), leftElement)
                {
                        this.leftElement = leftElement;
                        this.rightElement = rightElement;
                        hasTwinView = false;
                }
                
                public override void DragMouse (int x, int y)
                {
                        int distance = dragStartX - x;
                        Time distanceTime = TimeFu.FromPixels (Math.Abs (distance), 
                                                               (double) modelRoot.Timeline.ZoomLevel);
                        
                        if (distance > 0) {
                                rightElement.CurrentLeftNoCheck = rightElement.SavedLeft - distanceTime;
                                leftElement.CurrentRightNoCheck = rightElement.CurrentLeftNoCheck;
                        } else {
                                leftElement.CurrentRightNoCheck = leftElement.SavedRight + distanceTime;
                                rightElement.CurrentLeftNoCheck = leftElement.CurrentRightNoCheck;
                        }

                        if (hasTwinView)
                                modelRoot.Pipeline.SeekTwinView (leftElement.Clip.ClipTimeToSourceTime(leftElement.CurrentRight),
                                                                 rightElement.Clip.ClipTimeToSourceTime(rightElement.CurrentLeft));
                }

                public override void DragStart (int x, int y)
                {
                        leftElement.SaveThyself ();
                        rightElement.SaveThyself ();
                        
                        dragStartX = x;
                        dragStartY = y;
                        dragStartTime = modelRoot.Timeline.PixelToTime (x);
                        
                        leftElement.State = ViewElementState.Drag;
                        rightElement.State = ViewElementState.Drag;

                        if (leftElement.Clip.HasVideo == false || rightElement.Clip.HasVideo == false)
                                return;
                        
                        Source src1 = leftElement.Clip.ParentItem.GetFreshSource ();
                        Source src2 = rightElement.Clip.ParentItem.GetFreshSource ();

                        modelRoot.Pipeline.StartTwinView (src1, src2, 0, 0);
                        hasTwinView = true;
                }
                
                public override void DragStop (int x, int y)
                {
                        Time left = leftElement.CurrentRight;
                        Time right = rightElement.CurrentLeft;
                        
                        leftElement.State = ViewElementState.Normal;
                        rightElement.State = ViewElementState.Normal;
                        
                        leftElement.RestoreThyself ();
                        rightElement.RestoreThyself ();

                        // Now prepare the command
                        Core.IUndoableCommand cmd1 = new Commands.RightAdjustClip (leftElement.Clip, left);
                        Core.IUndoableCommand cmd2 = new Commands.LeftAdjustClip (rightElement.Clip, right);

                        Commands.MultiCommand multiCmd = new Commands.MultiCommand
                                (String.Format (adjustMessageSS,
                                                leftElement.Clip.ParentItem.Name,
                                                rightElement.Clip.ParentItem.Name),
                                 String.Format (adjustInstantMessageSS,
                                                 leftElement.Clip.ParentItem.Name,
                                                rightElement.Clip.ParentItem.Name));

                        if (left < leftElement.CurrentRight) {
                                multiCmd.Add (cmd1);
                                multiCmd.Add (cmd2); 
                        } else {
                                multiCmd.Add (cmd2);
                                multiCmd.Add (cmd1);
                        }

                        modelRoot.CommandProcessor.PushCommand (multiCmd);

                        if (hasTwinView)
                                modelRoot.Pipeline.StopTwinView ();
                        
                        dragStartTime = Time.Empty;
                        dragStartX = -1;
                        dragStartY = -1;
                        hasTwinView = false;
                }
                
        }
        
}