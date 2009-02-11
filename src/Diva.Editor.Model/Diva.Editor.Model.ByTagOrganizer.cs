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
        using Mono.Unix;
        using Util;
        using System.Collections.Generic;
        using System.Collections;

        public sealed class ByTagOrganizer : IStuffOrganizer {
                
                // Public event ///////////////////////////////////////////////
                
                public event NodeHandler NodeNameChange;

                // Translatable ///////////////////////////////////////////////
                
                readonly static string unTaggedSS = Catalog.GetString
                        ("Untagged");
                
                // Constant ///////////////////////////////////////////////////
                
                readonly static int unTaggedConstant = 1000;
                
                // Fields /////////////////////////////////////////////////////
                
                Dictionary <int, Core.Tag> idToTag; // id -> Tag
                Dictionary <Core.Tag, int> tagToId; // Tag -> id
                int counter;

                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ByTagOrganizer ()
                {
                        idToTag = new Dictionary <int, Core.Tag> ();
                        tagToId = new Dictionary <Core.Tag, int> ();
                        counter = 0;
                }

                public int[] GetNodeIdForStuff (Core.Stuff stuff)
                {
                        if (stuff.HasNoTags)
                                return OtherFu.ArrayizeInt (unTaggedConstant);
                        else {
                                List <int> tags = new List <int> ();
                                IEnumerator tagsEnum = stuff.TagsEnum;
                                while (tagsEnum.MoveNext ())
                                        tags.Add (GetIdForTag (tagsEnum.Current as Core.Tag));

                                return tags.ToArray ();
                        }
                }
                
                public string GetMajorForNodeId (int id, int count)
                {
                        // Untagged
                        if (id == unTaggedConstant)
                                return String.Format (unTaggedSS, count);
                        else
                                return String.Format ("{0} ({1})", GetTagForId (id).Name,
                                                      count);
                }
                
                public string GetMinorForNodeId (int id, int count)
                {
                        return String.Empty;
                }

                public string GetTagsForNodeId (int id, int count)
                {
                        return String.Empty;
                }

                // Private methods /////////////////////////////////////////////

                int GetIdForTag (Core.Tag t)
                {
                        if (tagToId.ContainsKey (t))
                                return tagToId [t];
                        
                        tagToId [t] = counter;
                        idToTag [counter] = t;
                        t.Change += OnTagChange;
                        counter++;

                        if (counter == unTaggedConstant)
                                throw new Exception ("Too many tags!");

                        return counter - 1;
                }

                Core.Tag GetTagForId (int i)
                {
                        return idToTag [i];
                }
                
                void OnTagChange (object o, Core.TagArgs args)
                {
                        if (NodeNameChange != null)
                                NodeNameChange (this, new NodeArgs (GetIdForTag (args.Tag)));
                }

        }

}
