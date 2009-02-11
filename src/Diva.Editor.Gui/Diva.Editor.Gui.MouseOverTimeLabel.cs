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
        
        public sealed class MouseOverTimeLabel : Label {
                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null; 
                
                // Properties //////////////////////////////////////////////////
                
                public Gdv.Time Time {
                        set { 
                                if (value != Gdv.Time.Empty) 
                                        Text = String.Format ("{0}",
                                                              TimeFu.ToSMPTE (value,
                                                                              modelRoot.ProjectDetails.Format.VideoFormat.Fps,
                                                                              false));
                                else 
                                        Text = String.Empty;
                        }
                }
                
                // Public methods //////////////////////////////////////////////

                public MouseOverTimeLabel (Model.Root root) : base ("")
                {
                        modelRoot = root;
                        Time = modelRoot.Timeline.MouseOverTime;
                        
                        // Model bind
                        modelRoot.Timeline.MouseOverTimeChange += OnMouseOverTimeChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnMouseOverTimeChanged (object o, Model.TimeArgs args)
                {
                        Time = args.Time;
                }
                
        }
                
        
}
       
