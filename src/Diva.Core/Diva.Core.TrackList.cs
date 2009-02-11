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
        
        public class TrackList : IEnumerable, IBoilable {
                
                // Fields //////////////////////////////////////////////////////

                List<Track> trackList; // A list of all the tracks
                
                // Properties //////////////////////////////////////////////////

                public int Count { 
                        get { return trackList.Count; }
                }
                
                public Track this [int index] {
                        get { return trackList [index]; }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public TrackList ()
                {
                        trackList = new List<Track> ();
                }

                /* CONSTRUCTOR */
                public TrackList (ObjectContainer container, IBoilProvider provider)
                {
                        trackList = new List<Track> ();
                        
                        foreach (RefParameter reff in container.FindAllByName ("track"))
                                Add ((Track) reff.ToObject (provider));
                }

                /* Add track */
                public void Add (Track track)
                {
                        if (trackList.Contains (track))
                                throw new Exception ();
                        
                        trackList.Add (track);
                }

                /* Remove track */
                public void Remove (Track track)
                {
                        if (! trackList.Contains (track))
                                throw new Exception ();

                        trackList.Remove (track);
                }

                /* IEnumerable */
                public IEnumerator GetEnumerator ()
                {
                        return trackList.GetEnumerator ();
                }

                /* IBoilable */
                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (Track track in trackList)
                                lst.Add (track);

                        return lst;
                }

                /* IBoilable */
                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (Track track in trackList) 
                                container.Add (new RefParameter ("track", track, provider));
                }
                
        }
        
}
                
       
                                      
                                      
                                      
                
