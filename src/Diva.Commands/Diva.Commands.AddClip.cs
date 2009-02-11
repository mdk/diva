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
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;
        using Basics;
        using System.Collections.Generic;

        public class AddClip : ICommand, IUndoableCommand, IPreparableCommand,
                               IBoilable, IMessagingCommand {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Add clip '{0}' at {1}");

                readonly static string instantMessageSS = Catalog.GetString
                        ("'{0}' clip was placed at {1}");

                // Fields //////////////////////////////////////////////////////

                Clip clip;         // The clip in question
                Track track;       // The track in question
                TimeSpan timeSpan; // Time to add at
                string clipName;   // Saved clip name
                MediaItem item;    // Item used as a source
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, clipName, timeSpan); }
                }

                public string InstantMessage {
                        get { return String.Format (instantMessageSS, clipName, timeSpan); }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public AddClip (Track track, MediaItem item, TimeSpan timeSpan) : base ()
                {
                        this.track = track;
                        this.item = item;
                        this.timeSpan = timeSpan;
                }

                /* CONSTRUCTOR */
                public AddClip (ObjectContainer container, IBoilProvider provider)
                {
                        this.clip = (Gdv.Clip) container.FindRef ("clip").ToObject (provider);
                        this.item = (Gdv.MediaItem) container.FindRef ("mediaitem").ToObject (provider);
                        this.track = (Gdv.Track) container.FindRef ("track").ToObject (provider);

                        this.timeSpan = container.FindTimeSpan ("timespan").Value;
                        this.clipName = container.FindString ("clipname").Value;                        
                }
                
                public void Prepare (Project project)
                {
                        clip = new Clip (item);
                        clipName = clip.ParentItem.Name;
                }
                         

                public void DoAction (Project project)
                {
                        clip.PlaceAt (timeSpan);
                        track.AddClip (clip);
                        project.Clips.Add (clip);
                }

                public void UndoAction (Project project)
                {
                        track.RemoveClip (clip);
                        project.Clips.Remove (clip);
                }

                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (item, track, clip);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("mediaitem", item, provider));
                        container.Add (new RefParameter ("clip", clip, provider));
                        container.Add (new RefParameter ("track", track, provider));  

                        container.Add (new StringParameter ("clipname", clipName));
                        container.Add (new TimeSpanParameter ("timespan", timeSpan));                        
                }
                
        }

}

