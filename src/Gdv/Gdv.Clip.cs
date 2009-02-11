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

        public class Clip : GLib.Object {
                
                // Imports /////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clip_new
                        (IntPtr mediaItemHandle);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clip_new_explicit
                        (IntPtr sourceHandle, IntPtr mediaItemHandle);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_clip_place_at_timespan
                        (IntPtr handle, ref TimeSpan timeSpan);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_clip_place_at_time
                        (IntPtr handle, UInt64 time);

                [DllImport ("gdv")]
                internal static extern bool gdv_clip_move_to
                        (IntPtr handle, UInt64 time);

                [DllImport ("gdv")]
                internal static extern bool gdv_clip_left_adjust_to
                        (IntPtr handle, UInt64 time);

                [DllImport ("gdv")]
                internal static extern bool gdv_clip_right_adjust_to
                        (IntPtr handle, Time time);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_clip_is_placed
                        (IntPtr handle);

                [DllImport ("gdv")]
                internal static extern UInt64 gdv_clip_time_to_source_time
                        (IntPtr handle, UInt64 t);

                // Events //////////////////////////////////////////////////////

                public event EventHandler Changed {
                        add {
                                Signal sig = GLib.Signal.Lookup (this, "changed");
                                sig.AddDelegate (value);
                        }
                        remove {
                                Signal sig = GLib.Signal.Lookup (this, "changed");
                                sig.RemoveDelegate (value);
                        }
                }
                
                // Properties //////////////////////////////////////////////////
                
                public MediaItem ParentItem {
                        get { return (MediaItem) GetProperty ("parentitem"); }
                }
                
                public Source Source {
                        get { return (Source) GetProperty ("source"); }
                }
                
                public Time TimelineIn {
                        get { return (Time) GetProperty ("timelinein"); }
                }
                
                public Time TimelineOut {
                        get { return (Time) GetProperty ("timelineout"); }
                }
                
                public Time MinTimelineIn {
                        get { return (Time) GetProperty ("mintimelinein"); }
                }
                
                public Time MaxTimelineOut {
                        get { return (Time) GetProperty ("maxtimelineout"); }
                }
                
                public TimeSpan TimelineSpan {
                        get { return (TimeSpan) GetProperty ("timelinespan"); }
                        set { SetProperty ("timelinespan", new Value(value)); }
                }
                
                public TimeSpan SourceSpan {
                        get { return (TimeSpan) GetProperty ("sourcespan"); }
                        set { SetProperty ("sourcespan", new Value(value)); }
                }
                
                public bool IsPlaced {
                        get { return gdv_clip_is_placed (Handle); }
                }

                public Track Track {
                        get { return (Track) GetProperty ("track"); }
                }

                public bool HasVideo {
                        get { return (bool) GetProperty ("hasvideo"); }
                }
                
                public bool HasAudio {
                        get { return (bool) GetProperty ("hasaudio"); }
                }
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Clip (IntPtr ptr) : base (ptr)
                {
                }
                
                /* CONSTRUCTOR */
                public Clip (MediaItem item) : base (IntPtr.Zero)
                {
                        IntPtr ptr = gdv_clip_new (item.Handle);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        
                        Raw = ptr;
                }

                /* CONSTRUCTOR */
                public Clip (Source source, MediaItem item) : base (IntPtr.Zero)
                {
                        IntPtr ptr = gdv_clip_new_explicit (source.Handle, item.Handle);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        
                        Raw = ptr;
                }
                
                public bool PlaceAt (TimeSpan timeSpan)
                {
                        return gdv_clip_place_at_timespan (Handle, ref timeSpan);
                }
                
                public bool PlaceAt (Time time)
                {
                        return gdv_clip_place_at_time (Handle, time);
                }

                public void MoveTo (Time t)
                {
                        gdv_clip_move_to (Handle, t);
                }

                public void LeftAdjustTo (Time t)
                {
                        gdv_clip_left_adjust_to (Handle, t);
                }

                public void RightAdjustTo (Time t)
                {
                        gdv_clip_right_adjust_to (Handle, t);
                }

                public Time ClipTimeToSourceTime (Time t)
                {
                        return gdv_clip_time_to_source_time (Handle, t.NSeconds);
                }
                
        }
        
}
                
