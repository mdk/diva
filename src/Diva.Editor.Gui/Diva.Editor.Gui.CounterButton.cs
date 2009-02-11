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

namespace Diva.Editor.Gui {
        
        using Gtk;
        using System;
        using Util;
        
        public class CounterButton : Button {

                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                
                // Properties //////////////////////////////////////////////////
                
                public Gdv.Time displayedTime {
                        set { 
                                (Child as Label).Markup = 
                                        String.Format ("<small>{0}</small>", 
                                                       TimeFu.ToSMPTE (value, 
                                                                       modelRoot.ProjectDetails.Format.VideoFormat.Fps,
                                                                       false));
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CounterButton (Model.Root root) : base ("")
                {
                        modelRoot = root;
                        displayedTime = modelRoot.Timeline.DisplayedTimeSpan.Start;
                        
                        // Model bind
                        modelRoot.Timeline.DisplayedTimeSpanChange += OnDisplayedTimeSpanChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                public void OnDisplayedTimeSpanChanged (object o, Model.TimeSpanArgs args)
                {
                        displayedTime = args.TimeSpan.Start;
                }
                
        }
        
}
                
                
