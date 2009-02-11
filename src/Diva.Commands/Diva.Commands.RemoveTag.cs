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

namespace Diva.Commands {

        using System;
        using Core;
        using Mono.Unix;
        using Basics;
        using System.Collections.Generic;
        
        public class RemoveTag : ICommand, IUndoableCommand, IBoilable,
                                 IMessagingCommand {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Remove tag '{0}' from '{1}'");

                readonly static string instantMessageSS = Catalog.GetString
                        ("Tag '{0}' was removed from '{1}'");

                // Fields //////////////////////////////////////////////////////

                Tag tag;     // The tag in question
                Stuff stuff; // The stuff in question
                string tagName;   // Msg only
                string stuffName; // Msg only
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, tagName, stuffName); }
                }

                public string InstantMessage {
                        get { return String.Format (instantMessageSS, tagName, stuffName); }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public RemoveTag (Tag tag, Stuff stuff) : base ()
                {
                        this.tag = tag;
                        this.stuff = stuff;
                        this.tagName = tag.Name;
                        this.stuffName = stuff.Name;
                }

                /* CONSTRUCTOR */
                public RemoveTag (ObjectContainer container, IBoilProvider provider)
                {
                        this.tagName = container.FindString ("tagname").Value;
                        this.stuffName = container.FindString ("stuffname").Value;                        
                        this.tag = (Tag) container.FindRef ("tag").ToObject (provider);
                        this.stuff = (Stuff) container.FindRef ("stuff").ToObject (provider);
                }

                public void DoAction (Project project)
                {
                        stuff.RemoveTag (tag);
                }

                public void UndoAction (Project project)
                {
                        stuff.ApplyTag (tag);
                }
                
                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (tag, stuff);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("tag", tag, provider));
                        container.Add (new RefParameter ("stuff", stuff, provider));                        
                        container.Add (new StringParameter ("tagname", tagName));
                        container.Add (new StringParameter ("stuffname", stuffName));
                }
                
        }

}

