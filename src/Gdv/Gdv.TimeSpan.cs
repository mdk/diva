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
        
        [StructLayout (LayoutKind.Sequential)]
        public struct TimeSpan {

                // Imports ////////////////////////////////////////////////////
                 
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_timespan_new_empty 
                        ();
                
                [DllImport ("gdv")]
                internal static extern UInt64 gdv_timespan_duration 
                        (ref TimeSpan ts);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_is_empty
                        (ref TimeSpan ts);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_contains 
                        (ref TimeSpan ts, Time t);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_contains_math
                        (ref TimeSpan ts, Time t);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_intersects_with
                        (ref TimeSpan ts, ref TimeSpan to);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_intersect
                        (ref TimeSpan ts, ref TimeSpan to, ref TimeSpan o);

                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_is_adjacent
                        (ref TimeSpan ts, ref TimeSpan to);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_timespan_is_equal
                        (ref TimeSpan ts, ref TimeSpan to);
                
                [DllImport ("gdv")]
                internal static extern void gdv_timespan_move_to
                        (ref TimeSpan ts, Time t);
                
                [DllImport ("gdv")]
                internal static extern void gdv_timespan_cut_left
                        (ref TimeSpan ts, Time t);
                
                [DllImport ("gdv")]
                internal static extern void gdv_timespan_cut_right
                        (ref TimeSpan ts, Time t);

                [DllImport ("gdv")]
                internal static extern void gdv_timespan_halve
                        (ref TimeSpan ts, ref TimeSpan h1, ref TimeSpan h2);
                
                [DllImport ("gdv")]
                internal static extern void gdv_timespan_add_offset
                        (ref TimeSpan ts, Time t);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_timespan_get_type
                        ();
                
                [DllImport ("gdv")]
                internal static extern void gdv_glue_gvalue_set_boxed
                        (ref Value val, ref TimeSpan ts);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_glue_gvalue_get_boxed
                        (ref Value val);

                // Fields /////////////////////////////////////////////////////
                
                Time start;
                Time end;
                
                // Properties /////////////////////////////////////////////////
                
                public Time Start {
                        get { return start; }
                        set { start = value; }
                }
                
                public Time End {
                        get { return end; }
                        set { end = value; }
                }
                
                public static TimeSpan Empty {
                        get { 
                                IntPtr ptr = gdv_timespan_new_empty ();
                                return New (ptr);
                        }
                }
                
                public Time Duration {
                        get { 
                                UInt64 t = gdv_timespan_duration (ref this);
                                return Time.FromNSeconds (t);
                        }
                }
                
                public static GType GType {
                        get {
                                IntPtr ptr = gdv_timespan_get_type ();
                                return new GType (ptr);
                        }
                }
                                
                // Methods ////////////////////////////////////////////////////
                
                public TimeSpan (Time start, Time end)
                {
                        this.start = start;
                        this.end = end;
                }
                                
                public static TimeSpan New (IntPtr ptr)
                {
                        if (ptr == IntPtr.Zero)
                                return new TimeSpan ();
                        
                        TimeSpan self = (TimeSpan) 
                                Marshal.PtrToStructure (ptr, typeof (TimeSpan));
                        return self;
                }
                
                public bool IsEmpty ()
                {
                        return gdv_timespan_is_empty (ref this);
                }
                
                public bool Contains (Time t)
                {
                        return gdv_timespan_contains (ref this, t);
                }

                public bool ContainsMath (Time t)
                {
                        return gdv_timespan_contains_math (ref this, t);
                }
                
                public bool IntersectsWith (TimeSpan other)
                {
                        return gdv_timespan_intersects_with (ref this, ref other);
                }
                
                public TimeSpan Intersect (TimeSpan other)
                {
                        TimeSpan r = new TimeSpan ();
                        gdv_timespan_intersect (ref this, ref other, ref r);
                        return r;
                }
                
                public bool IsAdjacent (TimeSpan other)
                {
                        return gdv_timespan_is_adjacent (ref this, ref other);
                }
                
                public void MoveTo (Time t)
                {
                        gdv_timespan_move_to (ref this, t);
                }
                
                public void CutLeft (Time t)
                {
                        gdv_timespan_cut_left (ref this, t);
                }
                
                public void CutRight (Time t)
                {
                        gdv_timespan_cut_right (ref this, t);
                }
                
                public void Halve (out TimeSpan h1, out TimeSpan h2)
                {
                        TimeSpan hh1 = new TimeSpan ();
                        TimeSpan hh2 = new TimeSpan ();
                        gdv_timespan_halve (ref this, ref hh1, ref hh2);
                        h1 = hh1;
                        h2 = hh2;
                        return;
                }
                
                public void AddOffset (Time t)
                {
                        gdv_timespan_add_offset (ref this, t);
                }
                
                public override string ToString ()
                {
                        return String.Format ("{0}-{1}", start, end);
                }

                // Operators //////////////////////////////////////////////////
                
                public static explicit operator Value (TimeSpan ts)
                {
                        Value val = Value.Empty;
                        val.Init (TimeSpan.GType);
                        gdv_glue_gvalue_set_boxed (ref val, ref ts);
                        return val;
                }

                public static explicit operator TimeSpan (Value val)
                {
                        IntPtr ptr = gdv_glue_gvalue_get_boxed (ref val);
                        return New (ptr);
                }
                              
                public static bool operator == (TimeSpan a, TimeSpan b)
                {
                        return gdv_timespan_is_equal (ref a, ref b);
                } 
                
                public static bool operator != (TimeSpan a, TimeSpan b)
                {
                        return ! gdv_timespan_is_equal (ref a, ref b);
                } 
                
                public override bool Equals (object o)
                {
                        if (! (o is TimeSpan))
                                return false;
                        
                        TimeSpan other = (TimeSpan) o;
                        return gdv_timespan_is_equal (ref this, ref other);
                }
                
                public override int GetHashCode ()
                {
                        UInt64 u = start + end;
                        return u.GetHashCode ();
                }

        }
        
}
                                 
                       
                
                
                
                
