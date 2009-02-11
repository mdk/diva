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

        public class HighlightController : Element, IMouseFollowController {
                
                // Fields //////////////////////////////////////////////////////
                
                ViewElement controlledElement = null;

                // Properties //////////////////////////////////////////////////

                public ViewElement BoundElement {
                        get { return controlledElement; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public HighlightController (Model.Root modelRoot, ViewElement element) :
                base (modelRoot, element.Layer)
                {
                        controlledElement = element;
                }
                
                /* IMouseFollowController . Sets the state of the controlled view 
                 * element accordingly. Changing the state forces the element's 
                 * invalidation (redraw) */
                public void FollowMouse (int x, int y)
                {
                        ViewElementState state = controlledElement.State;
                        
                        if (state == ViewElementState.Normal && 
                            controlledElement.DrawingRect.Contains (x, y))
                                controlledElement.State = ViewElementState.Active;
                        
                        if (state == ViewElementState.Active &&
                            (! controlledElement.DrawingRect.Contains (x, y)))
                                controlledElement.State = ViewElementState.Normal;
                }
                
        }
        
}
