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

        public abstract class CursorController : Element, IMouseFollowController {
                
                // Fields //////////////////////////////////////////////////////
                
                protected Gdk.Cursor cursor = null;
                protected long token = -1;

                // Properties //////////////////////////////////////////////////
                
                public abstract Gdk.Rectangle ActivationRect { get; }

                public abstract ViewElement BoundElement { get; }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CursorController (Model.Root modelRoot, ElementLayer layer, Gdk.Cursor cursor) :
                base (modelRoot, layer)
                {
                        this.cursor = cursor;
                }
                
                /* IMouseFollowController */
                public virtual void FollowMouse (int x, int y)
                {
                        if (ActivationRect.Contains (x, y) && token == -1)
                                token = modelRoot.Timeline.PushCursor (cursor, (int) Layer);
                        else if ((! ActivationRect.Contains (x, y)) && token != -1) {
                                modelRoot.Timeline.RemoveCursor (token);
                                token = -1;
                        }
                }
                
        }
        
}
