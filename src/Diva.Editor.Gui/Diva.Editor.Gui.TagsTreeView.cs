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
        using Widgets;
        using System.Collections;
        using System.Collections.Generic;
        using Util;
        using Mono.Unix;

        public sealed class TagsTreeView : TreeView, IUnbindable {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string noTagsSS = Catalog.GetString
                        ("No tags in the project");
                
                // Fields //////////////////////////////////////////////////////
                                
                Model.Root modelRoot = null;
                ListStore listStore = null;
                Dictionary <TreeIter, Core.Tag> iterToTag = null;
                Dictionary <Core.Tag, TreeIter> tagToIter = null;
                EmptyModelController controller = null;
                                
                // Properties //////////////////////////////////////////////////
                
                public EmptyModelController Controller {
                        get { return controller; }
                }
                
                public Core.Tag SelectedTag {
                        get { 
                                if (controller.Empty)
                                        return null;
                                
                                TreeIter iter = TreeIter.Zero;
                                if (! Selection.GetSelected (out iter))
                                        return null;
                                
                                return iterToTag [iter];
                        }
                }
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TagsTreeView (Model.Root root) : base ()
                {
                        modelRoot = root;
                        HeadersVisible = false;
                        RulesHint = true;
                        
                        // Renderers
                        CellRendererText textRenderer = new CellRendererText ();
                        textRenderer.Ellipsize = Pango.EllipsizeMode.End;
                        
                        // Column
                        TreeViewColumn clmn = new TreeViewColumn ();
                        clmn.PackStart (textRenderer, true);
                        clmn.AddAttribute (textRenderer, "markup", 0);
                        AppendColumn (clmn);
                        
                        RebuildModel ();
                        
                        // Model bind
                        modelRoot.Tags.TagChange += OnTagChanged;
                        modelRoot.Tags.TagStatusChange += OnTagStatusChanged;
                        modelRoot.Stuff.StuffAppliedTagChange += OnStuffAppliedTagChanged;
                }
                
                public void Unbind ()
                {
                        modelRoot.Tags.TagChange -= OnTagChanged;
                        modelRoot.Tags.TagStatusChange -= OnTagStatusChanged;
                        modelRoot.Stuff.StuffAppliedTagChange -= OnStuffAppliedTagChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                void RebuildModel ()
                {
                        listStore = new ListStore (typeof (string),
                                                   typeof (int));
                        iterToTag = new Dictionary <TreeIter, Core.Tag> ();
                        tagToIter = new Dictionary <Core.Tag, TreeIter> ();
                        
                        foreach (Core.Tag tag in modelRoot.Tags)
                                RefillTag (tag);

                        this.Model = listStore;
                        
                        // Controller
                        controller = new EmptyModelController (listStore, this);
                        controller.EmptyChange += OnControllerEmptyChange;
                        controller.Analyze ();
                        
                }
                
                void RefillTag (Core.Tag tag)
                {
                        int count = modelRoot.Tags.GetAppliedCount (tag);
                        string txt = String.Format ("{0} ({1})", 
                                                    StringFu.Markupize (tag.Name), 
                                                    count);
                        
                        if (! tagToIter.ContainsKey (tag)) {
                                TreeIter iter = listStore.AppendValues (txt, count);
                                iterToTag [iter] = tag;
                                tagToIter [tag] = iter;
                        } else {
                                TreeIter iter = tagToIter [tag];
                                listStore.SetValue (iter, 0, txt);
                                listStore.SetValue (iter, 1, count);
                        }
                }
                
                void RemoveTag (Core.Tag tag)
                {
                        if (! tagToIter.ContainsKey (tag))
                                return;
                        
                        TreeIter iter = tagToIter [tag];
                        tagToIter.Remove (tag);
                        iterToTag.Remove (iter);
                        
                        listStore.Remove (ref iter);
                }
                
                void OnTagChanged (object o, Model.TagArgs args)
                {
                        RefillTag (args.Tag);
                }
                
                void OnTagStatusChanged (object o, Core.TagStatusArgs args)
                {
                        if (args.Present == true)
                                RefillTag (args.Tag);
                        else
                                RemoveTag (args.Tag);
                }
                
                void OnStuffAppliedTagChanged (object o, Core.AppliedTagArgs args)
                {
                        RefillTag (args.Tag);
                }
                
                void OnControllerEmptyChange (object o, EmptyModelControllerEmptyArgs args)
                {
                        if (args.IsEmpty) {
                                string txt = String.Format ("<i>{0}</i>", StringFu.Markupize (noTagsSS));
                                args.TreeIter = listStore.AppendValues (txt, 0);
                        } else 
                                listStore.Remove (ref args.TreeIter);
                }

        }
        
}

       
