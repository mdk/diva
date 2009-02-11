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
        using Gdv;
        using Mono.Unix;
        using System.Collections.Generic;
        
        public abstract class IntyComboBox : ComboBox {

                // Fields //////////////////////////////////////////////////////

                protected ListStore store;

                // Properties //////////////////////////////////////////////////

                public int ActiveValue {
                        get { 
                                TreeIter iter;
                                GetActiveIter (out iter);
                                return (int) store.GetValue (iter, 1);
                        }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public IntyComboBox (int def) : base ()
                {
                        // Create the store
                        store = new ListStore (typeof (string), typeof (int));
                        
                        // Set it
                        Model = store;

                        // Set the renderer
                        CellRendererText col1Renderer = new CellRendererText ();

                        PackStart (col1Renderer, false);
                        AddAttribute (col1Renderer, "text", 0);

                        InitializeDefaults ();

                        if (def != -1)
                                SelectByInt (def);
                }

                /* CONSTRUCTOR */
                public IntyComboBox () : this (-1)
                {
                }
                
                // Private methods /////////////////////////////////////////////

                /* Override this to fill the combo values */
                protected virtual void InitializeDefaults ()
                {
                }

                protected void SelectByInt (int val)
                {
                        TreeIter iter = Util.GtkFu.TreeModelIterByInt (store, val, 1);
                        if (iter.Stamp != TreeIter.Zero.Stamp)
                                SetActiveIter (iter);
                }

                protected void AddInt (string text, int val)
                {
                        store.AppendValues (text, val);
                }
                
        }

}