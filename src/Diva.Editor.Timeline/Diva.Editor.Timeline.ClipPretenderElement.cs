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

namespace Diva.Editor.Timeline {
        
        using System;
        using Gtk;
        using Cairo;
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;

        public class ClipPretenderElement : ClipElement {
                
                // Fields //////////////////////////////////////////////////////
                
                // Properties //////////////////////////////////////////////////

                public override Time CurrentPosition {
                        get { return base.CurrentPosition; }
                        set {
                                TimeSpan testSpan = timeSpan;
                                TimeSpan outSpan = timeSpan;
                                testSpan.MoveTo (value);
                                if (! modelRoot.Tracks.IsSpanEmpty (track, testSpan)) {
                                        if (modelRoot.Tracks.SuggestPosition (track, testSpan, ref outSpan,
                                                                              modelRoot.Timeline.MouseOverTime)) 
                                                base.CurrentPosition = outSpan.Start;
                                        else
                                                return;
                                } else
                                        base.CurrentPosition = value;
                                
                        }
                }

                public override int CurrentTrack {
                        get { return base.CurrentTrack; }
                        set {
                                TimeSpan testSpan = timeSpan;
                                TimeSpan outSpan = timeSpan;
                                if (! modelRoot.Tracks.IsSpanEmpty (value, testSpan)) {
                                        if (modelRoot.Tracks.SuggestPosition (value, testSpan, ref outSpan,
                                                                              modelRoot.Timeline.MouseOverTime)) {
                                                base.CurrentPosition = outSpan.Start;
                                                base.CurrentTrack = value;
                                        } else
                                                return;
                                } else
                                        base.CurrentTrack = value;
                                
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                public static ClipPretenderElement NewFromMediaItem (Model.Root root,
                                                                     Time time, 
                                                                     int track, 
                                                                     MediaItem item)
                {
                        TimeSpan span;
                        
                        if (item is ITimeable) 
                                span = new TimeSpan (time, time + (item as ITimeable).Length);
                        else 
                                span = new TimeSpan (time, time + Time.FromSeconds (1));
                        
                        return new ClipPretenderElement (root, span, track, item);
                }
                
                public override void SyncThyself ()
                {
                        Core.ICommand cmd = new Commands.AddClip (modelRoot.Tracks.GetByNo (track), mediaItem, timeSpan);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                                                
                        base.SyncThyself ();
                }
                                
                // Private methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                protected ClipPretenderElement (Model.Root root, TimeSpan span, int track, MediaItem item) :
                base (root, span, track, item)
                {
                }

        }
        
}
