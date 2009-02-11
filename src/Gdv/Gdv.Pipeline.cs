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

namespace Gdv {

        using System;
        using System.Runtime.InteropServices;
        using GLib;
        
        public class Pipeline : GLib.Object {
                
                // Imports ////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_can_handle_project_format
                        (IntPtr handle, ref IntPtr error);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_new
                        (IntPtr formatHandle);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_set_error_idle_func
                        (IntPtr handle, IntPtrIdleHandler handler);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_remove_error_idle_func
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_set_state_idle_func
                        (IntPtr handle, IntPtrIdleHandler handler);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_remove_state_idle_func
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_set_ticker_idle_func
                (IntPtr handle, IntPtrIdleHandler handler);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_pipeline_remove_ticker_idle_func
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_launch
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_play
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_pause
                        (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_ready
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_playpause
                        (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_shutdown
                        (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_set_target_window
                        (IntPtr handle, IntPtr windowHandle);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_add_track
                        (IntPtr pipelineHandle, IntPtr trackHandle);

                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_force_expose
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern void gdv_pipeline_seek
                        (IntPtr handle, Time time);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_start_single_view
                        (IntPtr handle, IntPtr sourceHandle, Time time);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_start_twin_view
                        (IntPtr handle, IntPtr sourceHandle1, IntPtr sourceHandle2,
                         Time time1, Time time2);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_start_exporting
                        (IntPtr handle, IntPtr exporterHandle);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_stop_exporting
                (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_seek_single_view
                        (IntPtr handle, Time time);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_seek_twin_view
                        (IntPtr handle, Time time1, Time time2);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_stop_single_view
                        (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_stop_twin_view
                        (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern bool gdv_pipeline_save_last_buffer
                        (IntPtr handle, IntPtr fileNamePtr);

                // Events //////////////////////////////////////////////////////

                public event PipelineStateIdleHandler StateIdle {
                        add {
                                stateHandlerWrapper = new IntPtrIdleHandler (OnPipelineStateIdle);
                                pipelineStateIdleHandler = value;
                                gdv_pipeline_set_state_idle_func (this.Handle, stateHandlerWrapper);
                        }
                        remove {
                                gdv_pipeline_remove_state_idle_func (this.Handle);
                                
                                pipelineStateIdleHandler = null;
                                stateHandlerWrapper = null;
                        }
                }

                public event PipelineTickerIdleHandler TickerIdle {
                        add {
                                tickerHandlerWrapper = new IntPtrIdleHandler (OnPipelineTickerIdle);
                                pipelineTickerIdleHandler = value;
                                gdv_pipeline_set_ticker_idle_func (this.Handle, tickerHandlerWrapper);
                        }
                        remove {
                                gdv_pipeline_remove_ticker_idle_func (this.Handle);
                                
                                pipelineTickerIdleHandler = null;
                                tickerHandlerWrapper = null;
                        }
                }

                public event PipelineErrorIdleHandler ErrorIdle {
                        add {
                                errorHandlerWrapper = new IntPtrIdleHandler (OnPipelineErrorIdle);
                                pipelineErrorIdleHandler = value;
                                gdv_pipeline_set_error_idle_func (this.Handle, errorHandlerWrapper);
                        }
                        remove {
                                gdv_pipeline_remove_error_idle_func (this.Handle);
                                
                                pipelineErrorIdleHandler = null;
                                errorHandlerWrapper = null;
                        }
                }

                // Fields //////////////////////////////////////////////////////

                PipelineStateIdleHandler pipelineStateIdleHandler = null;
                IntPtrIdleHandler stateHandlerWrapper = null;

                PipelineTickerIdleHandler pipelineTickerIdleHandler = null;
                IntPtrIdleHandler tickerHandlerWrapper = null;

                PipelineErrorIdleHandler pipelineErrorIdleHandler = null;
                IntPtrIdleHandler errorHandlerWrapper = null;

                // Property ////////////////////////////////////////////////////

                public bool Launched {
                        get { return (bool) GetProperty ("launched"); }
                }

                public bool Shutdowned { 
                        get { return (bool) GetProperty ("shutdowned"); }
                }

                public ProjectFormat ProjectFormat { 
                        get { return (ProjectFormat) GetProperty ("projectformat"); }
                }
                
                public string Error {
                        get { 
                                IntPtr ptr = (IntPtr) GetProperty ("error");
                                if (ptr == IntPtr.Zero)
                                        throw new Exception ("FIXME: No error!");
                                return Glue.GErrorGetString (ptr);
                        }
                }
                
                public Time Ticker {
                        get { return (Time.FromNSeconds ((UInt64) GetProperty ("ticker"))); }
                }
                
                public Track[] Tracks {
                        get { 
                                IntPtr ptr = (IntPtr) GetProperty ("tracks");
                                if (ptr == IntPtr.Zero) 
                                        return new Track[0];
                                        
                                List list = new List (ptr, typeof (Track), false, false);
                                Track[] tracks = new Track [list.Count];
                                list.CopyTo (tracks, 0);
                                
                                return tracks;
                        }
                }

                public PipelineState State {
                        get { return (PipelineState) (int) GetProperty ("state"); }
                }

                public double Volume {
                        get { return (double) GetProperty ("volume"); }
                        set { SetProperty ("volume", new Value (value)); }
                }

                // Public methods /////////////////////////////////////////////

                public Pipeline (ProjectFormat format) : base (IntPtr.Zero)
                {
                        IntPtr raw = gdv_pipeline_new (format.Handle);
                        
                        if (raw == IntPtr.Zero)
                                throw new Exception ();

                        this.Raw = raw;
                }

                public void SetTargetWindow (Gdk.Window window)
                {
                        gdv_pipeline_set_target_window (this.Handle, window.Handle);
                }

                public void Launch ()
                {
                        gdv_pipeline_launch (this.Handle);
                }
                
                public void Play ()
                {
                        gdv_pipeline_play (this.Handle);
                }

                public void Ready ()
                {
                        gdv_pipeline_ready (this.Handle);
                }
                
                public void Pause ()
                {
                        gdv_pipeline_pause (this.Handle);
                }
                
                public void PlayPause ()
                {
                        gdv_pipeline_playpause (this.Handle);
                }

                public void Shutdown ()
                {
                        gdv_pipeline_shutdown (this.Handle);
                }

                public void ForceExpose ()
                {
                        gdv_pipeline_force_expose (this.Handle);
                }
                
                public void AddTrack (Track track)
                {
                        if (gdv_pipeline_add_track (Handle, track.Handle) != true)
                                throw new Exception ("Failed to add track");
                }
                                
                public static bool CanHandleFormat (ProjectFormat format)
                {
                        IntPtr error = IntPtr.Zero;
                        bool res = gdv_pipeline_can_handle_project_format (format.Handle, ref error);

                        if (error != IntPtr.Zero)
                                throw new Exception (Glue.GErrorGetString (error));

                        return res;
                }
                
                public void Seek (Time t)
                {
                        gdv_pipeline_seek (Handle, t);
                }

                public void StartSingleView (Source source, Time t)
                {
                        gdv_pipeline_start_single_view (Handle, source.Handle, t);
                }

                public void StopSingleView ()
                {
                        gdv_pipeline_stop_single_view (Handle);
                }

                public void SeekSingleView (Time t)
                {
                        gdv_pipeline_seek_single_view (Handle, t);
                }

                public void StartTwinView (Source source1, Source source2, Time t1, Time t2)
                {
                        gdv_pipeline_start_twin_view (Handle, source1.Handle, source2.Handle,
                                                      t1, t2);
                }

                public void StopTwinView ()
                {
                        gdv_pipeline_stop_twin_view (Handle);
                }

                public void SeekTwinView (Time t1, Time t2)
                {
                        gdv_pipeline_seek_twin_view (Handle, t1, t2);
                }

                public void StartExporting (Exporter exporter)
                {
                        gdv_pipeline_start_exporting (Handle, exporter.Handle);
                }

                public void StopExporting ()
                {
                        gdv_pipeline_stop_exporting (Handle);
                }

                public void SaveLastBuffer (string fileName)
                {
                        IntPtr fileNamePtr = Marshaller.StringToFilenamePtr (fileName);

                        bool result = gdv_pipeline_save_last_buffer (Handle, fileNamePtr);
                        Marshaller.Free (fileNamePtr);

                        if (! result)
                                throw new Exception ();
                        // FIXME: Different exception!
                }

                // Private methods /////////////////////////////////////////////

                bool OnPipelineStateIdle (IntPtr ptr)
                {
                        Int32 val = Marshal.ReadInt32 (ptr);
                        Marshaller.Free (ptr);

                        if (pipelineStateIdleHandler != null)
                                pipelineStateIdleHandler (this,
                                                          new PipelineStateIdleArgs ((PipelineState) val));
                        
                        return false;
                }
                
                bool OnPipelineTickerIdle (IntPtr ptr)
                {
                        if (ptr == IntPtr.Zero)
                                return false;

                        UInt64 t = (UInt64) System.Runtime.InteropServices.Marshal.ReadInt64 (ptr);
                        GLib.Marshaller.Free (ptr);

                        Time time = Gdv.Time.FromNSeconds (t);
                                
                        if (pipelineTickerIdleHandler != null)
                                pipelineTickerIdleHandler (this,
                                                           new PipelineTickerIdleArgs (time));
                                
                        return false;
                }

                bool OnPipelineErrorIdle (IntPtr ptr)
                {
                        if (ptr == IntPtr.Zero)
                                return false;

                        // That frees the ptr automatically
                        string error = Glue.GErrorGetString (ptr);
                                
                        if (pipelineErrorIdleHandler != null)
                                pipelineErrorIdleHandler (this,
                                                          new PipelineErrorIdleArgs (error));
                                
                        return false;
                }

        }
        
}
                
