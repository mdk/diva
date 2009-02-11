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

using System;
using System.Runtime.InteropServices;
using GLib;

namespace Gdv {

        public class ClipStore : GLib.Object {
                
                // Imports /////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clipstore_new
                        ();
                
                [DllImport ("gdv")]
                internal static extern bool gdv_clipstore_is_span_empty
                        (IntPtr handle, ref TimeSpan span);

                [DllImport ("gdv")]
                internal static extern bool gdv_clipstore_suggest_position
                        (IntPtr handle, ref TimeSpan span, ref TimeSpan outspan, Time t);

                [DllImport ("gdv")]
                internal static extern bool gdv_clipstore_is_span_empty_excluding
                        (IntPtr handle, ref TimeSpan span, IntPtr clipPtr);

                [DllImport ("gdv")]
                internal static extern bool gdv_clipstore_suggest_position_excluding
                        (IntPtr handle, ref TimeSpan span, ref TimeSpan outspan, Time t, IntPtr clipPtr);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clipstore_get_clip_before_clip
                        (IntPtr handle, IntPtr clip);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clipstore_get_clip_after_clip
                        (IntPtr handle, IntPtr clip);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clipstore_get_clip_at_time
                        (IntPtr handle, Time time);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clipstore_get_clip_before_time
                        (IntPtr handle, Time time);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_clipstore_get_clip_after_time
                        (IntPtr handle, Time time);

                // Properties //////////////////////////////////////////////////
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ClipStore (IntPtr ptr) : base (ptr)
                {
                }
                
                /* CONSTRUCTOR */
                public ClipStore () : base (IntPtr.Zero)
                {
                        IntPtr ptr = gdv_clipstore_new ();
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        
                        Raw = ptr;
                }

                public bool IsSpanEmpty (TimeSpan span)
                {
                        return gdv_clipstore_is_span_empty (Handle, ref span);
                }

                public bool IsSpanEmptyExcluding (TimeSpan span, Clip clip)
                {
                        return gdv_clipstore_is_span_empty_excluding (Handle, ref span,
                                                                      clip.Handle);
                }

                public bool SuggestPosition (TimeSpan span, ref TimeSpan outspan, Time t)
                {
                        return gdv_clipstore_suggest_position (Handle, ref span, ref outspan, t);
                }

                public bool SuggestPositionExcluding (TimeSpan span, ref TimeSpan outspan, Time t, Clip clip)
                {
                        return gdv_clipstore_suggest_position_excluding (Handle, ref span,
                                                                         ref outspan, t, clip.Handle);
                }

                public Clip GetClipBeforeClip (Clip clip)
                {
                        IntPtr ptr = IntPtr.Zero;
                        ptr = gdv_clipstore_get_clip_before_clip (Handle, clip.Handle);
                        if (ptr == IntPtr.Zero)
                                return null;
                        else
                                return new Clip (ptr);
                }

                public Clip GetClipAfterClip (Clip clip)
                {
                        IntPtr ptr = IntPtr.Zero;
                        ptr = gdv_clipstore_get_clip_after_clip (Handle, clip.Handle);
                        if (ptr == IntPtr.Zero)
                                return null;
                        else
                                return new Clip (ptr);
                }

                public Clip GetClipAtTime (Time time)
                {
                        IntPtr ptr = IntPtr.Zero;
                        ptr = gdv_clipstore_get_clip_at_time (Handle, time);
                        if (ptr == IntPtr.Zero)
                                return null;
                        else
                                return new Clip (ptr);
                }

                public Clip GetClipBeforeTime (Time time)
                {
                        IntPtr ptr = IntPtr.Zero;
                        ptr = gdv_clipstore_get_clip_before_time (Handle, time);
                        if (ptr == IntPtr.Zero)
                                return null;
                        else
                                return new Clip (ptr);
                }

                public Clip GetClipAfterTime (Time time)
                {
                        IntPtr ptr = IntPtr.Zero;
                        ptr = gdv_clipstore_get_clip_after_time (Handle, time);
                        if (ptr == IntPtr.Zero)
                                return null;
                        else
                                return new Clip (ptr);
                }
                
        }
        
}