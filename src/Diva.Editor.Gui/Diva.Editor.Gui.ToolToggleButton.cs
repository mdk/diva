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

namespace Diva.Editor.Gui {
        
        using System;
        using Gtk;
        using Util;
        
        public class ToolToggleButton : ToggleButton {
                
                // Fields //////////////////////////////////////////////////////
                
                Model.ToolSetTool tool;      // Tool we're representing
                Model.Root modelRoot = null; //
                bool trickSwitch = false;    // To fight recursive events
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ToolToggleButton (Model.Root root, Gdk.Pixbuf pixbuf,
                                         bool bevel, Model.ToolSetTool tool)
                {
                        modelRoot = root;
                        this.tool = tool;
                        
                        Gtk.Image image = new Image (pixbuf);
                        Add (image);
                        CanFocus = false;
                        
                        Relief = (bevel == true) ? ReliefStyle.Normal : ReliefStyle.None;
                        
                        // Set the initial state 
                        trickSwitch = true;
                        if (root.ToolSet.SelectedTool == tool)
                                Active = true;
                        else
                                Active = false;
                        trickSwitch = false;
                        
                        // Model bind
                        modelRoot.ToolSet.SelectedToolChange += OnSelectedToolChanged;
                }
                
                /* CONSTRUCTOR */
                public ToolToggleButton (Model.Root root, string stock, 
                                         bool bevel, Model.ToolSetTool tool) :
                this (root, IconFu.GetStockIcon (stock, IconSize.Button),
                      bevel, tool)
                {
                }

                // Private methods /////////////////////////////////////////////
                
                void OnSelectedToolChanged (object o, Model.ToolSetToolArgs args)
                {
                        trickSwitch = true;
                        if (args.Tool == tool)
                                Active = true;
                        else 
                                Active = false;
                        trickSwitch = false;
                }
                
                protected override void OnToggled ()
                {
                        if (trickSwitch)
                                return;
                        
                        // Prevent the "untoggle" of the button
                        if (Active == false && modelRoot.ToolSet.SelectedTool == tool) {
                                trickSwitch = true;
                                Active = true;
                                trickSwitch = false;
                        }
                        
                        // This was a real click
                        modelRoot.ToolSet.SelectedTool = tool;
                }
                                
        }
        
}
       
