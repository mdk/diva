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

namespace Diva.Editor.Model {
        
        using System;
        using System.Collections;
        using Commands;
        using Mono.Unix;
        using Gdv;
        
        public sealed class Tracks : IModelPart, IEnumerable {
                
                // Translatable ////////////////////////////////////////////////

                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                
                // Events //////////////////////////////////////////////////////
                
                // Properties //////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public int Count {
                        get { return modelRoot.Project.Tracks.Count; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Tracks (Root root)
                {
                        modelRoot = root;
                }
                
                /* IEnumerable */
                public IEnumerator GetEnumerator () 
                {
                        return modelRoot.Project.Tracks.GetEnumerator ();
                }

                public Track GetByNo (int track)
                {
                        return modelRoot.Project.Tracks [track];
                }

                public int NoByTrack (Track track)
                {
                        if (track == null)
                                throw new Exception ("Trying to find a null track");
                        
                        int i = 0;
                        foreach (Track t in modelRoot.Project.Tracks) {
                                if (t == track)
                                        return i;
                                i++;
                        }
                        
                        throw new Exception ("Track not found");
                }

                public bool IsSpanEmpty (int trackno, Gdv.TimeSpan span)
                {
                        Track track = GetByNo (trackno);
                        return track.ClipStore.IsSpanEmpty (span);
                }

                public bool SuggestPosition (int trackno, Gdv.TimeSpan span, ref Gdv.TimeSpan outspan,
                                             Time time)
                {
                        Track track = GetByNo (trackno);
                        return track.ClipStore.SuggestPosition (span, ref outspan, time);
                }

                public bool SuggestLeftAdjustPosition (int trackno, Clip clip, ref Time t)
                {
                        Track track = GetByNo (trackno);
                        Clip prevClip = track.ClipStore.GetClipBeforeClip (clip);
                        if (prevClip == null)
                                return false;
                        else {
                                t = prevClip.TimelineSpan.End;
                                return true;
                        }
                }

                public bool SuggestRightAdjustPosition (int trackno, Clip clip, ref Time t)
                {
                        Track track = GetByNo (trackno);
                        Clip nextClip = track.ClipStore.GetClipAfterClip (clip);
                        if (nextClip == null)
                                return false;
                        else {
                                t = nextClip.TimelineSpan.Start;
                                return true;
                        }
                }

                public bool IsSpanEmptyExcluding (int trackno, Gdv.TimeSpan span, Clip clip)
                {
                        Track track = GetByNo (trackno);
                        return track.ClipStore.IsSpanEmptyExcluding (span, clip);
                }

                public bool SuggestPositionExcluding (int trackno, Gdv.TimeSpan span, ref Gdv.TimeSpan outspan,
                                                      Time time, Clip clip)
                {
                        Track track = GetByNo (trackno);
                        return track.ClipStore.SuggestPositionExcluding (span, ref outspan, time, clip);
                }

                public Time GetTimeBeforeTime (Time current)
                {
                        Time minTime = 0;
                        foreach (Track track in modelRoot.Project.Tracks)
                                foreach (Clip clip in track.Clips) {
                                        if (clip.TimelineIn > minTime &&
                                            clip.TimelineIn < current)
                                                minTime = clip.TimelineIn;

                                        if (clip.TimelineOut > minTime &&
                                            clip.TimelineOut < current)
                                                minTime = clip.TimelineOut;
                                }
                        
                        return minTime;
                }

                public Time GetTimeAfterTime (Time current)
                {
                        // FIXME: "Infinity" should go here!
                        Time maxTime = Time.FromSeconds (3600 * 10);
                        foreach (Track track in modelRoot.Project.Tracks)
                                foreach (Clip clip in track.Clips) {
                                        if (clip.TimelineIn < maxTime &&
                                            clip.TimelineIn > current)
                                                maxTime = clip.TimelineIn;
                                        
                                        if (clip.TimelineOut < maxTime &&
                                            clip.TimelineOut > current)
                                                maxTime = clip.TimelineOut;
                                }

                        if (maxTime == Time.FromSeconds (3600 * 10))
                                return Time.Zero;
                        else
                                return maxTime;
                }
                
                // Private methods /////////////////////////////////////////////
                                                
        }
        
}
