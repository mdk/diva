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
        
        public sealed class ByTypeOrganizer : IStuffOrganizer {
                
                // Enums ///////////////////////////////////////////////////////
                
                enum NodeId { Video, Audio, Photo, Other}
                
                // Events //////////////////////////////////////////////////////
                
                public event NodeHandler NodeNameChange;

                // Translatable ////////////////////////////////////////////////

                readonly static string videoSS = Catalog.GetString
                        ("Video clips ({0})");

                readonly static string audioSS = Catalog.GetString
                        ("Music & sound ({0})");

                readonly static string photoSS = Catalog.GetString
                        ("Photos ({0})");

                readonly static string otherSS = Catalog.GetString
                        ("Other ({0})");

                // Public methods //////////////////////////////////////////////
                                
                public int[] GetNodeIdForStuff (Core.Stuff stuff)
                {
                        if (! (stuff is Core.MediaItemStuff))
                                return OtherFu.ArrayizeInt ((int) NodeId.Other);
                        else {
                                if ((stuff as Core.MediaItemStuff).MediaItem is Gdv.AVItem)
                                        return OtherFu.ArrayizeInt ((int) NodeId.Video);
                                else if ((stuff as Core.MediaItemStuff).MediaItem is Gdv.PhotoItem)
                                        return OtherFu.ArrayizeInt ((int) NodeId.Photo);
                                else if ((stuff as Core.MediaItemStuff).MediaItem is Gdv.AudioItem)
                                        return OtherFu.ArrayizeInt ((int) NodeId.Audio);

                                // Unrecognized
                                return OtherFu.ArrayizeInt ((int) NodeId.Other);
                        }
                }

                public string GetMajorForNodeId (int id, int count)
                {
                        switch ((NodeId) id) {
                                
                                case NodeId.Video:
                                        return String.Format (videoSS, count);
                                        
                                case NodeId.Audio:
                                        return String.Format (audioSS, count);

                                case NodeId.Photo:
                                        return String.Format (photoSS, count);
                                        
                                default:
                                        return String.Format (otherSS, count);
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
