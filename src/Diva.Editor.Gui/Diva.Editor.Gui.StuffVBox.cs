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
        using Mono.Unix;
        using Widgets;
        using System.Collections.Generic;
        using Util;

        public sealed class StuffVBox : VBox {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string organizeSS = Catalog.GetString
                        ("Organize");

                readonly static string addButtonSS = Catalog.GetString
                        ("Add or import new files to the project");

                // Fields /////////////////////////////////////////////////////
                
                HBox topHBox = null;                      // "Organize by"
                Button addButton = null;                  // Add button
                Model.Root modelRoot = null;              // App model
                StuffTreeView stuffTreeView = null;       // The tree
                ScrolledWindow treeScrolledWindow = null; // Scroller for the tree

                long addMessageToken = -1;

                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public StuffVBox (Model.Root modelRoot) : base (false, 6)
                {
                        this.modelRoot = modelRoot;
                        
                        // Add button
                        addButton = new Button (Stock.Add);
                        addButton.Clicked += OnAddClicked;
                        addButton.Entered += OnAddButtonEntered;
                        addButton.Left += OnAddButtonLeft;
                        
                        // Top Hbox
                        topHBox = new HBox (false, 6);
                        topHBox.PackStart (new Label (organizeSS), false, false, 0);
                        topHBox.PackStart (new OrganizeByComboBox(modelRoot), true, true, 0);
                        topHBox.PackStart (addButton, false, false, 0);
                        
                        // Tree
                        stuffTreeView = new StuffTreeView (modelRoot);
                        
                        // Scrolled
                        treeScrolledWindow = new ScrolledWindow ();
                        treeScrolledWindow.Add (stuffTreeView);
                        treeScrolledWindow.ShadowType = ShadowType.In;
                        treeScrolledWindow.VscrollbarPolicy = PolicyType.Automatic;
                        treeScrolledWindow.HscrollbarPolicy = PolicyType.Never;
                        
                        // Pack
                        PackStart (topHBox, false, false, 0);
                        PackStart (treeScrolledWindow, true, true, 0);
                        PackEnd (new MediaControlsHBox (modelRoot), false, false, 0);

                }
                
                // Private methods ////////////////////////////////////////////
                
                /* "Add" was clicked in the organizer */
                void OnAddClicked (object sender, EventArgs args)
                {
                        OpenMediaDialog openDialog = null;
                        
                        openDialog = new OpenMediaDialog (null);
                        string lastPathUsed = Config.Projects.LastAddMediaPath;
                        if (lastPathUsed != String.Empty &&
                            IOFu.DirPresent (lastPathUsed))
                                openDialog.SetCurrentFolder (lastPathUsed);
                        
                        if (openDialog.Run () == (int) ResponseType.Accept)
                        {
                                AddMediaFiles (openDialog.Filenames);
                                Config.Projects.LastAddMediaPath = openDialog.CurrentFolder;
                        }                
                        openDialog.Destroy ();
                }
                
                void AddMediaFiles (string[] fileNames)
                {
                        if (fileNames.Length == 0)
                                return;
                        
                        List <string> files = new List <string> ();
                        
                        foreach (string file in fileNames) {
                                if (modelRoot.MediaItems.HasFileName (file)) {
                                        // Message dialog
                                        Dialog dialog = new AlreadyImportedDialog (null, file);
                                        int resp = dialog.Run ();
                                        dialog.Destroy ();
                                        
                                        // Operation canceled
                                        if (resp == (int) ResponseType.Cancel)
                                                return;
                                        
                                        // Import anyways
                                        if (resp == 200)
                                                files.Add (file);
                                        
                                        // For skip we do nothing
                                                                                
                                } else 
                                        files.Add (file);
                        }
                        
                        if (files.Count > 0)
                                modelRoot.Stuff.AddMediaFiles ((string []) files.ToArray ());
                }

                void OnAddButtonEntered (object o, EventArgs args)
                {
                        if (addMessageToken != -1) {
                                modelRoot.Window.PopMessage (addMessageToken);
                                addMessageToken = -1;
                        }
                        
                        addMessageToken = modelRoot.Window.PushMessage (addButtonSS, Editor.Model.MessageLayer.Widget2);
                }
                
                void OnAddButtonLeft (object o, EventArgs args)
                {
                        if (addMessageToken != -1) {
                                modelRoot.Window.PopMessage (addMessageToken);
                                addMessageToken = -1;
                        }
                }

        }
        
}
       
