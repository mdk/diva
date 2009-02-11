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
        using Util;

        public sealed class TagsWindow : Gtk.Window, IUnbindable {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("Tags");
                
                readonly static string isAppliedSS = Catalog.GetString
                        ("Tag '{0}' that you're trying to remove is applied to one or more items. " +
                         "Are you sure that you want to remove it?");
                
                readonly static string isAppliedHeaderSS = Catalog.GetString
                        ("Remove tag?");
                                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null; // App model
                VBox mainVBox = null;
                HBox topHBox = null;
                VButtonBox rightBox = null;
                HButtonBox bottomBox = null;
                Button closeButton = null;
                Button helpButton = null;
                Button newButton = null;
                Button editButton = null;
                Button removeButton = null;
                TagsTreeView tagsTree = null;
                Frame tagsFrame = null;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TagsWindow (Model.Root root, Gtk.Window parent) : base (titleSS)
                {
                        BorderWidth = 6;
                        modelRoot = root;
                        TransientFor = parent;
                        
                        // Bottom buttons
                        closeButton = new Button (Stock.Close);
                        closeButton.Clicked += OnCloseClicked;
                        helpButton = new Button (Stock.Help);
                        
                        // Bottom box
                        bottomBox = new HButtonBox ();
                        bottomBox.Spacing = 6;
                        bottomBox.Add (helpButton);
                        bottomBox.Add (closeButton);
                        
                        // Right buttons
                        newButton = new Button (Stock.New);
                        newButton.Clicked += OnNewClicked;
                        editButton = new Button (Stock.Edit);
                        editButton.Clicked += OnEditClicked;
                        removeButton = new Button (Stock.Remove);
                        removeButton.Clicked += OnRemoveClicked;
                        
                        // Right button box
                        rightBox = new VButtonBox ();
                        rightBox.Spacing = 6;
                        rightBox.Layout = ButtonBoxStyle.Start;
                        rightBox.Add (newButton);
                        rightBox.Add (editButton);
                        rightBox.Add (removeButton);
                        
                        // Tags tree
                        tagsTree = new TagsTreeView (root);
                        tagsTree.Controller.StateChange += OnControllerStateChange;
                        tagsTree.RowActivated += OnTagsTreeRowActivated;
                        editButton.Sensitive = tagsTree.Controller.Valid;
                        removeButton.Sensitive = tagsTree.Controller.Valid;
                                                
                        // Frame
                        tagsFrame = new Frame ();
                        tagsFrame.Shadow = ShadowType.In;
                        tagsFrame.Add (tagsTree);

                        // Top HBox
                        topHBox = new HBox (false, 6);
                        topHBox.PackStart (tagsFrame, true, true, 0);
                        topHBox.PackEnd (rightBox, false, false, 0);
                        
                        // Main VBox
                        mainVBox = new VBox (false, 12);
                        mainVBox.PackStart (topHBox, true, true, 0);
                        mainVBox.PackEnd (bottomBox, false, false, 0);
                        Add (mainVBox);
                        
                        GtkFu.ParseGeometry (this, Config.Ui.TagsWindowGeometry);
                        ShowAll ();
                }
                
                public void Unbind ()
                {
                        tagsTree.Unbind ();
                }
                
                public void SaveGeometry ()
                {
                        Config.Ui.TagsWindowGeometry = GtkFu.GetGeometry (this);
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnControllerStateChange (object o, EmptyModelControllerStateArgs args)
                {
                        removeButton.Sensitive = args.Valid;
                        editButton.Sensitive = args.Valid;
                }
                
                void OnCloseClicked (object o, EventArgs args)
                {
                        modelRoot.Window.HideTagsWindow ();
                }
                
                protected override bool OnDeleteEvent (Gdk.Event evnt)
                {
                        modelRoot.Window.HideTagsWindow ();
                        return true;
                }
                
                void OnTagsTreeRowActivated (object o, RowActivatedArgs args)
                {
                        if (! tagsTree.Controller.Valid)
                                return;
                                        
                        Core.Tag editedTag = tagsTree.SelectedTag;
                        if (editedTag == null) 
                                return;
                        
                        EditTag (editedTag);
                }
                
                void EditTag (Core.Tag editedTag)
                {
                        EditTagDialog dialog = new EditTagDialog (modelRoot, this, editedTag);
                        ResponseType response = (ResponseType) dialog.Run ();
                        
                        if (response == ResponseType.Ok) 
                                modelRoot.Tags.EditTag (editedTag, dialog.TagName);
                        
                        dialog.Destroy ();
                }
                
                void OnNewClicked (object o, EventArgs args)
                {
                        EditTagDialog dialog = new EditTagDialog (modelRoot, this);
                        ResponseType response = (ResponseType) dialog.Run ();
                        
                        if (response == ResponseType.Ok) 
                                modelRoot.Tags.Create (dialog.TagName);

                        dialog.Destroy ();
                }
                
                void OnEditClicked (object o, EventArgs args)
                {
                        if (! tagsTree.Controller.Valid)
                                return;
                        
                        Core.Tag editedTag = tagsTree.SelectedTag;
                        if (editedTag == null)
                                return;
                        
                        EditTag (editedTag);
                }
                
                void OnRemoveClicked (object o, EventArgs args)
                {
                        if (! tagsTree.Controller.Valid)
                                return;
                        
                        Core.Tag removedTag = tagsTree.SelectedTag;
                        if (removedTag == null)
                                return;
                        
                        // Display warning
                        if (modelRoot.Tags.GetAppliedCount (removedTag) != 0) {
                                string msg = String.Format (isAppliedSS, removedTag.Name);
                                ResponseType response = FastDialog.WarningYesNo (this,
                                                                                 isAppliedHeaderSS,
                                                                                 msg);
                                if (response != ResponseType.Yes)
                                        return;
                        }
                        
                        modelRoot.Tags.Delete (removedTag);
                        tagsTree.Controller.EmitStateChange ();
                }

        }
                
}
       
