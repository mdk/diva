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

        using Gdv;
        using System;
        using Core;
        using Mono.Unix;
        using Basics;
        using System.Collections.Generic;

        public class ChangeOpacity : ICommand, IUndoableCommand, IBoilable,
                                     IMessagingCommand, IPreparableCommand {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("Change track opacity to {0}");

                readonly static string instantMessageSS = Catalog.GetString
                        ("Track opacity was changed to {0}");

                readonly static string technoSS = Catalog.GetString
                        ("techno mode");
                
                // Fields //////////////////////////////////////////////////////

                Track track;          // Track in question
                double oldValue;      // Saved value
                double newValue;      // The new value
                
                // Properties //////////////////////////////////////////////////

                public string Message {
                        get {
                                string v = (newValue != -1.0) ?
                                        String.Format ("{0}%", newValue * 100) :
                                        technoSS;
                                        
                                return String.Format (messageSS, v);
                        }
                }

                public string InstantMessage {
                        get {
                                string v = (newValue != -1.0) ?
                                        String.Format ("{0}%", newValue * 100) :
                                        technoSS;

                                return String.Format (instantMessageSS, v);
                        }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ChangeOpacity (Track track, double newValue) : base ()
                {
                        this.track = track;
                        this.newValue = newValue;
                }

                /* CONSTRUCTOR */
                public ChangeOpacity (ObjectContainer container, IBoilProvider provider)
                {
                        this.track = (Track) container.FindRef ("track").ToObject (provider);
                        this.newValue = container.FindDouble ("new").Value;
                        this.oldValue = container.FindDouble ("old").Value;
                }

                public void Prepare (Project project)
                {
                        oldValue = track.Opacity;
                }

                public void DoAction (Project project)
                {
                        track.Opacity = newValue;
                }

                public void UndoAction (Project project)
                {
                        track.Opacity = oldValue;
                }
                
                public List <object> GetDepObjects ()
                {
                        return BoilFactory.GetList (track);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("track", track, provider));
                        container.Add (new DoubleParameter ("new", newValue));
                        container.Add (new DoubleParameter ("old", oldValue));
                }

        }

}