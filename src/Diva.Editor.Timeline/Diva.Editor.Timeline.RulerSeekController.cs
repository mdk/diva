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

        public class RulerSeekController : CursorController, ILeftClickController {
                
                // Fields //////////////////////////////////////////////////////
                
                protected ViewElement baseElement = null;

                // Properties //////////////////////////////////////////////////
                
                public override Gdk.Rectangle ActivationRect {
                        get { return baseElement.DrawingRect; }
                }

                public override ViewElement BoundElement {
                        get { return baseElement; } 
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public RulerSeekController (Model.Root modelRoot, ViewElement baseElement) :
                base (modelRoot, ElementLayer.RulerController, new Gdk.Cursor (Gdk.CursorType.Crosshair))
                {
                        this.baseElement = baseElement;
                }

                public void LeftClick (int x, int y)
                {
                        modelRoot.Pipeline.Seek (modelRoot.Timeline.MouseOverTime);
                }
                
        }
        
}