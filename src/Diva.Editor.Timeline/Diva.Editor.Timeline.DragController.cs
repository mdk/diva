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

        public abstract class DragController : CursorController, IDragController {
                
                // Fields //////////////////////////////////////////////////////
                
                protected int dragStartX = -1;
                protected int dragStartY = -1;
                protected Time dragStartTime = Time.Empty;
                protected ViewElement baseElement = null;

                // Properties //////////////////////////////////////////////////
                
                public abstract Gdk.Rectangle DragRect { get; }
                
                public override Gdk.Rectangle ActivationRect {
                        get { 
                                return DragRect; 
                        }
                }

                public override ViewElement BoundElement {
                        get { return baseElement; }
                }

                public virtual bool LeftActivated {
                        get { return true; }
                }

                public virtual bool RightActivated {
                        get { return false; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public DragController (Model.Root modelRoot, ElementLayer layer, Gdk.Cursor cursor, ViewElement baseElement) :
                base (modelRoot, layer, cursor)
                {
                        this.cursor = cursor;
                        this.baseElement = baseElement;
                }
                
                /* IDragController */
                public virtual void DragStart (int x, int y)
                {
                        if (baseElement is IFloating)
                                (baseElement as IFloating).SaveThyself ();
                        
                        dragStartX = x;
                        dragStartY = y;
                        dragStartTime = modelRoot.Timeline.PixelToTime (x);
                        baseElement.State = ViewElementState.Drag;
                }
                
                /* IDragController */
                public virtual void DragStop (int x, int y)
                {
                        if (baseElement is IFloating) 
                                (baseElement as IFloating).SyncThyself ();
                        
                        baseElement.State = ViewElementState.Normal;
                        dragStartTime = Time.Empty;
                        dragStartX = -1;
                        dragStartY = -1;
                }
                
                /* IDragController */
                public virtual void DragAbort ()
                {
                        if (baseElement is IFloating)
                                (baseElement as IFloating).RestoreThyself ();
                }
                
                public abstract void DragMouse (int x, int y);
                                        
        }
        
}
