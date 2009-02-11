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
        
        using Gdv;
        using System;
        using TimeSpan = Gdv.TimeSpan;
        
        public sealed class Pipeline : IModelPart {
                
                // Events //////////////////////////////////////////////////////
                
                public event PipelineStateHandler StateChange;
                
                public event PipelineTickerHandler Ticker;
                
                public event PipelineTickerHandler NextSeekTicker;

                public event PipelineErrorHandler Error;

                public event PipelineVolumeHandler VolumeChange;

                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                Gdv.Time nextSeek;
                uint nextSeekTimeout = 0;
                                
                // Properties //////////////////////////////////////////////////

                public PipelineState State {
                        get { return modelRoot.Project.Pipeline.State; }
                }
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public bool Launched {
                        get { return modelRoot.Project.Pipeline.Launched; }
                }
                
                public bool Shutdowned {
                        get { return modelRoot.Project.Pipeline.Shutdowned; }
                }
                
                public Time CurrentTicker {
                        get { return modelRoot.Project.Pipeline.Ticker; }
                }

                public Time NextSeek {
                        get { return nextSeek; }
                        set {
                                nextSeek = value;
                                if (nextSeekTimeout != 0)
                                        GLib.Source.Remove (nextSeekTimeout);

                                nextSeekTimeout = GLib.Timeout.Add (500, OnNextSeekTimeout);

                                if (NextSeekTicker != null)
                                        NextSeekTicker (modelRoot, new PipelineTickerArgs (value));
                        }
                }
                
                public double Volume {
                        get { return modelRoot.Project.Pipeline.Volume; }
                        set { 
                                modelRoot.Project.Pipeline.Volume = value;
                                
                                if (VolumeChange != null)
                                        VolumeChange (modelRoot, new PipelineVolumeArgs (value));
                        }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Pipeline (Root root)
                {
                        modelRoot = root;
                        
                        modelRoot.Project.Pipeline.StateIdle += OnStateChanged;
                        modelRoot.Project.Pipeline.TickerIdle += OnTicker;
                        modelRoot.Project.Pipeline.ErrorIdle += OnError;
                }
                
                public void Launch (Gdk.Window window)
                {
                        if (Shutdowned == true)
                                return;
                        
                        modelRoot.Project.Pipeline.SetTargetWindow (window);
                        modelRoot.Project.Pipeline.Launch ();
                }
                
                public void Shutdown ()
                {
                        if (Shutdowned == true || Launched == false)
                                return;
                        
                        modelRoot.Project.Pipeline.Shutdown ();
                }
                
                public void ForceExpose ()
                {
                        if (Shutdowned == true || Launched == false)
                                return;
                        
                        modelRoot.Project.Pipeline.ForceExpose ();
                }
                
                public void PlayPause ()
                {
                        modelRoot.Project.Pipeline.PlayPause ();
                }
                
                public void Seek (Gdv.Time t)
                {
                        modelRoot.Project.Pipeline.Seek (t);
                }

                public void CommitNextSeek ()
                {
                        if (nextSeekTimeout != 0) {
                                Seek (nextSeek);
                                GLib.Source.Remove (nextSeekTimeout);
                                nextSeekTimeout = 0;
                        }
                }

                public void SeekToZero ()
                {
                        Seek (0);
                }

                public void SeekToPrevCut ()
                {
                        Time prevTime = modelRoot.Tracks.GetTimeBeforeTime (modelRoot.Project.Pipeline.Ticker);
                        Seek (prevTime);
                }

                public void SeekToNextCut ()
                {
                        Time nextTime = modelRoot.Tracks.GetTimeAfterTime (modelRoot.Project.Pipeline.Ticker);
                        if (nextTime != Time.Zero)
                                Seek (nextTime);
                }

                public void StartSingleView (Source src, Time t)
                {
                        modelRoot.Project.Pipeline.StartSingleView (src, t);
                }

                public void StopSingleView ()
                {
                        modelRoot.Project.Pipeline.StopSingleView ();
                }

                public void SeekSingleView (Time t)
                {
                        modelRoot.Project.Pipeline.SeekSingleView (t);
                }

                public void StartTwinView (Source src1, Source src2, Time t1, Time t2)
                {
                        modelRoot.Project.Pipeline.StartTwinView (src1, src2, t1, t2);
                }

                public void StopTwinView ()
                {
                        modelRoot.Project.Pipeline.StopTwinView ();
                }

                public void SeekTwinView (Time t1, Time t2)
                {
                        modelRoot.Project.Pipeline.SeekTwinView (t1, t2);
                }

                public void StartExporting (Exporter exporter)
                {
                        modelRoot.Project.Pipeline.StartExporting (exporter);
                }

                public void StopExporting ()
                {
                        modelRoot.Project.Pipeline.StopExporting ();
                }

                public void SaveLastBuffer (string fileName)
                {
                        modelRoot.Project.Pipeline.SaveLastBuffer (fileName);
                }
                
                // Private methods /////////////////////////////////////////////
                
                public void OnStateChanged (object o, PipelineStateIdleArgs args)
                {
                        if (args.PipelineState == PipelineState.Unknown)
                                return;
                        
                        try { 
                                Gdk.Threads.Enter ();
                                
                                if (StateChange != null) {
                                        bool playing = (args.PipelineState == PipelineState.Playing) ? true : false; 
                                        StateChange (modelRoot, new PipelineStateArgs (playing));
                                }
                        } finally {
                                Gdk.Threads.Leave ();
                        }
                                
                }
                
                public void OnTicker (object o, PipelineTickerIdleArgs args)
                {
                        
                        try {
                                Console.WriteLine ("Emitting ticker");
                                if (Ticker != null)
                                        Ticker (modelRoot, new PipelineTickerArgs (args.Time));
                                
                                // Try updating the display time span, if needed
                                TimeSpan currentSpan = modelRoot.Timeline.DisplayedTimeSpan;
                                Console.WriteLine ("Checking state");
                                //if (State == PipelineState.Playing &&
                                if (args.Time > currentSpan.End - (currentSpan.Duration / 10)) {
                                        Console.WriteLine ("Doing something");
                                        currentSpan.MoveTo (args.Time - currentSpan.Duration / 10);
                                        modelRoot.Timeline.DisplayedTimeSpan = currentSpan;
                                }
                                
                        } finally {
                                Console.WriteLine ("Leaving threads");
                                Gdk.Threads.Leave ();
                        }
                }

                public void OnError (object o, PipelineErrorIdleArgs args)
                {
                        try { 
                                Gdk.Threads.Enter ();

                                if (Error != null)
                                        Error (modelRoot, new PipelineErrorArgs (args.Error));
                               
                        } finally {
                                Gdk.Threads.Leave ();
                        }
                                
                }

                public bool OnNextSeekTimeout ()
                {
                        Seek (nextSeek);
                        return false;
                }
                
        }

}
       
