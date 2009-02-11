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
        using System.Collections.Generic;
        
        public abstract class Tool {
                
                // Fields //////////////////////////////////////////////////////
                
                protected ElementList controllerList; // The controller elements
                protected Model.Root modelRoot;       // App model

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Tool (Model.Root root)
                {
                        controllerList = new ElementList ();
                        controllerList.Reversed = true;
                        modelRoot = root;
                }
                                
                public abstract void Mouse (int x, int y);
                
                public abstract void LeftPress (int x, int y);
                
                public abstract void LeftRelease (int x, int y);

                public abstract void RightPress (int x, int y);
                
                public abstract void RightRelease (int x, int y);
                
                public virtual List <Element> SpawnCustomElements (ElementList viewElementsList)
                {
                        return null;
                }
                
                public abstract void NewViewElement (ViewElement element);
                
                public virtual void RemovedViewElement (ViewElement removedElement)
                {
                        // Let's simply find and remove all controllers
                        // referring to this element
                        List <Element> forRemoval = new List <Element> ();
                        
                        foreach (Element element in controllerList) {
                                if (element is IController) {
                                        IController ctrl = element as IController;
                                        if (ctrl.BoundElement == removedElement)
                                                forRemoval.Add (element);
                                }
                        }

                        foreach (Element element in forRemoval) {
                                // FIXME: Temporary. We should have a reset func 
                                if (element is IMouseFollowController) {
                                        IMouseFollowController ctrl = element as IMouseFollowController;
                                        ctrl.FollowMouse (-1000, -1000);
                                }
                                controllerList.Remove (element);
                        }
                }
                
                // Private methods /////////////////////////////////////////////
                
                /* Execute the follow function with the given x, y on all
                 * mouse following controllers */
                protected void FollowControllers (int x, int y)
                {
                        foreach (Element e in controllerList) 
                                if (e is IMouseFollowController)
                                        (e as IMouseFollowController).FollowMouse (x, y);
                }

                /* Execute the right click function on the first right click
                 * controller -- if found */
                protected void RightClicks (int x, int y)
                {
                        foreach (Element e in controllerList) 
                                if (e is IRightClickController) {
                                        IRightClickController ctrl = e as IRightClickController;
                                        if (ctrl.ActivationRect.Contains (x, y)) {
                                                    ctrl.RightClick (x, y);
                                                    return;
                                        }
                                }
                }

                protected void LeftClicks (int x, int y)
                {
                        foreach (Element e in controllerList) 
                                if (e is ILeftClickController) {
                                        ILeftClickController ctrl = e as ILeftClickController;
                                        if (ctrl.ActivationRect.Contains (x, y)) {
                                                    ctrl.LeftClick (x, y);
                                                    return;
                                        }
                                }
                }
                
                protected void SpawnHighlight (ViewElement viewElement, bool clips)
                {
                        if ((viewElement is ClipElement && clips) || 
                            viewElement is CursorElement) {
                                Element newController = new HighlightController (modelRoot, 
                                                                                 viewElement);
                                controllerList.Add (newController);
                        }
                }

        }
        
}
