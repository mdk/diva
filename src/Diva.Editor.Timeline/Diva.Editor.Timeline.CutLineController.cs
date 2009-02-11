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

        public class CutLineController : CursorController, ILeftClickController {
                
                // Fields //////////////////////////////////////////////////////
                
                CutLineElement cutLine = null;
                ViewElement controlledElement = null;

                // Properties //////////////////////////////////////////////////

                public override Gdk.Rectangle ActivationRect {
                        get { return controlledElement.DrawingRect; }
                }

                public override ViewElement BoundElement {
                        get { return controlledElement; }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CutLineController (Model.Root modelRoot, ViewElement element, CutLineElement cutLine) :
                        base (modelRoot, element.Layer, new Gdk.Cursor (Gdk.CursorType.Crosshair))
                {
                        this.controlledElement = element;
                        this.cutLine = cutLine;
                }

                public override void FollowMouse (int x, int y)
                {
                        // We assume that the controlling tool resets the cut line on each mouse move
                        if (controlledElement.DrawingRect.Contains (x, y)) 
                                cutLine.Time = modelRoot.Timeline.MouseOverTime;

                        base.FollowMouse (x, y);
                }

                public void LeftClick (int x, int y)
                {
                        ClipTrueElement elem = controlledElement as ClipTrueElement;
                        Gdv.Time t = modelRoot.Timeline.MouseOverTime;
                        
                        Core.ICommand cmd = new Commands.CutClip (elem.Clip, t);
                        modelRoot.CommandProcessor.StackCommand (cmd);
                }
                
        }
        
}