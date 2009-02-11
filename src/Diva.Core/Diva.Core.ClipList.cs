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
        
        public class ClipList : IEnumerable, IBoilable {
                
                // Events //////////////////////////////////////////////////////
                
                public event ClipHandler ClipAdd;
                
                public event ClipHandler ClipRemove;
                
                // Fields //////////////////////////////////////////////////////

                List<Clip> clipList; // A list of all the clips
                
                // Properties //////////////////////////////////////////////////
                
                public int Count { 
                        get { return clipList.Count; }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ClipList ()
                {
                        clipList = new List<Clip> ();
                }

                /* CONSTRUCTOR */
                public ClipList (ObjectContainer container, IBoilProvider provider)
                {
                        clipList = new List<Clip> ();
                        
                        foreach (RefParameter reff in container.FindAllByName ("clip"))
                                Add ((Clip) reff.ToObject (provider));
                }

                /* Add track */
                public void Add (Clip clip)
                {
                        if (clipList.Contains (clip))
                                throw new Exception ();
                        
                        clipList.Add (clip);
                        
                        if (ClipAdd != null)
                                ClipAdd (this, new ClipArgs (clip));
                }

                /* Remove track */
                public void Remove (Clip clip)
                {
                        if (! clipList.Contains (clip))
                                throw new Exception ();

                        clipList.Remove (clip);
                        
                        if (ClipRemove != null)
                                ClipRemove (this, new ClipArgs (clip));
                }

                /* IEnumerable */
                public IEnumerator GetEnumerator ()
                {
                        return clipList.GetEnumerator ();
                }

                /* IBoilable */
                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (Clip clip in clipList)
                                lst.Add (clip);

                        return lst;
                }

                /* IBoilable */
                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (Clip clip in clipList) 
                                container.Add (new RefParameter ("clip", clip, provider));
                }

                /* Get end of the most rightish clip. In other words -- get the
                 * duration of the project */
                public Time GetMaximumOutPoint ()
                {
                        Time max = Time.Zero;

                        foreach (Clip clip in clipList)
                                max = (UInt64) Math.Max (clip.TimelineOut, max);

                        return max;
                }
                
        }
        
}
