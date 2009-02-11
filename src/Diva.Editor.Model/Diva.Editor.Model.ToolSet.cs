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
        
        public sealed class ToolSet : IModelPart {
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;                      // App model
                ToolSetTool currentTool = ToolSetTool.Edit; // Selected tool
                ToolSetTool stackedTool = ToolSetTool.None; // Stacked tool 

                // Events /////////////////////////////////////////////////////
                
                public event ToolSetToolHandler SelectedToolChange;
                
                // Properties /////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public ToolSetTool SelectedTool {
                        get { return currentTool; }
                        set { 
                                if (value == currentTool)
                                        return;
                                
                                currentTool = value;
                                if (SelectedToolChange != null)
                                        SelectedToolChange (modelRoot, 
                                                            new ToolSetToolArgs (value));
                        }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ToolSet (Root root)
                {
                        modelRoot = root;
                }

                public void PushTool (ToolSetTool tool)
                {
                        stackedTool = SelectedTool;
                        SelectedTool = tool;
                }

                public ToolSetTool PopTool ()
                {
                        if (stackedTool == ToolSetTool.None)
                                return ToolSetTool.None;

                        SelectedTool = stackedTool;
                        stackedTool = ToolSetTool.None;
                        
                        return SelectedTool;
                }
                
                // Private methods /////////////////////////////////////////////
                
                
        }
        
}
