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
        using System.Collections.Generic;
        using Mono.Unix;
        using System.Collections;
        using Basics;
        
        public abstract class Stuff : IBoilable {

                // Fields ///////////////////////////////////////////////////////

                List <Tag> appliedTags; // Tags applied
                
                // Events ///////////////////////////////////////////////////////
                
                public event StuffHandler Change;
                
                public event AppliedTagHandler AppliedTagChange;
                                
                // Abstract properties //////////////////////////////////////////
                
                /* If this is generic stuff */
                public abstract bool Generic { get; }
                
                /* If a border should be display around stuff icons (ie. thubmnails) */
                public abstract bool Border { get; }
                                
                /* FIXME */
                public abstract string Major { get; }

                /* FIXME */
                public abstract string Minor { get; }

                /* Normal name */
                public abstract string Name { get; }
                
                /* Icon to put in the TreeView */
                public abstract Gdk.Pixbuf Pixbuf { get; }

                /* Icon to use during drag opsw */
                public abstract Gdk.Pixbuf DragPixbuf { get; }

                // Properties //////////////////////////////////////////////////

                public string TagsString {
                        get {
                                string s = String.Empty;
                                foreach (Tag tag in appliedTags)
                                        s += (tag.Name + " ");
                                
                                s = s.TrimEnd (' ');
                                return s;
                        }
                }

                public bool HasNoTags {
                        get { return (appliedTags.Count == 0) ? true : false ; }
                }

                public IEnumerator TagsEnum {
                        get { return appliedTags.GetEnumerator (); }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Stuff ()
                {
                        appliedTags = new List <Tag> ();
                }

                /* Apply a given tag to the stuff */
                public void ApplyTag (Tag tag)
                {
                        if (appliedTags.Contains (tag))
                                return;

                        appliedTags.Add (tag);
                        tag.Change += OnAppliedTagChanged;
                        
                        if (AppliedTagChange != null)
                                AppliedTagChange (this, new AppliedTagArgs (tag, true));
                        if (Change != null)
                                Change (this, new StuffArgs (this));
                }

                /* Remove a given tag from the stuff */
                public void RemoveTag (Tag tag)
                {
                        if (! appliedTags.Contains (tag))
                                return;

                        appliedTags.Remove (tag);
                        tag.Change -= OnAppliedTagChanged;
                        
                        if (AppliedTagChange != null)
                                AppliedTagChange (this, new AppliedTagArgs (tag, false));
                        if (Change != null)
                                Change (this, new StuffArgs (this));
                }

                public bool HasTag (Tag tag)
                {
                        return appliedTags.Contains (tag);
                }
                
                public override string ToString ()
                {
                        return String.Format ("stuff:{0}", Major);
                }

                /* IBoilable */
                public virtual List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (Tag tag in appliedTags)
                                lst.Add (tag);
                        
                        return lst;
                }

                /* IBoilable */
                public virtual void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (Tag tag in appliedTags)
                                container.Add (new RefParameter ("appliedtag", tag, provider));
                }

                // Private methods ////////////////////////////////////////////
                
                void OnAppliedTagChanged (object o, TagArgs args)
                {
                        if (Change != null)
                                Change (this, new StuffArgs (this));
                }

                protected void ParseContainer (ObjectContainer container, IBoilProvider provider)
                {
                        appliedTags = new List <Tag> ();
                        
                        foreach (RefParameter reff in container.FindAllByName ("appliedtag"))
                                ApplyTag ((Tag) reff.ToObject (provider));
                }
                
                
        }
        
}
                
       
                                      
                                      
                                      
                
