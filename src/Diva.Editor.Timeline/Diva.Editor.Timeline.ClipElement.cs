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
        using Util;

        public abstract class ClipElement : ViewElement, IMovable, IFloating, ITracking {
                
                // Fields //////////////////////////////////////////////////////
                
                protected TimeSpan timeSpan;
                protected TimeSpan backupSpan;
                protected int track;
                protected int backupTrack;
                protected MediaItem mediaItem; // For display only
                                
                // Properties //////////////////////////////////////////////////
                
                public override Gdk.Rectangle DrawingRect {
                        get {
                                // If out of view range, return zero
                                Gdk.Rectangle totalRect = modelRoot.Timeline.ViewportRectangle;
                                if (! (timeSpan.IntersectsWith (modelRoot.Timeline.DisplayedTimeSpan)))
                                        return Gdk.Rectangle.Zero;
                                
                                int startX = Math.Max (totalRect.Left,
                                                       modelRoot.Timeline.TimeToPixel (timeSpan.Start));
                                int endX = Math.Min (totalRect.Width,
                                                     modelRoot.Timeline.TimeToPixel (timeSpan.End));
                                
                                return new Gdk.Rectangle (startX, Helper.GetYForTrackNo (modelRoot, track),
                                                          endX - startX + 1, 32);
                        }
                }
                
                // FIXME: Make private?
                public bool LeftCut {
                        get {
                                return (timeSpan.Start < modelRoot.Timeline.DisplayedTimeSpan.Start)
                                        ? true : false;
                        }
                }
                
                // FIXME: Make private?
                public bool RightCut {
                        get {
                                return (timeSpan.End > modelRoot.Timeline.DisplayedTimeSpan.End)
                                        ? true : false;
                        }
                }
                
                public virtual Time CurrentPosition {
                        get { return timeSpan.Start; }
                        set {
                                timeSpan.MoveTo (value);
                                Invalidate ();
                        }
                }
                
                public Time SavedPosition {
                        get { return backupSpan.Start ; }
                }

                public virtual int CurrentTrack {
                        get { return track; }
                        set {
                                track = value;
                                Invalidate ();
                        }
                }

                public int SavedTrack {
                        get { return backupTrack; }
                }

                Gdv.Color MediaItemColor {
                        get {
                                if (mediaItem is Gdv.AVItem)
                                        return Gdv.Color.FromTango (ColorTango.Chameleon1);
                                else if (mediaItem is Gdv.PhotoItem)
                                        return Gdv.Color.FromTango (ColorTango.Butter1);
                                else if (mediaItem is Gdv.AudioItem)
                                        return Gdv.Color.FromTango (ColorTango.SkyBlue1);
                                else
                                        return Gdv.Color.FromTango (ColorTango.Chameleon1);
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ClipElement (Model.Root modelRoot, TimeSpan span, int track, MediaItem item) :
                base (modelRoot, ElementLayer.Clip)
                {
                        timeSpan = span;
                        backupSpan = TimeSpan.Empty;
                        this.track = track;
                        backupTrack = -1;
                        mediaItem = item;
                }

                public override void Draw (Graphics gr, Gdk.Rectangle rect)
                {
                        Gdk.Rectangle clipRect = DrawingRect;
                        CairoCut cutMode = CairoCut.None;
                        CairoState stateMode = CairoState.Normal;
                        
                        // Check the cuts
                        if (LeftCut)
                                cutMode = cutMode | CairoCut.Left;
                        if (RightCut)
                                cutMode = cutMode | CairoCut.Right;
                        
                        // Check the state
                        // FIXME: We've got a helper in base class for that, don't we?
                        switch (state) {
                                
                                case ViewElementState.Active:
                                stateMode = CairoState.Active;
                                break;
                                
                                case ViewElementState.Drag:
                                stateMode = CairoState.Drag;
                                break;
                                
                                default:
                                stateMode = CairoState.Normal;
                                break;
                                
                        }

                        // Get a formatted string
                        string str = String.Format ("<b>{0}</b>", StringFu.Markupize (mediaItem.Name));
                        
                        // Draw
                        if (mediaItem is IThumbnailable && (mediaItem as IThumbnailable).SmallThumbnail != null)
                                ReadyMade.Clip (gr, clipRect.Left, clipRect.Top, clipRect.Width, 
                                                MediaItemColor, 
                                                str, (mediaItem as IThumbnailable).SmallThumbnail,
                                                cutMode, stateMode);
                        else
                                ReadyMade.Clip (gr, clipRect.Left, clipRect.Top, clipRect.Width, 
                                                MediaItemColor, 
                                                str,
                                                cutMode, stateMode);
                                    
                            
                        
                        base.Draw (gr, rect);
                }
                
                /* IFloating */
                public void SaveThyself ()
                {
                        backupSpan = timeSpan;
                        backupTrack = track;
                }
                
                /* IFloating */
                public void RestoreThyself ()
                {
                        timeSpan = backupSpan;
                        track = backupTrack;
                }
                
                /* IFloating */
                public void LooseThyself ()
                {
                        backupSpan = TimeSpan.Empty;
                        backupTrack = -1;
                }
                
                /* IFloating */
                public virtual void SyncThyself ()
                {
                        LooseThyself ();                        
                }
                
        }
        
}
