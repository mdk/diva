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
        using Gdv;
        using Mono.Unix;
        using Widgets;
        using Basics;
        using System.Collections.Generic;

        public class AddFileToProject : ICommand, IUndoableCommand,
                                        ITaskPreparableCommand, ICursorWaiting,
                                        IBoilable, IMessagingCommand {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Add '{0}' to the project");

                readonly static string instantMessageSS = Catalog.GetString
                        ("'{0}' was added to the project");

                // Fields //////////////////////////////////////////////////////

                LoaderTask task;          // Our execution task
                MediaItem item;           // Item we're adding
                Stuff stuff;              // Stuff we're adding
                string mediaItemName;     // Name
                string fileName;          // Filename to load
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, mediaItemName); }
                }

                public string InstantMessage {
                        get { return String.Format (instantMessageSS, mediaItemName); }
                }
                
                public Stuff Stuff {
                        get { return stuff; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public AddFileToProject (string fileName)
                {
                        this.mediaItemName = String.Empty;
                        this.fileName = fileName;
                        this.task = null;
                }

                public AddFileToProject (ObjectContainer container, IBoilProvider provider)
                {
                        this.mediaItemName = container.FindString ("name").Value;
                        this.item = (MediaItem) container.FindRef ("mediaitem").ToObject (provider);
                        this.stuff = (Stuff) container.FindRef ("stuff").ToObject (provider);                        
                        this.fileName = String.Empty;
                        this.task = null;
                }

                public Task Prepare (Project project)
                {
                        string [] arr = new string [1] { fileName };
                        task = new LoaderTask (project.Format, arr);
                        return task;
                }
                
                public void PostPrepare (Project project)
                {
                        item = task.MediaItems [0];
                        stuff = new MediaItemStuff (item);
                        mediaItemName = stuff.Name;
                }

                public void DoAction (Project project)
                {
                        project.Stuff.Add (stuff);
                        project.MediaItems.Add (item);
                }

                public void UndoAction (Project project)
                {
                        project.Stuff.Remove (stuff);
                        project.MediaItems.Remove (item);
                }

                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (item, stuff);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("stuff", stuff, provider));
                        container.Add (new RefParameter ("mediaitem", item, provider));
                        container.Add (new StringParameter ("name", mediaItemName));
                }

        }

}

