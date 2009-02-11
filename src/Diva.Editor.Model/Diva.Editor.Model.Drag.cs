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

namespace Diva.Editor.Model {
        
        using System;
        using System.Collections;
        
        public sealed class Drag : IModelPart {
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;
                DragStatus status = DragStatus.Idle;
                object draggedObject = null;
                                
                // Events /////////////////////////////////////////////////////
                
                // Properties /////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public DragStatus Status {
                        get { return status; }
                }
                
                public object DraggedObject { 
                        get { return draggedObject; }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Drag (Root root)
                {
                        modelRoot = root;
                }
                
                public void Start (object o)
                {
                        if (status != DragStatus.Idle)
                                throw new Exception ("Drag already in progress");
                        
                        status = DragStatus.Dragging;
                        draggedObject = o;
                }
                
                public object Claim ()
                {
                        status = DragStatus.Claimed;
                        return draggedObject;
                }
                
                public void UnClaim ()
                {
                        if (status != DragStatus.Claimed)
                                throw new Exception ("Drag not claimed");
                        
                        status = DragStatus.Dragging;
                }
                
                public void SoftStop ()
                {
                        status = DragStatus.Idle;
                        draggedObject = null;
                }
                
        }
        
}
