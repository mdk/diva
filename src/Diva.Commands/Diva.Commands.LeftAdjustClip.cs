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
        using Basics;
        using System.Collections.Generic;
        
        public class LeftAdjustClip : ICommand, IUndoableCommand, IPreparableCommand,
                                      IBoilable, IMessagingCommand {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Left adjust '{0}' to {1}");

                readonly static string instantMessageSS = Catalog.GetString
                        ("'{0}' was left adjusted to {1}");

                // Fields //////////////////////////////////////////////////////

                string clipName; // Saved clip name
                Clip clip;       // The clip in question
                Time newTime;    // Time to adjust to
                Time oldTime;    // Saved time 
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get { return String.Format (messageSS, clipName, newTime); }
                }

                public string InstantMessage {
                        get { return String.Format (instantMessageSS, clipName, newTime); }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public LeftAdjustClip (Clip clip, Time time) : base ()
                {
                        this.clip = clip;
                        this.newTime = time;
                }

                /* CONSTRUCTOR */
                public LeftAdjustClip (ObjectContainer container, IBoilProvider provider)
                {
                        this.clipName = container.FindString ("name").Value;
                        this.clip = (Clip) container.FindRef ("clip").ToObject (provider);
                        this.newTime = container.FindTime ("newtime").Value;
                        this.oldTime = container.FindTime ("oldtime").Value;
                }

                public void Prepare (Project project)
                {
                        oldTime = clip.TimelineSpan.Start;
                        clipName = clip.ParentItem.Name;
                }

                public void DoAction (Project project)
                {
                        clip.LeftAdjustTo (newTime);
                }

                public void UndoAction (Project project)
                {
                        clip.LeftAdjustTo (oldTime);
                }

                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (clip);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("clip", clip, provider));
                        container.Add (new StringParameter ("name", clipName));
                        container.Add (new TimeParameter ("newtime", newTime));
                        container.Add (new TimeParameter ("oldtime", oldTime));
                }
                
        }

}

