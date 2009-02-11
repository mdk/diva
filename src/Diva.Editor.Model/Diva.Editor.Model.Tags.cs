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
        using System.Collections;
        using Commands;
        using Mono.Unix;

        public sealed class Tags : IModelPart, IEnumerable {
                
                // Translatable ////////////////////////////////////////////////

                readonly static string createApplyMessageSS = Catalog.GetString
                        ("Apply new tag '{0}' to '{1}'");

                readonly static string createApplyInstantMessageSS = Catalog.GetString
                        ("New tag '{0}' was applied to '{1}'");
                
                readonly static string deleteMessageSS = Catalog.GetString
                        ("Delete tag '{0}'");

                readonly static string deleteInstantMessageSS = Catalog.GetString
                        ("Tag '{0}' was deleted");
                
                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                
                // Events //////////////////////////////////////////////////////
                
                public event TagHandler TagChange;
                
                public event Core.TagStatusHandler TagStatusChange;
                
                // Properties //////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Tags (Root root)
                {
                        modelRoot = root;
                        
                        modelRoot.Project.Tags.TagChange += OnTagChanged;
                        modelRoot.Project.Tags.TagStatusChange += OnTagStatusChanged;
                }
                
                /* IEnumerable */
                public IEnumerator GetEnumerator () 
                {
                        return modelRoot.Project.Tags.GetEnumerator ();
                }
                
                public void Delete (Core.Tag tag)
                {
                        MultiCommand multiCmd = new MultiCommand (String.Format (deleteMessageSS,
                                                                                 tag.Name),
                                                                  String.Format (deleteInstantMessageSS,
                                                                                 tag.Name));
                        
                        // Find all occurences of this tag and remove them
                        foreach (Core.Stuff stuff in modelRoot.Project.Stuff) 
                                if (stuff.HasTag (tag)) {
                                        Core.IUndoableCommand cmd = new RemoveTag (tag, stuff);
                                        multiCmd.Add (cmd);
                                }
                        
                        multiCmd.Add (new DeleteTag (tag));
                        modelRoot.CommandProcessor.PushCommand (multiCmd);
                }
                
                public void Create (string tagName)
                {
                        Core.Tag tag = new Core.Tag (tagName);
                        Core.IUndoableCommand createCmd = new CreateNewTag (tag);
                        modelRoot.CommandProcessor.PushCommand (createCmd);
                }
                
                public void CreateAndApply (string tagName, Core.Stuff stuff)
                {
                        Core.Tag tag = new Core.Tag (tagName);
                        MultiCommand cmd = new MultiCommand (String.Format (createApplyMessageSS,
                                                                            tag.Name,
                                                                            stuff.Name),
                                                             String.Format (createApplyInstantMessageSS,
                                                                            tag.Name,
                                                                            stuff.Name));
                        cmd.Add (new CreateNewTag (tag));
                        cmd.Add (new ApplyTag (tag, stuff));
                        
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }
                
                public void Apply (Core.Tag tag, Core.Stuff stuff)
                {
                        Core.ICommand cmd = new ApplyTag (tag, stuff);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }
                
                public void Remove (Core.Tag tag, Core.Stuff stuff)
                {
                        Core.ICommand cmd = new RemoveTag (tag, stuff);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }
                
                public bool HasTagName (string tagName)
                {
                        foreach (Core.Tag t in modelRoot.Project.Tags)
                                if (t.Name == tagName)
                                        return true;
                        
                        return false;
                }
                
                public bool HasTagNameExcluding (string tagName, Core.Tag exclude)
                {
                        foreach (Core.Tag t in modelRoot.Project.Tags)
                                if (t.Name == tagName && 
                                    t != exclude)
                                        return true;
                        
                        return false;
                }
                                
                public int GetAppliedCount (Core.Tag tag)
                {
                        int counter = 0;
                        foreach (Core.Stuff stuff in modelRoot.Stuff)
                                if (stuff.HasTag (tag))
                                        counter++;
                        
                        return counter;
                }
                
                public void EditTag (Core.Tag tag, string newName)
                {
                        // Exit if no change requested
                        if (newName != null && newName == tag.Name)
                                return;
                        
                        Core.IUndoableCommand cmd = new EditTagName (tag, newName);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }

                // Private methods /////////////////////////////////////////////
                
                public void OnTagChanged (object o, Core.TagArgs args)
                {
                        // FIXME: Remove the duplicated delegates
                        if (TagChange != null)
                                TagChange (modelRoot, new TagArgs (args.Tag));
                }
                
                public void OnTagStatusChanged (object o, Core.TagStatusArgs args)
                {
                        if (TagStatusChange != null)
                                TagStatusChange (modelRoot, args);
                }
                
                
        }
        
}
