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
        using System.Collections.Generic;
        using Widgets;
        using Basics;

        public class AddManyFilesToProject : ICommand, IUndoableCommand,
                                             ITaskPreparableCommand, ICursorWaiting,
                                             IBoilable, IMessagingCommand {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Add {0} files to the project");

                readonly static string instantMessageSS = Catalog.GetString
                        ("{0} files were added to the project");

                // Fields //////////////////////////////////////////////////////
                
                LoaderTask task;    // Our execution task
                int filesCount;     // files count. For message
                string[] fileNames; // Filenames to load
                
                List <Stuff> stuffList = null;
                List <MediaItem> mediaItemsList = null;
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, mediaItemsList.Count); }   
                }

                public string InstantMessage {
                        get { return String.Format (instantMessageSS, mediaItemsList.Count); }
                }
                
                // public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public AddManyFilesToProject (string[] fileNames)
                {
                        this.fileNames = fileNames;
                }

                /* CONSTRUCTOR */
                public AddManyFilesToProject (ObjectContainer container, IBoilProvider provider)
                {
                        mediaItemsList = new List <MediaItem> ();
                        stuffList = new List <Stuff> ();                        
                        
                        foreach (RefParameter reff in container.FindAllByName ("mediaitem"))
                                mediaItemsList.Add ((MediaItem) reff.ToObject (provider));
                        
                        foreach (RefParameter reff in container.FindAllByName ("stuff"))
                                stuffList.Add ((Stuff) reff.ToObject (provider));
                }

                /* Load here */
                public Task Prepare (Project project)
                {
                        task = new LoaderTask (project.Format, fileNames);
                        return task;
                }

                public void PostPrepare (Project project)
                {
                        mediaItemsList = task.MediaItems;
                        stuffList = new List <Stuff> ();
                        foreach (MediaItem item in mediaItemsList)
                                stuffList.Add (new MediaItemStuff (item));
                }

                public void DoAction (Project project)
                {
                        // Add stuff to project
                        foreach (Stuff stuff in stuffList) {
                                project.Stuff.Add (stuff);
                        }
                        
                        // Add mediaitems to project
                        foreach (MediaItem item in mediaItemsList) {
                                project.MediaItems.Add (item);
                        }
                }

                public void UndoAction (Project project)
                {
                        // Remove stuff from project
                        foreach (Stuff stuff in stuffList)
                                project.Stuff.Remove (stuff);
                        
                        // Remove mediaitems from project
                        foreach (MediaItem item in mediaItemsList)
                                project.MediaItems.Remove (item);
                }

                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        
                        foreach (MediaItem item in mediaItemsList)
                                lst.Add (item);
                        
                        foreach (Stuff stuff in stuffList)
                                lst.Add (stuff);

                        return lst;
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (MediaItem item in mediaItemsList)
                                container.Add (new RefParameter ("mediaitem", item, provider));
                        
                        foreach (Stuff stuff in stuffList)
                                container.Add (new RefParameter ("stuff", stuff, provider));
                }
                
        }

}
