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
        using Mono.Unix;
        using Gdv;
        
        public class VideoFrame : AspectFrame {

                // Fields //////////////////////////////////////////////////////

                DrawingArea videoArea; // the drawing area that will contain the overlay
                Model.Root modelRoot;
                bool fullscreen;       // If we're in fullscreen mode
                                
                // Properties //////////////////////////////////////////////////

                public Gdk.Window GdkWindowTarget {
                        get { return videoArea.GdkWindow; }
                }

                public bool Fullscreen {
                        get { return fullscreen; }
                        set {
                                if (value)
                                        SetUpRefreshIdle ();
                                
                                ShadowType = (value == true) ? ShadowType.None : ShadowType.In;
                                fullscreen = value;
                                QueueDraw ();
                        }
                }
                
                // Public methods //////////////////////////////////////////////

                public VideoFrame (Model.Root modelRoot) :
                base (null, 0.5f, 0.5f, (float) (double) modelRoot.ProjectDetails.Format.VideoFormat.FrameAspect, false)
                {
                        this.modelRoot = modelRoot;
                        videoArea = new DrawingArea ();
                        videoArea.ExposeEvent += OnVideoAreaExposed;
                        videoArea.SizeRequested += OnVideoAreaSizeRequested;

                        // This is required for our process of controlling the exposisions
                        // of the overlay
                        videoArea.DoubleBuffered = false;
                        
                        Add (videoArea);
                }

                // Private /////////////////////////////////////////////////////

                void SetUpRefreshIdle ()
                {
                        GLib.Idle.Add (OnRefreshIdle);
                }

                void OnVideoAreaExposed (object o, ExposeEventArgs args)
                {
                        if (modelRoot.Pipeline.Launched == false && 
                            modelRoot.Pipeline.Shutdowned == false) {
                                modelRoot.Pipeline.Launch (videoArea.GdkWindow);
                                SetUpRefreshIdle ();
                                return;
                        }
                                
                        // Force the exposition
                        modelRoot.Pipeline.ForceExpose ();
                }
                
                void OnVideoAreaSizeRequested (object o, SizeRequestedArgs args)
                {
                        if (args.Requisition.Width == 0 &&
                            args.Requisition.Height == 0) {
                                Requisition req = args.Requisition;
                                req.Width = modelRoot.ProjectDetails.Format.VideoFormat.FrameDimensions.Width / 10;
                                req.Height = modelRoot.ProjectDetails.Format.VideoFormat.FrameDimensions.Height / 10;
                                args.Requisition = req;
                        }
                }

                protected override bool OnExposeEvent (Gdk.EventExpose evnt)
                {
                        if (fullscreen)
                                GdkWindow.DrawRectangle (Style.BlackGC, true,
                                                         evnt.Region.Clipbox);
                        
                        return base.OnExposeEvent (evnt);
                }

                protected override bool OnKeyPressEvent (Gdk.EventKey evnt)
                {
                        throw new Exception ("Key pressed!");
                }

                protected bool OnRefreshIdle ()
                {
                        Gdk.Threads.Enter ();
                        modelRoot.Pipeline.ForceExpose ();
                        Gdk.Threads.Leave ();

                        return false;
                }

        }

}
