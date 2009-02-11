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

/* See notes about StoreHelper */

namespace Diva.Editor.Model {

        using System;
        using System.Collections.Generic;
        using Gtk;

        public class StuffTreeStore : TreeStore {
                
                // Fields //////////////////////////////////////////////////////

                IStuffOrganizer organizer; // The organizer for this store
                StoreHelper tree;          // A helper tree-like thing so we can quickly find stuff
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public StuffTreeStore (IStuffOrganizer organizer) : base (typeof (Gdk.Pixbuf),
                                                                          typeof (bool),
                                                                          typeof (string),
                                                                          typeof (string),
                                                                          typeof (string))
                {
                        this.organizer = organizer;
                        organizer.NodeNameChange += OnNodeNameChanged;                        
                        this.tree = new StoreHelper ();
                }

                /* Add new stuff */
                public void Add (Core.Stuff stuff)
                {
                        // Ids this stuff should be in
                        int[] ids = organizer.GetNodeIdForStuff (stuff);

                        foreach (int id in ids) {
                                TreeIter iter;
                                if (tree.HasNode (id))
                                        iter = tree.GetIterForNode (id);
                                else {
                                        Append (out iter);
                                        tree.AddNode (id, iter);
                                }

                                TreeIter subIter = Insert (iter, 0);
                                tree.AddSubNode (id, subIter, stuff);

                                RefillCategory (id);
                        }

                        RefillStuff (stuff);
                        BindStuff (stuff);
                }

                public void Remove (Core.Stuff stuff)
                {
                        int[] ids = tree.GetIdsForStuff (stuff);
                        TreeIter[] iters = tree.GetItersForStuff (stuff);

                        // First remove the iters
                        foreach (TreeIter iter in iters) {
                                TreeIter iteriter = iter;
                                Remove (ref iteriter);
                        }

                        // Now remove the cats/nodes
                        foreach (int id in ids) {
                                tree.RemoveSubNode (id, stuff);
                                
                                // Check if we need to remove node...
                                if (tree.GetCountForNode (id) == 0) {
                                        TreeIter nodeIter = tree.GetIterForNode (id);
                                        Remove (ref nodeIter);
                                        tree.RemoveNode (id);
                                } else 
                                        RefillCategory (id);
                                
                        }
                        
                        UnBindStuff (stuff);
                }
                
                public Core.Stuff GetStuffForIter (TreeIter iter)
                {
                        return tree.GetStuffForIter (iter);
                }

                // Private methods /////////////////////////////////////////////

                void RefillCategory (int id)
                {
                        string major = organizer.GetMajorForNodeId (id, tree.GetCountForNode (id));
                        string minor = organizer.GetMinorForNodeId (id, tree.GetCountForNode (id));
                        string tags = organizer.GetTagsForNodeId (id, tree.GetCountForNode (id));
                        
                        TreeIter iter = tree.GetIterForNode (id);
                        // FIXME: Pixbuf?
                        
                        SetValue (iter, 1, false);
                        SetValue (iter, 2, major);
                        SetValue (iter, 3, minor);
                        SetValue (iter, 4, tags);                        
                }

                void RefillStuff (Core.Stuff stuff)
                {
                        TreeIter[] iters = tree.GetItersForStuff (stuff);
                        
                        foreach (TreeIter iter in iters) {
                                
                                SetValue (iter, 0, stuff.Pixbuf);
                                SetValue (iter, 1, stuff.Border);
                                SetValue (iter, 2, stuff.Major);
                                SetValue (iter, 3, stuff.Minor);
                                SetValue (iter, 4, stuff.TagsString);
                                
                        }
                }

                void BindStuff (Core.Stuff stuff)
                {
                        stuff.Change += OnStuffChanged;
                }

                void UnBindStuff (Core.Stuff stuff)
                {
                        stuff.Change -= OnStuffChanged;
                }

                public void OnStuffChanged (object sender, EventArgs args)
                {
                        UpdateStuff (sender as Core.Stuff);
                }

                public void UpdateStuff (Core.Stuff stuff)
                {
                        int[] newIds = organizer.GetNodeIdForStuff (stuff);
                        int[] oldIds = tree.GetIdsForStuff (stuff);

                        // Remove old
                        foreach (int oldId in oldIds) {
                                bool hasIt = false;
                                foreach (int newId in newIds)
                                        if (newId == oldId) {
                                                hasIt = true;
                                                break;
                                        }

                                if (! hasIt) {
                                        // We need to remove it
                                        TreeIter subIter = tree.GetIterForSubNode (oldId, stuff);
                                        Remove (ref subIter);
                                        tree.RemoveSubNode (oldId, stuff);
                                        
                                        // Check if we need to remove node...
                                        if (tree.GetCountForNode (oldId) == 0) {
                                                TreeIter nodeIter = tree.GetIterForNode (oldId);
                                                Remove (ref nodeIter);
                                                tree.RemoveNode (oldId);
                                        } else 
                                                RefillCategory (oldId);
                                }
                        }

                        // Add new
                        foreach (int newId in newIds) {
                                bool hasIt = false;
                                foreach (int oldId in oldIds)
                                        if (newId == oldId) {
                                                hasIt = true;
                                                break;
                                        }

                                if (! hasIt) {
                                        // We need to add it
                                        TreeIter iter;
                                        if (tree.HasNode (newId))
                                                iter = tree.GetIterForNode (newId);
                                        else {
                                                Append (out iter);
                                                tree.AddNode (newId, iter);
                                        }

                                        TreeIter subIter = Insert (iter, 0);
                                        tree.AddSubNode (newId, subIter, stuff);
                                        
                                        RefillCategory (newId);
                                }
                        }

                        // Update all the stuff
                        RefillStuff (stuff);
                }
                
                public void OnNodeNameChanged (object o, NodeArgs args)
                {
                        RefillCategory (args.NodeId);
                }
                        
        }

}
