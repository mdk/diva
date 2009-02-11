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
        using Mono.Unix;
        using Gtk;
        using Util;
        using Widgets;

        public class HistoryWindow : Gtk.Window, IUnbindable {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("History");

                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                
                VBox mainVBox = null;
                HButtonBox buttonBox = null;
                HistoryTreeView treeView = null;
                ScrolledWindow treeScrolledWindow = null;
                                
                // Properties //////////////////////////////////////////////////

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public HistoryWindow (Model.Root root, Window parent) : base (titleSS)
                {
                        BorderWidth = 6;
                        modelRoot = root;
                        TransientFor = parent;
                        
                        // Button box
                        buttonBox = new HButtonBox ();
                        Button helpButton = new Button (Stock.Help);
                        Button closeButton = new Button (Stock.Close);
                        closeButton.Clicked += OnCloseClicked;
                        buttonBox.Add (helpButton);
                        buttonBox.Add (closeButton);
                        buttonBox.Spacing = 6;
                        
                        // TreeView
                        treeView = new HistoryTreeView (root);

                        // Scrolled
                        treeScrolledWindow = new ScrolledWindow ();
                        treeScrolledWindow.Add (treeView);
                        treeScrolledWindow.ShadowType = ShadowType.In;
                        treeScrolledWindow.VscrollbarPolicy = PolicyType.Automatic;
                        treeScrolledWindow.HscrollbarPolicy = PolicyType.Never;
                                                
                        // MainVBox
                        mainVBox = new VBox (false, 12);
                        mainVBox.PackStart (treeScrolledWindow, true, true, 0);
                        mainVBox.PackEnd (buttonBox, false, false, 0);
                        
                        Add (mainVBox);
                        
                        GtkFu.ParseGeometry (this, Config.Ui.HistoryWindowGeometry);
                        ShowAll ();
                }
                
                public void Unbind ()
                {
                        treeView.Unbind ();
                }
                
                public void SaveGeometry ()
                {
                        Config.Ui.HistoryWindowGeometry = GtkFu.GetGeometry (this);
                }

                // Private methods /////////////////////////////////////////////
                
                void OnCloseClicked (object o, EventArgs args)
                {
                        modelRoot.Window.HideHistoryWindow ();
                }
                
                protected override bool OnDeleteEvent (Gdk.Event evnt)
                {
                        modelRoot.Window.HideHistoryWindow ();
                        return true;
                }

        }
        
}

