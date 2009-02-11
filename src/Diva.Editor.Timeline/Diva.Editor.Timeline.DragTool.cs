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
        using System.Collections.Generic;
        
        public abstract class DragTool : Tool {

                // Fields //////////////////////////////////////////////////////
                
                protected bool dragMode = false;
                protected IDragController dragController = null;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public DragTool (Model.Root root) :
                base (root)
                {
                }
                
                public override void Mouse (int x, int y)
                {
                        if (dragMode)
                                dragController.DragMouse (x, y);
                        else
                                FollowControllers (x, y);
                }

                public override void LeftPress (int x, int y)
                {
                        DragPress (x, y, true, false);
                }
                
                public override void LeftRelease (int x, int y)
                {
                        DragRelease (x, y);
                        LeftClicks (x, y);
                }

                public override void RightPress (int x, int y)
                {
                        DragPress (x, y, false, true);
                }

                public override void RightRelease (int x, int y)
                {
                        DragRelease (x, y);
                        RightClicks (x, y);
                }

                // Private methods /////////////////////////////////////////////

                /* Spawn standard drag and click controllers for the cursor ticker */
                protected void SpawnTickerController (ViewElement viewElement)
                {
                        // Move controller for cursor
                        if (viewElement is CursorElement) {
                                Element moveController = new MoveController (modelRoot, 
                                                                             viewElement);
                                controllerList.Add (moveController);
                        }

                        // Click-seek controller for ruler
                        if (viewElement is RulerElement) {
                                Element rulerSeekController = new RulerSeekController (modelRoot,
                                                                                       viewElement);
                                controllerList.Add (rulerSeekController);
                        }                                        
                }

                protected void DragPress (int x, int y, bool leftmatch, bool rightmatch)
                {
                        if (dragMode)
                                return;
                        
                        foreach (Element element in controllerList) 
                                if (element is IDragController) {
                                        
                                        if (leftmatch == true && (element as IDragController).LeftActivated == false)
                                                continue;
                                        
                                        if (rightmatch == true && (element as IDragController).RightActivated == false)
                                                continue;
                                        
                                        if ((element as IDragController).DragRect.Contains (x, y)) {
                                                dragMode = true;
                                                dragController = (element as IDragController);
                                                break;
                                        }
                                }
                        
                        if (dragMode) 
                                dragController.DragStart (x, y);
                }

                protected void DragRelease (int x, int y)
                {
                        if (dragController == null)
                                return;

                        /*
                        {
                                // This is a "normal" left click, let's try lunnching
                                // a left click controller
                                foreach (Element element in controllerList)
                                        if (element is ILeftClickController) {
                                                ILeftClickController ctrl = element as ILeftClickController;
                                                if (ctrl.ActivationRect.Contains (x, y))
                                                        ctrl.LeftClick (x, y);
                                        }
                                return;
                                }*/
                        
                        dragController.DragStop (x, y);
                        dragMode = false;
                        dragController = null;
                        
                        FollowControllers (x, y);
                }
                
        }
        
}