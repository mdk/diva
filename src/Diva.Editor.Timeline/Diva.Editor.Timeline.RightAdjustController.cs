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

        public class RightAdjustController : DragController {

                // Properties //////////////////////////////////////////////////
                
                public override Gdk.Rectangle DragRect {
                        get {
                                if (baseElement.DrawingRect.Width > 20)
                                        return new Gdk.Rectangle (baseElement.DrawingRect.Right - 10,
                                                                  baseElement.DrawingRect.Top,
                                                                  10,
                                                                  baseElement.DrawingRect.Height);
                                else
                                        return Gdk.Rectangle.Zero;
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public RightAdjustController (Model.Root modelRoot, ViewElement element) :
                base (modelRoot, ElementLayer.LeftRightAdjust, new Gdk.Cursor (Gdk.CursorType.SbRightArrow), element)
                {
                }
                
                public override void DragMouse (int x, int y)
                {
                        IRightAdjustable adjustable = baseElement as IRightAdjustable;
                        
                        int distance = dragStartX - x;
                        Time distanceTime = TimeFu.FromPixels (Math.Abs (distance), 
                                                               (double) modelRoot.Timeline.ZoomLevel);
                        
                        if (distance > 0)
                                adjustable.CurrentRight = adjustable.SavedRight - distanceTime;
                        else
                                adjustable.CurrentRight = adjustable.SavedRight + distanceTime;

                        ClipTrueElement ele = baseElement as ClipTrueElement;
                        if (ele.Clip.HasVideo)
                                modelRoot.Pipeline.SeekSingleView (ele.Clip.ClipTimeToSourceTime(adjustable.CurrentRight));
                }
                
                public override void DragStart (int x, int y)
                {
                        base.DragStart (x, y);

                        ClipTrueElement ele = baseElement as ClipTrueElement;

                        if (! ele.Clip.HasVideo)
                                return;
                        
                        Source src = ele.Clip.ParentItem.GetFreshSource ();

                        modelRoot.Pipeline.StartSingleView (src, ele.Clip.SourceSpan.End);
                }

                public override void DragStop (int x, int y)
                {
                        ClipTrueElement ele = baseElement as ClipTrueElement;
                        
                        if (ele.Clip.HasVideo)
                                modelRoot.Pipeline.StopSingleView ();
                        
                        base.DragStop (x, y);
                }
                
        }
        
}