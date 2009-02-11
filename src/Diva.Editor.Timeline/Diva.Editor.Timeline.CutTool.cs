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
        
        public sealed class CutTool : DragTool {

                // Fields //////////////////////////////////////////////////////
                
                CutLineElement cutLine = null;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CutTool (Model.Root root) :
                base (root)
                {
                }
                
                public override void Mouse (int x, int y)
                {
                        if (dragMode)
                                dragController.DragMouse (x, y);
                        else {
                                cutLine.CheapReset ();
                                FollowControllers (x, y);
                        }
                }
                
                public override void NewViewElement (ViewElement viewElement)
                {
                        SpawnHighlight (viewElement, false);
                        SpawnTickerController (viewElement);
                        
                        if (viewElement is ClipElement)
                                HandleClipElement (viewElement as ClipElement);
                }

                public override List <Element> SpawnCustomElements (ElementList viewElementsList)
                {
                        List <Element> list = new List <Element> ();
                        cutLine = new CutLineElement (modelRoot);
                        list.Add (cutLine);
                        return list;
                }
                
                // Private methods /////////////////////////////////////////////

                void HandleClipElement (ClipElement viewElement)
                {
                        if (viewElement is ClipTrueElement) {
                                Element cutLineController = new CutLineController (modelRoot,
                                                                                   viewElement,
                                                                                   cutLine);
                                controllerList.Add (cutLineController);
                        }
                }
                
        }
        
}