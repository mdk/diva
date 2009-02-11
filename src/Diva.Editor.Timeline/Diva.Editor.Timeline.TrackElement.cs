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
        using Gdk;
        using Diva;
        using Gdv;
        using Cairo;
        using Mono.Unix;

        public class TrackElement : ViewElement {

                // Translatable /////////////////////////////////////////////
                
                readonly static string technoModeSS = Catalog.GetString 
                        ("techno mode");

                readonly static string opacitySS = Catalog.GetString 
                        ("<small>Opacity: {0}</small>");

                // Fields //////////////////////////////////////////////////////

                Track track;
                Gdv.Color color;
                
                // Properties //////////////////////////////////////////////////
                
                public override Rectangle DrawingRect {
                        get {
                                int no = modelRoot.Tracks.NoByTrack (track);
                                int y = Helper.GetYForTrackNo (modelRoot, no);
                                                                
                                Gdk.Rectangle full = modelRoot.Timeline.ViewportRectangle;
                                return new Gdk.Rectangle (full.Left, y + 1,
                                                          full.Width, 30);
                        }
                }
                
                public Track Track {
                        get { return track; }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TrackElement (Model.Root modelRoot, Track track) :
                base (modelRoot, ElementLayer.Track)
                {
                        this.track = track;

                        color = Gdv.Color.FromTango (ColorTango.Aluminium4);
                        color.Alpha = 0.5f;
                }
                
                public override void Draw (Graphics gr, Rectangle rect)
                {
                        if (state == ViewElementState.Active)
                                Cairo.Draw.SolidRect (gr, rect.Left, rect.Top, rect.Width, rect.Height, color);

                        if (track.Opacity != 1.0) {
                                string text = String.Format (opacitySS,
                                                             (track.Opacity != -1.0) ?
                                                             String.Format ("{0}%", track.Opacity * 100) :
                                                             technoModeSS); 
                                
                                Pango.Layout layout = new Pango.Layout (gr.Context);
                                
                                layout.Width = Pango.Units.FromPixels (DrawingRect.Width - 3);
                                layout.Alignment = Pango.Alignment.Right;
                                layout.SetMarkup (text);
                                
                                int pWidth, pHeight;
                                layout.GetSize (out pWidth, out pHeight);
                                pHeight = Pango.Units.ToPixels (pHeight);
                                
                                gr.Window.DrawLayout (gr.Style.BlackGC, 0,
                                                      DrawingRect.Bottom - pHeight - 3,
                                                      layout);
                        }
                        
                        base.Draw (gr, rect);
                }

        }
        
}