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
        
        public sealed class ByLengthOrganizer : IStuffOrganizer {
                
                // Enums ///////////////////////////////////////////////////////
                
                enum NodeId { Undefined, UpToFiveSeconds, MoreFiveSeconds, MoreTenSeconds,
                              MoreThirtySeconds, MoreMinute }
                
                // Events //////////////////////////////////////////////////////
                
                public event NodeHandler NodeNameChange;

                // Translatable ////////////////////////////////////////////////

                readonly static string undefinedSS = Catalog.GetString
                        ("Undefined ({0})");

                readonly static string upToFiveSecondsSS = Catalog.GetString
                        ("Up to 5 seconds ({0})");

                readonly static string moreFiveSecondsSS = Catalog.GetString
                        ("More than 5 seconds ({0})");

                readonly static string moreTenSecondsSS = Catalog.GetString
                        ("More than 10 seconds ({0})");

                readonly static string moreThirtySecondsSS = Catalog.GetString
                        ("More than 30 seconds ({0})");

                readonly static string moreMinuteSS = Catalog.GetString
                        ("More than 1 minute ({0})");

                // Public methods //////////////////////////////////////////////
                                
                public int[] GetNodeIdForStuff (Core.Stuff stuff)
                {
                        if (! (stuff is Core.MediaItemStuff))
                                return OtherFu.ArrayizeInt ((int) NodeId.Undefined);

                        Core.MediaItemStuff mediaItemStuff = stuff as Core.MediaItemStuff;

                        if (! (mediaItemStuff.MediaItem is Gdv.ITimeable))
                                return OtherFu.ArrayizeInt ((int) NodeId.Undefined);

                        Gdv.ITimeable item = mediaItemStuff.MediaItem as Gdv.ITimeable;

                        if (item.Length < Gdv.Time.FromSeconds (5))
                                return OtherFu.ArrayizeInt ((int) NodeId.UpToFiveSeconds);

                        // Here we can begin adding

                        List <int> list = new List <int> ();
                        list.Add ((int) NodeId.MoreFiveSeconds);

                        // More than 10s
                        if (item.Length > Gdv.Time.FromSeconds (10))
                                list.Add ((int) NodeId.MoreTenSeconds);

                        // More than 30s
                        if (item.Length > Gdv.Time.FromSeconds (30))
                                list.Add ((int) NodeId.MoreThirtySeconds);

                        // More than 1 minute
                        if (item.Length > Gdv.Time.FromSeconds (60))
                                list.Add ((int) NodeId.MoreMinute);

                        return (int []) list.ToArray ();
                }

                public string GetMajorForNodeId (int id, int count)
                {
                        switch ((NodeId) id) {
                                
                                case NodeId.UpToFiveSeconds:
                                        return String.Format (upToFiveSecondsSS, count);

                                case NodeId.MoreFiveSeconds:
                                        return String.Format (moreFiveSecondsSS, count);

                                case NodeId.MoreTenSeconds:
                                        return String.Format (moreTenSecondsSS, count);

                                case NodeId.MoreThirtySeconds:
                                        return String.Format (moreThirtySecondsSS, count);

                                case NodeId.MoreMinute:
                                        return String.Format (moreMinuteSS, count);

                                default:
                                        return String.Format (undefinedSS, count);
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