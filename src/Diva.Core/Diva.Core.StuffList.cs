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

/* Stuff represents something that is displayed in the "organizer". This is
 * all filters, media files, everything. Various Stuff implementations are
 * possible. Stuff can be generic (provided by the application itself, not
 * saved along with the project -- ie. all filters). Stuff can be labeled.
 */

namespace Diva.Core {

        using System;
        using System.Collections.Generic;
        using System.Collections;
        using Mono.Unix;
        using Basics;

        public class StuffList : IEnumerable, IBoilable {

                // Fields //////////////////////////////////////////////////////

                List<Stuff> stuffList; // A list of all the stuff

                // Events //////////////////////////////////////////////////////
                
                public event StuffStatusHandler StuffStatusChange;
                
                public event StuffHandler StuffChange;
                
                public event AppliedTagHandler StuffAppliedTagChange;
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public StuffList ()
                {
                        stuffList = new List<Stuff> ();
                }

                /* CONSTRUCTOR */
                public StuffList (ObjectContainer container, IBoilProvider provider)
                {
                        stuffList = new List <Stuff> ();
                        
                        foreach (RefParameter reff in container.FindAllByName ("stuff"))
                                Add ((Stuff) reff.ToObject (provider));
                }

                /* Add stuff */
                public void Add (Stuff stuff)
                {
                        if (stuffList.Contains (stuff))
                                throw new Exception ();
                        
                        stuffList.Add (stuff);
                        
                        stuff.Change += OnStuffChanged;
                        stuff.AppliedTagChange += OnStuffAppliedTagChange;
                                                
                        if (StuffStatusChange != null)
                                StuffStatusChange (this, new StuffStatusArgs (stuff,
                                                                              true));
                        
                }

                /* Remove stuff */
                public void Remove (Stuff stuff)
                {
                        if (! stuffList.Contains (stuff))
                                throw new Exception ();

                        stuffList.Remove (stuff);
                        
                        stuff.Change -= OnStuffChanged;
                        stuff.AppliedTagChange -= OnStuffAppliedTagChange;
                        
                        if (StuffStatusChange != null)
                                StuffStatusChange (this, new StuffStatusArgs (stuff,
                                                                              false));
                }

                /* IEnumerable */
                public IEnumerator GetEnumerator ()
                {
                        return stuffList.GetEnumerator ();
                }

                public void OnStuffChanged (object o, StuffArgs args)
                {
                        if (StuffChange != null)
                                StuffChange (this, args);
                }
                
                public void OnStuffAppliedTagChange (object o, AppliedTagArgs args)
                {
                        if (StuffAppliedTagChange != null)
                                StuffAppliedTagChange (this, args);
                }
                
                /* IBoilable */
                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (Stuff stuff in stuffList)
                                lst.Add (stuff);

                        return lst;
                }

                /* IBoilable */
                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (Stuff stuff in stuffList) 
                                container.Add (new RefParameter ("stuff", stuff, provider));
                }



                
        }
        
}
                
       
                                      
                                      
                                      
                
