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

namespace Diva.Widgets {
        
        using System;
        using Gtk;
        using System.Collections;

        public sealed class EmptyModelController {
                
                // Events //////////////////////////////////////////////////////
                
                public event EmptyModelControllerEmptyHandler EmptyChange;
                
                public event EmptyModelControllerStateHandler StateChange;
                
                // Fields //////////////////////////////////////////////////////
                
                TreeModel model = null;    // Model interface we're controlling
                TreeView tree = null;      // Tree we're controlling
                bool trickSwitch = false;
                int count = 0;
                TreeIter emptyIter = TreeIter.Zero;
                
                // Properties //////////////////////////////////////////////////
                
                public bool Empty {
                        get { return (emptyIter.Stamp != TreeIter.Zero.Stamp); }
                }
                
                public bool HasSelection {
                        get {
                                TreeIter iter = TreeIter.Zero;
                                if (! tree.Selection.GetSelected (out iter))
                                        return false;
                                else
                                        return true;
                        }
                }
                
                public bool Valid {
                        get { 
                                return (! Empty) && HasSelection;
                        }
                }
                
                // Public fields ///////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public EmptyModelController (TreeModel model, TreeView tree)
                {
                        // We assume the model is empty during creation
                        this.model = model;
                        this.tree = tree;
                        
                        // Bind
                        model.RowInserted += OnRowInserted;
                        model.RowDeleted += OnRowDeleted;
                        tree.UnselectAll += OnUnselectAll;
                        tree.CursorChanged += OnCursorChanged;
                }
                
                public void Analyze ()
                {
                        TreeIter first = TreeIter.Zero;
                        
                        // Calculate the number of items
                        if (model.GetIterFirst (out first)) {
                                count = 1;
                                while (model.IterNext (ref first))
                                        count++;
                        } else
                                count = 0;
                        
                        // Decide 
                        if (count == 0) {
                                trickSwitch = true;
                                EmptyModelControllerEmptyArgs args = new EmptyModelControllerEmptyArgs (true, TreeIter.Zero);
                                if (EmptyChange != null)
                                        EmptyChange (this, args);
                                trickSwitch = false;
                                emptyIter = args.TreeIter;
                                EmitStateChange ();
                        }
                }
                
                public void EmitStateChange ()
                {
                        if (StateChange == null)
                                return;
                        
                        StateChange (this, new EmptyModelControllerStateArgs (Valid));
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnRowInserted (object o, RowInsertedArgs argss)
                {
                        if (trickSwitch)
                                return;
                        
                        count++;
                        
                        if (emptyIter.Stamp != TreeIter.Zero.Stamp) {
                                EmptyModelControllerEmptyArgs args = new EmptyModelControllerEmptyArgs (false, emptyIter);
                                trickSwitch = true;
                                if (EmptyChange != null)
                                        EmptyChange (this, args);
                                emptyIter = TreeIter.Zero;
                                trickSwitch = false;
                                EmitStateChange ();
                        }
                }
                
                void OnRowDeleted (object o, RowDeletedArgs argss)
                {
                        if (trickSwitch)
                                return;
                        
                        count--;
                        
                        if (count == 0) {
                                EmptyModelControllerEmptyArgs args = new EmptyModelControllerEmptyArgs (true, TreeIter.Zero);
                                trickSwitch = true;
                                if (EmptyChange != null)
                                        EmptyChange (this, args);
                                emptyIter = args.TreeIter;
                                trickSwitch = false;
                                EmitStateChange ();
                        }
                }
                
                void OnUnselectAll (object o, UnselectAllArgs args)
                {
                        EmitStateChange ();
                }
                
                void OnCursorChanged (object o, EventArgs args)
                {
                        EmitStateChange ();
                }
                                
        }
        
}
                
