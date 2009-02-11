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
        
        public class ViewElement : Element {
                
                // Fields //////////////////////////////////////////////////////
                                
                Gdk.Rectangle lastDrawingRect;     // Last draw area. For invalidations
                Gdk.Rectangle lastFullDrawingRect; // Last draw area of complete widget.
                                                   // For invalidations
                
                protected ViewElementState state; // Current state
                
                // Properties //////////////////////////////////////////////////
                
                /* Current area occupied by this element.
                 * Used for schedluing redraws */
                public virtual Gdk.Rectangle DrawingRect {
                        get { return Gdk.Rectangle.Zero; }
                }
                
                /* The state. Can be modyfied by controlling elements */
                public ViewElementState State {
                        get { return state; }
                        set { 
                                state = value;
                                Invalidate ();
                        }
                }
                
                protected CairoState CairoState {
                        get { 
                                switch (state) {
                                        
                                case ViewElementState.Active:
                                        return CairoState.Active;
                                        break;
                                
                                case ViewElementState.Drag:
                                        return CairoState.Drag;
                                        break;
                                
                                default:
                                        return CairoState.Normal;
                                        break;
                                }
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ViewElement (Model.Root modelRoot,ElementLayer layer) :
                base (modelRoot, layer)
                {
                        lastDrawingRect = Gdk.Rectangle.Zero;
                        state = ViewElementState.Normal;
                }

                /* Draw the current view element. The rect is the only part that
                 * needs to be redrawn */
                public virtual void Draw (Graphics gr, Gdk.Rectangle rect)
                {
                        lastDrawingRect = rect;
                        lastFullDrawingRect = DrawingRect;
                }
                
                /* Invalidate (for drawing) the area occupied by this element and 
                 * the area it's (was) drawn before. In other words -- call after
                 * modyfying the state of the element */
                public virtual void Invalidate ()
                {
                        // Invalidate previous drawing area
                        if (lastDrawingRect != Gdk.Rectangle.Zero) {
                                modelRoot.Timeline.InvalidateRectangle (lastDrawingRect);
                        }
                        
                        // Invalidate current area
                        if (DrawingRect != Gdk.Rectangle.Zero) {
                                modelRoot.Timeline.InvalidateRectangle (DrawingRect);
                        }

                        // Invalidate previous full drawing area
                        if (lastFullDrawingRect != Gdk.Rectangle.Zero) {
                                modelRoot.Timeline.InvalidateRectangle (lastFullDrawingRect);
                        }
                }

        }
        
}
