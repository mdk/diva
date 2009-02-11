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
        using System.Collections.Generic;
        using Basics;

        public class EditTagName : ICommand, IUndoableCommand, IBoilable {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Edit tag '{0}' ('{1}')");

                // Fields //////////////////////////////////////////////////////

                Tag tag;        // The tag in question
                string newName; // New name
                string oldName; // Stored old name
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, oldName, newName); }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public EditTagName (Tag tag, string newName) : base ()
                {
                        this.tag = tag;
                        this.newName = newName;
                        this.oldName = tag.Name;
                }

                /* CONSTRUCTOR */
                public EditTagName (ObjectContainer container, IBoilProvider provider)
                {
                        this.newName = container.FindString ("newname").Value;
                        this.oldName = container.FindString ("oldname").Value;
                        this.tag = (Tag) container.FindRef ("tag").ToObject (provider);
                }

                public void DoAction (Project project)
                {
                        tag.Name = newName;
                }

                public void UndoAction (Project project)
                {
                        tag.Name = oldName;
                }
                
                public static EditTagName NewUnboiled (Tag tag, string oldName, string newName)
                {
                        EditTagName editTagName = new EditTagName (tag, newName);
                        editTagName.oldName = oldName;
                        return editTagName;
                }
                
                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (tag);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("tag", tag, provider));
                        container.Add (new StringParameter ("newname", newName));
                        container.Add (new StringParameter ("oldname", oldName));
                }
                
        }

}

