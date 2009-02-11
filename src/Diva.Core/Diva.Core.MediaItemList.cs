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

/* We need all the typical events here, but I spare it till the time we needed
 * them */

namespace Diva.Core {
        
        using System;
        using System.Collections;
        using System.Collections.Generic;
        using Mono.Unix;
        using Gdv;
        using Basics;

        public class MediaItemList : IEnumerable, IBoilable {
                
                // Fields //////////////////////////////////////////////////////

                List<MediaItem> mediaItemList; // A list of all the items

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public MediaItemList ()
                {
                        mediaItemList = new List<MediaItem> ();
                }
                
                /* CONSTRUCTOR */
                public MediaItemList (ObjectContainer container, IBoilProvider provider)
                {
                        mediaItemList = new List <MediaItem> ();
                        
                        foreach (RefParameter reff in container.FindAllByName ("mediaitem"))
                                Add ((MediaItem) reff.ToObject (provider));
                }

                /* Add tag */
                public void Add (MediaItem item)
                {
                        if (mediaItemList.Contains (item))
                                throw new Exception ();
                        
                        mediaItemList.Add (item);
                }

                /* Remove tag */
                public void Remove (MediaItem item)
                {
                        if (! mediaItemList.Contains (item))
                                throw new Exception ();

                        mediaItemList.Remove (item);
                }

                /* IEnumerable */
                public IEnumerator GetEnumerator ()
                {
                        return mediaItemList.GetEnumerator ();
                }

                /* IBoilable */
                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (MediaItem item in mediaItemList)
                                lst.Add (item);

                        return lst;
                }

                /* IBoilable */
                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (MediaItem item in mediaItemList) 
                                container.Add (new RefParameter ("mediaitem", item, provider));
                }
                
        }
        
}
                
       
                                      
                                      
                                      
                
