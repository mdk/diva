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
        
        public sealed class EditChainTool : DragTool {
                
                // Fields //////////////////////////////////////////////////////

                List <ClipTrueElement> clipElementList;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public EditChainTool (Model.Root root) :
                base (root)
                {
                        clipElementList = new List <ClipTrueElement> ();
                }
                
                public override void NewViewElement (ViewElement viewElement)
                {
                        SpawnHighlight (viewElement, false);
                        SpawnTickerController (viewElement);
                        
                        if (viewElement is ClipTrueElement) {

                                ClipTrueElement leftElement = null;
                                ClipTrueElement rightElement = null;

                                if (ScanForMatching (viewElement as ClipTrueElement,
                                                     out leftElement,
                                                     out rightElement)) {
                                        ChainAdjustController ctrl = new ChainAdjustController (modelRoot,
                                                                                                leftElement,
                                                                                                rightElement);
                                        controllerList.Add (ctrl);
                                }

                                clipElementList.Add (viewElement as ClipTrueElement);
                        }

                        /*
                        spawnhighlight (viewElement, true);
                        SpawnTickerController (viewElement);
                        
                        if (viewElement is ClipElement)
                                HandleClipElement (viewElement as ClipElement);
                                
                        if (viewElement is TrackLinesElement) {
                                PanAreaController ctrl = new PanAreaController (modelRoot, viewElement);
                                ctrl.Hidden = true;
                                controllerList.Add (ctrl);
                                }*/
                }
                
                // Private methods /////////////////////////////////////////////

                void HandleClipElement (ClipElement viewElement)
                {
                        /*
                        Element moveController = new MoveAndTrackController (modelRoot, 
                                                                             viewElement);
                        controllerList.Add (moveController);
                                
                        if (viewElement is ClipTrueElement) {
                                Element menuController = new ClipMenuController (modelRoot, 
                                                                                 viewElement as ClipTrueElement);
                                controllerList.Add (menuController);

                                Element leftAdjust = new LeftAdjustController (modelRoot, 
                                                                               viewElement as ClipTrueElement);
                                controllerList.Add (leftAdjust);
                                
                                Element rightAdjust = new RightAdjustController (modelRoot, 
                                                                                 viewElement as ClipTrueElement);
                                controllerList.Add (rightAdjust);
                        }
                        */
                }

                bool ScanForMatching (ClipTrueElement element, out ClipTrueElement leftElement,
                                      out ClipTrueElement rightElement)
                {
                        foreach (ClipTrueElement e in clipElementList) {
                                                   
                                if (e.Clip.TimelineSpan.End == element.Clip.TimelineSpan.Start) {
                                        leftElement = e;
                                        rightElement = element;
                                        return true;
                                }

                                if (e.Clip.TimelineSpan.Start == element.Clip.TimelineSpan.End) {
                                        leftElement = element;
                                        rightElement = e;
                                        return true;
                                }
                        }

                        leftElement = null;
                        rightElement = null;
                        return false;
                }

        }
        
}