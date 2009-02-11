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
        
        public sealed class ByUsageOrganizer : IStuffOrganizer {

                // Fields //////////////////////////////////////////////////////

                Model.Root modelRoot = null;
                
                // Enums ///////////////////////////////////////////////////////
                
                enum NodeId { Unused, UsedOnce, UsedMore }
                
                // Events //////////////////////////////////////////////////////
                
                public event NodeHandler NodeNameChange;

                // Translatable ////////////////////////////////////////////////

                readonly static string unusedSS = Catalog.GetString
                        ("Unused ({0})");

                readonly static string usedOnceSS = Catalog.GetString
                        ("Used only once ({0})");

                readonly static string usedMoreSS = Catalog.GetString
                        ("Used many times ({0})");

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ByUsageOrganizer (Model.Root root)
                {
                        modelRoot = root;
                }
                                
                public int[] GetNodeIdForStuff (Core.Stuff stuff)
                {
                        if (! (stuff is Core.MediaItemStuff))
                                return OtherFu.ArrayizeInt ((int) NodeId.Unused);

                        Core.MediaItemStuff mediaItemStuff = stuff as Core.MediaItemStuff;
                        int count = modelRoot.MediaItems.GetUsageCount (mediaItemStuff.MediaItem);

                        if (count == 0)
                                return OtherFu.ArrayizeInt ((int) NodeId.Unused);
                        else if (count == 1)
                                return OtherFu.ArrayizeInt ((int) NodeId.UsedOnce);
                        else
                                return OtherFu.ArrayizeInt ((int) NodeId.UsedMore);
                }

                public string GetMajorForNodeId (int id, int count)
                {
                        switch ((NodeId) id) {
                                
                                case NodeId.UsedOnce:
                                        return String.Format (usedOnceSS, count);
                                        
                                case NodeId.UsedMore:
                                        return String.Format (usedMoreSS, count);
                                        
                                default:
                                        return String.Format (unusedSS, count);
                        }
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

        }

}