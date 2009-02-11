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
        using System.Collections.Generic;
        using Widgets;

        public sealed class HistoryTreeView : TreeView, IUnbindable {
                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null; // The model
                Gdk.Pixbuf undoIcon;         // Icon we're using for the undo
                Gdk.Pixbuf redoIcon;         // Icon we're using for the redo
                ListStore listStore = null;  // Our model
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public HistoryTreeView (Model.Root root) : base ()
                {
                        modelRoot = root;
                        HeadersVisible = false;
                        
                        // FIXME: Should be static? 
                        undoIcon = IconFu.GetStockIcon ("stock_undo", IconSize.Menu);
                        redoIcon = IconFu.GetStockIcon ("stock_redo", IconSize.Menu);
                        
                        // Load some initial data
                        RecreateModel ();

                        // Cell renderers
                        CellRendererPixbuf pixbufRenderer = new CellRendererPixbuf ();
                        CellRendererText textRenderer = new CellRendererText ();
                        
                        // Renderers and columns
                        TreeViewColumn clmn = new TreeViewColumn ();
                        clmn.PackStart (pixbufRenderer, false);
                        clmn.PackStart (textRenderer, true);
                        clmn.AddAttribute (pixbufRenderer, "pixbuf", 0);
                        clmn.AddAttribute (textRenderer, "text", 1);
                        clmn.AddAttribute (textRenderer, "underline", 2);
                        AppendColumn (clmn);
                        
                        // Model bind
                        modelRoot.CommandProcessor.UndoRedoChange += OnUndoRedoChanged;
                }
                
                public void Unbind ()
                {
                        modelRoot.CommandProcessor.UndoRedoChange -= OnUndoRedoChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                void RecreateModel ()
                {
                        listStore = new ListStore (typeof (Gdk.Pixbuf), 
                                                   typeof (string),
                                                   typeof (Pango.Underline),
                                                   typeof (int));
                        Model = listStore;
                        
                        // Redo list

                        List <string> lst = modelRoot.CommandProcessor.RedoStackMessages;
                        int redoCounter = lst.Count;
                        lst.Reverse ();
                        foreach (string str in lst) {
                                listStore.AppendValues (redoIcon, str,
                                                        Pango.Underline.None,
                                                        redoCounter);
                                redoCounter--;
                        }
                                                
                        // Undo list
                        int undoCounter = -1;
                        foreach (string str in modelRoot.CommandProcessor.UndoStackMessages) {
                                listStore.AppendValues (undoIcon, str,
                                                        (undoCounter == -1) ? Pango.Underline.Single :
                                                        Pango.Underline.None,
                                                        undoCounter);
                                undoCounter--;
                        }
                        
                }
                
                void OnUndoRedoChanged (object o, Model.UndoRedoArgs args)
                {
                        RecreateModel ();
                }
                
                protected override void OnRowActivated (TreePath path, TreeViewColumn clmn)
                {
                        TreeIter iter;
                        listStore.GetIterFromString (out iter, path.ToString ());
                        
                        int counter = (int) listStore.GetValue (iter, 3);
                        
                        if (counter > 0)
                                modelRoot.CommandProcessor.RedoMany (counter);
                        else
                                modelRoot.CommandProcessor.UndoMany (0 - counter);
                }
                
                
        }
        
}
       
