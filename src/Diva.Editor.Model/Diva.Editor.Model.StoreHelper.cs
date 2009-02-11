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

/* Okay, I hate this code. Looks like a "I'm in college and I just learned about
 * objective programming" shit. but it works. NEEDS a rewrite in future */

namespace Diva.Editor.Model {

        using System;
        using System.Collections.Generic;
        using Gtk;
        
        public class StoreHelper {

                // Private holders /////////////////////////////////////////////

                private class Node {
                   
                        public TreeIter Iter;
                        public int Count;
                        public int Id;
                        public List <SubNode> SubNodes;
                        
                }

                private class SubNode {
                   
                        public TreeIter Iter;
                        public Core.Stuff Stuff;
                        
                }
                
                // Fields //////////////////////////////////////////////////////

                Dictionary <int, Node> idToNode;          // Id -> Node
                Dictionary <TreeIter, Core.Stuff> iterToStuff; // TreeIter -> Stuff

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public StoreHelper ()
                {
                        idToNode = new Dictionary <int, Node> ();
                        iterToStuff = new Dictionary <TreeIter, Core.Stuff> ();
                }

                /* Add a node with a given id */
                public void AddNode (int id, TreeIter iter)
                {
                        if (HasNode (id))
                                throw new Exception ("Node already present");

                        Node node = new Node ();;
                        node.Iter = iter;
                        node.Id = id;
                        node.Count = 0;
                        node.SubNodes = new List <SubNode> ();

                        idToNode [id] = node;
                }

                /* Remove a node */
                public void RemoveNode (int id)
                {
                        idToNode.Remove (id);
                }

                /* Remove a node */
                public void RemoveSubNode (int id, Core.Stuff stuff)
                {
                        Node node = idToNode [id];

                        SubNode targetSubNode = null;
                        foreach (SubNode subNode in node.SubNodes)
                                if (subNode.Stuff == stuff) {
                                        targetSubNode = subNode;
                                        break;
                                }

                        if (targetSubNode == null)
                                throw new Exception ();

                        node.SubNodes.Remove (targetSubNode);
                        node.Count--;

                        iterToStuff.Remove (targetSubNode.Iter);
                }

                /* Get iter for the given stuff in the given node */
                public TreeIter GetIterForSubNode (int id, Core.Stuff stuff)
                {
                        Node node = idToNode [id];
                        
                        SubNode targetSubNode = null;
                        foreach (SubNode subNode in node.SubNodes)
                                if (subNode.Stuff == stuff) {
                                        targetSubNode = subNode;
                                        break;
                                }
                        
                        if (targetSubNode == null)
                                throw new Exception ();

                        return targetSubNode.Iter;
                }

                /* Adds a sub-node to a node */
                public void AddSubNode (int parentId, TreeIter iter, Core.Stuff stuff)
                {
                        Node node = idToNode [parentId];
                        SubNode subnode = new SubNode ();

                        subnode.Iter = iter;
                        subnode.Stuff = stuff;
                        
                        node.SubNodes.Add (subnode);
                        iterToStuff [subnode.Iter] = stuff;
                        node.Count++;
                }

                /* Return the node-ids this stuff is in */
                public int[] GetIdsForStuff (Core.Stuff stuff)
                {
                        List <int> ret = new List <int> ();
                        
                        foreach (Node node in idToNode.Values) 
                                foreach (SubNode subnode in node.SubNodes)
                                        if (subnode.Stuff == stuff) {
                                                ret.Add (node.Id);
                                                continue;
                                        }

                        return ret.ToArray ();
                }

                /* Return the iters representing the stuff */
                public TreeIter[] GetItersForStuff (Core.Stuff stuff)
                {
                        List <TreeIter> ret = new List <TreeIter> ();
                        
                        foreach (Node node in idToNode.Values) 
                                foreach (SubNode subnode in node.SubNodes)
                                        if (subnode.Stuff == stuff) {
                                                ret.Add (subnode.Iter);
                                                break;
                                        }

                        return ret.ToArray ();
                }

                /* Check if we have a node with a given id */
                public bool HasNode (int id)
                {
                        return idToNode.ContainsKey (id);
                }

                /* Get iter for the given id */
                public TreeIter GetIterForNode (int id)
                {
                        if (! HasNode (id))
                                throw new Exception ("Node not present");

                        return idToNode [id].Iter;
                }

                /* Return the count of sub-items for this node */
                public int GetCountForNode (int id)
                {
                        if (! HasNode (id))
                                throw new Exception ("Node not present");
                        
                        return idToNode [id].Count;
                }

                /* Get a stuff for a given treeiter. Null it iter
                 * represents a node */
                public Core.Stuff GetStuffForIter (TreeIter iter)
                {
                        if (iterToStuff.ContainsKey (iter))
                                return iterToStuff [iter];
                        else
                                return null;
                }

        }

}
