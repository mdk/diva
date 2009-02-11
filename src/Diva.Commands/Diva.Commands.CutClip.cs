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

        public class CutClip : ICommand, IUndoableCommand, IPreparableCommand,
                               IBoilable, IMessagingCommand {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Cut clip '{0}' at {1}");

                readonly static string instantMessageSS = Catalog.GetString
                        ("Clip '{0}' was cutted at {1}");

                // Fields //////////////////////////////////////////////////////

                Clip cuttedClip;   // Clip we're cutting
                Track track;       // Track we're operating on
                Gdv.Time cutTime;  // The cut time
                string clipName;   // Saved clip name
                Clip leftClip;
                Clip rightClip;
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, clipName, cutTime); }
                }

                public string InstantMessage {
                        get { return String.Format (instantMessageSS, clipName, cutTime); }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public CutClip (Clip clip, Time time)
                {
                        cuttedClip = clip;
                        cutTime = time;
                        track = null;
                        leftClip = null;
                        rightClip = null;
                }

                /* CONSTRUCTOR */
                public CutClip (ObjectContainer container, IBoilProvider provider)
                {
                        this.clipName = container.FindString ("name").Value;
                        this.cuttedClip = (Clip) container.FindRef ("cuttedclip").ToObject (provider);
                        this.leftClip = (Clip) container.FindRef ("leftclip").ToObject (provider);
                        this.rightClip = (Clip) container.FindRef ("rightclip").ToObject (provider);
                        this.track = (Track) container.FindRef ("track").ToObject (provider);
                        this.cutTime = container.FindTime ("cuttime").Value;
                }
                
                public void Prepare (Project project)
                {
                        track = cuttedClip.Track;
                        clipName = cuttedClip.ParentItem.Name;
                        
                        Time sourceCutTime = cuttedClip.ClipTimeToSourceTime (cutTime);
                        
                        TimeSpan leftSourceSpan = new TimeSpan (cuttedClip.SourceSpan.Start,
                                                                sourceCutTime);
                        TimeSpan rightSourceSpan = new TimeSpan (sourceCutTime,
                                                                 cuttedClip.SourceSpan.End);
                        TimeSpan leftTimelineSpan = new TimeSpan (cuttedClip.TimelineSpan.Start,
                                                                  cutTime);
                        TimeSpan rightTimelineSpan = new TimeSpan (cutTime,
                                                                   cuttedClip.TimelineSpan.End);

                        MediaItem item = cuttedClip.ParentItem;
                        leftClip = new Clip (item);
                        rightClip = new Clip (item);

                        leftClip.TimelineSpan = leftTimelineSpan;
                        leftClip.SourceSpan = leftSourceSpan;

                        rightClip.TimelineSpan = rightTimelineSpan;
                        rightClip.SourceSpan = rightSourceSpan;
                        
                }

                public void DoAction (Project project)
                {
                        track.RemoveClip (cuttedClip);
                        project.Clips.Remove (cuttedClip);
                        
                        track.AddClip (leftClip);
                        track.AddClip (rightClip);

                        project.Clips.Add (leftClip);
                        project.Clips.Add (rightClip);                        
                }

                public void UndoAction (Project project)
                {
                        track.RemoveClip (leftClip);
                        track.RemoveClip (rightClip);
                        
                        project.Clips.Remove (leftClip);
                        project.Clips.Remove (rightClip);
                        
                        track.AddClip (cuttedClip);
                        project.Clips.Add (cuttedClip);
                }
                
                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (cuttedClip, leftClip, rightClip, track);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("cuttedclip", cuttedClip, provider));
                        container.Add (new RefParameter ("leftclip", leftClip, provider));
                        container.Add (new RefParameter ("rightclip", rightClip, provider));
                        container.Add (new StringParameter ("name", clipName));
                        container.Add (new TimeParameter ("cuttime", cutTime));
                        container.Add (new RefParameter ("track", track, provider));
                }
                
        }

}

