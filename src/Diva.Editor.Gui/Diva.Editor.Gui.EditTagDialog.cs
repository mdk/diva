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

        public class EditTagDialog : Dialog {
                
                // Translatable /////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString 
                        ("New tag");
                
                readonly static string titleEditSS = Catalog.GetString 
                        ("Edit tag");
                
                readonly static string nameSS = Catalog.GetString
                        ("Tag name:");

                readonly static string notUniqueSS = Catalog.GetString 
                        ("You already have a tag with this name in a project. " +
                         "Please provide a different name.");
                
                readonly static string notUniqueHeaderSS = Catalog.GetString 
                        ("Name not unique");
                
                readonly static string emptySS = Catalog.GetString
                        ("Tag name cannot be empty. Please provide a valid tag name.");
                
                readonly static string emptyHeaderSS = Catalog.GetString
                        ("Empty name");
                
                readonly static string invalidSS = Catalog.GetString
                        ("This is not a valid tag name. " +
                         "Tag name consists of a single word with alphanumeric characters. " +
                         "Please provide a valid tag name.");
                
                readonly static string invalidHeaderSS = Catalog.GetString
                        ("Invalid name");

                // Fields ///////////////////////////////////////////////////
                
                Entry nameEntry;             // The entry where the user inputs the name
                Model.Root modelRoot = null; // App model
                Core.Tag refTag = null;      // Reference tag - if we're editing one

                // Properties ///////////////////////////////////////////////
                
                public string TagName {
                        get {
                                return nameEntry.Text;
                        }
                }

                // Public methods ///////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public EditTagDialog (Model.Root root, Gtk.Window parent) :
                base (titleSS, parent, DialogFlags.NoSeparator | DialogFlags.Modal)
                {
                        modelRoot = root;
                        refTag = null;
                        
                        Resizable = false;
                        BorderWidth = 6;
                        
                        // First let's pack the default buttons
                        Button okButton = new Button (Stock.Ok);
                        okButton.Clicked += OnOkButtonClicked;
                        AddButton (Stock.Cancel, ResponseType.Cancel);
                        ActionArea.Add (okButton);

                        Gtk.Label nameLabel = new Gtk.Label (nameSS);
                        nameEntry = new Entry ();
                        
                        // Table
                        HBox hBox = new HBox (false, 6);
                        hBox.PackStart (nameLabel, false, false, 0);
                        hBox.PackStart (nameEntry, true, true, 0);
                        
                        // We connect a default action for enter
                        nameEntry.Activated += OnNameEntryActivated;
                        
                        // Pack it all
                        VBox.PackStart (hBox, false, false, 6);
                        
                        VBox.Spacing = 18;
                        VBox.ShowAll ();
                        
                        nameEntry.GrabFocus ();
                }
                
                /* CONSTRUCTOR */
                public EditTagDialog (Model.Root root, Gtk.Window parent, Core.Tag tag) :
                this (root, parent)
                {
                        refTag = tag;
                        nameEntry.Text = tag.Name;
                        
                        Title = titleEditSS;
                }
                
                // Events /////////////////////////////////////////////////////
                
                /* Emitted when the user click the Ok button. We validate the input here */
                void OnOkButtonClicked (object sender, EventArgs args)
                {
                        if (ValidateInput ())
                                Respond (ResponseType.Ok);
                        else 
                                return;
                }
                
                /* Emitted when the user presses enter on the name entry. We act is if
                 * ok button was pressed */
                void OnNameEntryActivated (object sender, EventArgs args)
                {
                        OnOkButtonClicked (this, new EventArgs ());
                }
                
                bool ValidateInput ()
                {
                        string name = StringFu.MakeSane (nameEntry.Text);
                        
                        // Empty name
                        if (name == String.Empty) {
                                FastDialog.WarningOk (this, emptyHeaderSS, emptySS);
                                return false;
                        }
                        
                        // Invalid name
                        if (StringFu.HasWhiteSpace (name) ||
                            ! (StringFu.IsLetterOrDigit (name))) {
                                FastDialog.WarningOk (this, invalidHeaderSS, invalidSS);
                                return false;
                        }
                        
                        // Not unique
                        if (modelRoot.Tags.HasTagNameExcluding (name, refTag)) {
                                FastDialog.WarningOk (this, notUniqueHeaderSS, notUniqueSS);
                                return false;
                        }
                                
                        return true;
                }

        }
        
}
                

