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

namespace Gdv.Boilers {

        using System;
        using Gdv;
        using System.Collections.Generic;
        using Diva.Basics;
        
        public static class Common {

                public static List <object> GetDepObjectsForMediaItem (object o)
                {
                        List <object> list = new List <object> ();
                        list.Add ((o as Gdv.MediaItem).BaseSource);
                        return list;
                }

                public static void BoilMediaItem (object o, ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.MediaItem item = o as Gdv.MediaItem;
                        container.Add (new StringParameter ("name", item.Name));
                        container.Add (new RefParameter ("basesource", item.BaseSource, provider));
                }

                public static void BoilFileBased (object o, ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.IFileBased filebased = o as Gdv.IFileBased;
                        container.Add (new StringParameter ("basefile", filebased.BaseFile));
                }
                
        }

}