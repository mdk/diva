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
        
        public class StuffMenu : Menu {
                
                // Translatable /////////////////////////////////////////////

                readonly static string tagSS = Catalog.GetString 
                        ("Tag");
                
                readonly static string createNewTagSS = Catalog.GetString
                        ("Create new tag...");

                // Delegates & events ///////////////////////////////////////
                
                // Fields ///////////////////////////////////////////////////
                
                Core.Stuff stuff;                        // Stuff that this menu was spawned for
                Model.Root modelRoot;                    // App model
                Dictionary <object, Core.Tag> itemToTag; // For individual tag toggles
                
                // Public methods ///////////////////////////////////////////
                
                /* Constructor */
                public StuffMenu (Model.Root root, Core.Stuff stuff)
                {
                        itemToTag = new Dictionary <object, Core.Tag> ();
                        modelRoot = root;
                        this.stuff = stuff;
                        
                        // Tags
                        MenuItem tagMenuItem = new MenuItem (tagSS);
                        tagMenuItem.Submenu = CreateTagsMenu ();
                        Append (tagMenuItem);
                }
                
                // Private methods ////////////////////////////////////////////

                Menu CreateTagsMenu ()
                {
                        Menu tagsMenu = new Menu ();
                        
                        MenuItem newTagItem = new MenuItem (createNewTagSS);
                        newTagItem.Activated += OnNewTagActivated;
                        tagsMenu.Append (newTagItem);
                        
                        // All tags
                        foreach (Core.Tag tag in modelRoot.Tags) {
                                CheckMenuItem item = new CheckMenuItem (tag.Name);
                                item.Active = stuff.HasTag (tag);
                                item.Toggled += OnTagToggled;
                                tagsMenu.Append (item);
                                itemToTag [item] = tag;
                        }

                        return tagsMenu;
                }

                /* New tag */
                public void OnNewTagActivated (object sender, EventArgs args)
                {
                        // FIXME: Parent to the window
                        EditTagDialog dialog = new EditTagDialog (modelRoot, null);
                        ResponseType response = (ResponseType) dialog.Run ();
                        
                        if (response == ResponseType.Ok) 
                                modelRoot.Tags.CreateAndApply (dialog.TagName, 
                                                               stuff);

                        dialog.Destroy ();
                }

                /* Tag status changed */
                public void OnTagToggled (object sender, EventArgs args)
                {
                        Core.Tag tag = itemToTag [sender];
                        
                        if ((sender as CheckMenuItem).Active == true)
                                modelRoot.Tags.Apply (tag, stuff);
                        else
                                modelRoot.Tags.Remove (tag, stuff);
                }

        }
        
}
