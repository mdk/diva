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
        
        using System;
        using Gtk;
                
        public class TimelineHScrollbar : HScrollbar {
                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                bool trickSwitch = true;     // We start with a trick-switch on 
                                             // because of constructing
                uint timeout = 0;            // We set with a delay

                readonly static uint timeoutPeriod = 25;
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TimelineHScrollbar (Model.Root root) :
                base (new Adjustment (root.Timeline.DisplayedTimeSpan.Start.Seconds, 
                                      0.0, 3600.0, 1.0, 10.0, 1.0))
                {
                        modelRoot = root;
                                                
                        Adjustment adj = Adjustment;
                        CalculateAdjustment (root.Timeline.DisplayedTimeSpan, ref adj);
                        trickSwitch = false;
                        
                        // Model bind
                        modelRoot.Timeline.DisplayedTimeSpanChange += OnDisplayedTimeSpanChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                static void CalculateAdjustment (Gdv.TimeSpan timeSpan, ref Adjustment adj)
                {
                        adj.Value = timeSpan.Start.Seconds;
                        adj.Lower = 0.0;
                        adj.Upper = Math.Max (timeSpan.Duration.Seconds * 1.2, timeSpan.End.Seconds);
                        adj.StepIncrement = 0.1;
                        adj.PageIncrement = timeSpan.Duration.Seconds / 10;
                        adj.PageSize = 2.0;
                        
                        adj.Change ();
                        adj.ChangeValue ();
                }
                
                void OnDisplayedTimeSpanChanged (object o, Model.TimeSpanArgs args)
                {
                        trickSwitch = true;
                        Adjustment adj = Adjustment;
                        CalculateAdjustment (args.TimeSpan, ref adj);
                }
                
                protected override void OnValueChanged ()
                {
                        if (trickSwitch) {
                                trickSwitch = false;
                                return;
                        }

                        if (timeout == 0) 
                                timeout = GLib.Timeout.Add (timeoutPeriod, OnTimeout);
                        
                        Gdv.TimeSpan span = modelRoot.Timeline.DisplayedTimeSpan;
                        span.MoveTo (Gdv.Time.FromSeconds (Value));
                        
                        trickSwitch = true;
                        Adjustment adj = Adjustment;
                        CalculateAdjustment (span, ref adj); 
                }

                bool OnTimeout ()
                {
                        Gdv.TimeSpan span = modelRoot.Timeline.DisplayedTimeSpan;
                        span.MoveTo (Gdv.Time.FromSeconds (Value));
                        modelRoot.Timeline.DisplayedTimeSpan = span;
                        timeout = 0;

                        return false;
                }
                                                
        }
                
}
       
