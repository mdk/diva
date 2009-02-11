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
        
        public class ClipTrueElement : ClipElement, ILeftAdjustable, IRightAdjustable {
                
                // Fields //////////////////////////////////////////////////////

                Clip clip; // Clip we're representing
                Time leftCut;
                Time rightCut;
                bool leftCutValid;
                bool rightCutValid;
                bool positionValid;
                bool trackValid;
                
                // Properties //////////////////////////////////////////////////
                
                public override Time CurrentPosition {
                        get { return base.CurrentPosition; }
                        set {
                                TimeSpan testSpan = timeSpan;
                                TimeSpan outSpan = timeSpan;
                                testSpan.MoveTo (value);
                                if (! modelRoot.Tracks.IsSpanEmptyExcluding (track, testSpan, clip)) {
                                        if (modelRoot.Tracks.SuggestPositionExcluding (track, testSpan, ref outSpan,
                                                                                       modelRoot.Timeline.MouseOverTime,
                                                                                       clip)) 
                                                base.CurrentPosition = outSpan.Start;
                                        else
                                                return;
                                } else
                                        base.CurrentPosition = value;

                                positionValid = true;
                        }
                }

                public override int CurrentTrack {
                        get { return base.CurrentTrack; }
                        set {
                                // FIXME: Temporarily disabled, needs functions to remove clip
                                // from store/clip
                                
                                TimeSpan testSpan = timeSpan;
                                TimeSpan outSpan = timeSpan;
                                if (! modelRoot.Tracks.IsSpanEmptyExcluding (value, testSpan, clip)) {
                                        if (modelRoot.Tracks.SuggestPositionExcluding (value, testSpan, ref outSpan,
                                                                                       modelRoot.Timeline.MouseOverTime,
                                                                                       clip)) {
                                                base.CurrentPosition = outSpan.Start;
                                                base.CurrentTrack = value;
                                        } else
                                                return;
                                } else
                                        base.CurrentTrack = value;

                                trackValid = true;
                        }
                }

                public Clip Clip {
                        get { return clip; }
                }

                public Time CurrentLeft {
                        get { return timeSpan.Start; }
                        set {
                                // FIXME: Check for 1 frame min duration
                                if (value < clip.MinTimelineIn)
                                        value = clip.MinTimelineIn;

                                TimeSpan testSpan = timeSpan;
                                testSpan.Start = value;
                                Time t = Time.Zero;

                                if (! modelRoot.Tracks.IsSpanEmptyExcluding (track, testSpan, clip)) {
                                        if (modelRoot.Tracks.SuggestLeftAdjustPosition (track, clip, ref t))
                                                testSpan.Start = t;
                                        else
                                                return;
                                }

                                timeSpan = testSpan;
                                leftCutValid = true;
                                leftCut = testSpan.Start;
                                Invalidate ();
                        }

                }

                public Time CurrentLeftNoCheck {
                        get { return timeSpan.Start; }
                        set {
                                // FIXME: Check for 1 frame min duration
                                if (value < clip.MinTimelineIn)
                                        value = clip.MinTimelineIn;

                                TimeSpan testSpan = timeSpan;
                                testSpan.Start = value;
                                
                                timeSpan = testSpan;
                                leftCutValid = true;
                                leftCut = testSpan.Start;
                                Invalidate ();
                        }

                }

                public Time SavedLeft {
                        get { return backupSpan.Start; }
                }

                public Time CurrentRight {
                        get { return timeSpan.End; }
                        set {
                                // FIXME: Check for 1 frame min duration
                                if (value > clip.MaxTimelineOut)
                                        value = clip.MaxTimelineOut;

                                TimeSpan testSpan = timeSpan;
                                testSpan.End = value;
                                Time t = Time.Zero;

                                if (! modelRoot.Tracks.IsSpanEmptyExcluding (track, testSpan, clip)) {
                                        if (modelRoot.Tracks.SuggestRightAdjustPosition (track, clip, ref t))
                                                testSpan.End = t;
                                        else
                                                return;
                                }

                                timeSpan = testSpan;
                                rightCutValid = true;
                                rightCut = testSpan.End;
                                Invalidate ();
                        }

                }

                public Time CurrentRightNoCheck {
                        get { return timeSpan.End; }
                        set {
                                // FIXME: Check for 1 frame min duration
                                if (value > clip.MaxTimelineOut)
                                        value = clip.MaxTimelineOut;

                                TimeSpan testSpan = timeSpan;
                                testSpan.End = value;
                                
                                timeSpan = testSpan;
                                rightCutValid = true;
                                rightCut = testSpan.End;
                                Invalidate ();
                        }

                }

                public Time SavedRight {
                        get { return backupSpan.End; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ClipTrueElement (Model.Root root, Clip clip) :
                base (root, clip.TimelineSpan, root.Tracks.NoByTrack (clip.Track), clip.ParentItem)
                {
                        this.clip = clip;
                        leftCutValid = false;
                        rightCutValid = false;
                        positionValid = false;
                        trackValid = false;
                        leftCut = Time.Zero;
                        rightCut = Time.Zero;
                        clip.Changed += OnClipChanged;
                }

                public override void SyncThyself ()
                {
                        // Track operation
                        if (positionValid && trackValid) {
                                Track track = modelRoot.Tracks.GetByNo (base.CurrentTrack);
                                Core.ICommand cmd = new Commands.TrackClip (clip, timeSpan.Start, track);
                                modelRoot.CommandProcessor.PushCommand (cmd);
                        } else if (positionValid) {
                                Core.ICommand cmd = new Commands.MoveClip (clip, timeSpan.Start);
                                modelRoot.CommandProcessor.PushCommand (cmd);
                        }

                        // Left adjust operation
                        if (leftCutValid) {
                                Core.ICommand cmd = new Commands.LeftAdjustClip (clip, leftCut);
                                modelRoot.CommandProcessor.PushCommand (cmd);
                        }

                        // Right adjust operation
                        if (rightCutValid) {
                                Core.ICommand cmd = new Commands.RightAdjustClip (clip, rightCut);
                                modelRoot.CommandProcessor.PushCommand (cmd);
                        }

                        // Reset
                        leftCutValid = false;
                        rightCutValid = false;
                        positionValid = false;
                        trackValid = false;
                        leftCut = Time.Zero;
                        rightCut = Time.Zero;
                        
                        base.SyncThyself ();
                }
                
                // Private methods /////////////////////////////////////////////
                
                public void OnClipChanged (object o, EventArgs args)
                {
                        // FIXME: Should invalidate only if different
                        // FIXME: This is one of the places where "Freezing" object notification would help
                        
                        timeSpan = clip.TimelineSpan;
                        if (clip.Track != null)
                                track = modelRoot.Tracks.NoByTrack (clip.Track);
                        Invalidate ();
                }

        }
        
}
