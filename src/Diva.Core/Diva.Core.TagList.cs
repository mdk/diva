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

namespace Diva.Core {
        
        using System;
        using System.Collections;
        using System.Collections.Generic;
        using Mono.Unix;
        using Diva.Basics;

        public class TagList : IEnumerable, IBoilable {
                
                // Events //////////////////////////////////////////////////////
                
                public event TagHandler TagChange;
                
                public event TagStatusHandler TagStatusChange;

                // Fields //////////////////////////////////////////////////////

                List<Tag> tagList; // A list of all the stuff

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public TagList ()
                {
                        tagList = new List<Tag> ();
                }

                /* CONSTRUCTOR */
                public TagList (ObjectContainer container, IBoilProvider provider)
                {
                        tagList = new List<Tag> ();
                        
                        foreach (RefParameter reff in container.FindAllByName ("tag"))
                                Add ((Tag) reff.ToObject (provider));
                }

                /* Add tag */
                public void Add (Tag tag)
                {
                        if (tagList.Contains (tag))
                                throw new Exception ();
                        
                        tagList.Add (tag);
                        
                        if (TagStatusChange != null)
                                TagStatusChange (this, new TagStatusArgs (tag, true));
                        
                        tag.Change += OnTagChanged;
                }

                /* Remove tag */
                public void Remove (Tag tag)
                {
                        if (! tagList.Contains (tag))
                                throw new Exception ();

                        tagList.Remove (tag);
                        
                        if (TagStatusChange != null)
                                TagStatusChange (this, new TagStatusArgs (tag, false));
                        
                        tag.Change -= OnTagChanged;
                }

                /* IEnumerable */
                public IEnumerator GetEnumerator ()
                {
                        return tagList.GetEnumerator ();
                }

                /* IBoilable */
                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (Tag tag in tagList)
                                lst.Add (tag);

                        return lst;
                }

                /* IBoilable */
                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (Tag tag in tagList) 
                                container.Add (new RefParameter ("tag", tag, provider));
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnTagChanged (object o, TagArgs args)
                {
                        if (TagChange != null)
                                TagChange (this, args);
                }

        }
        
}
                
       
                                      
                                      
                                      
                
