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
        public struct Time {

                // Imports ////////////////////////////////////////////////////
                 
                [DllImport ("gdv")]
                internal static extern double gdv_time_to_seconds 
                        (UInt64 time);
                
                [DllImport ("gdv")]
                internal static extern UInt64 gdv_time_from_seconds
                        (double seconds);
                
                [DllImport ("gdv")]
                internal static extern UInt64 gdv_time_relative_to_absolute
                        (UInt64 offset, UInt64 time);
                
                [DllImport ("gdv")]
                internal static extern UInt64 gdv_time_absolute_to_relative
                        (UInt64 offset, UInt64 time);
                
                [DllImport ("gdv")]
                internal static extern UInt64 gdv_time_round
                        (UInt64 time, UInt64 precision);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_time_inspan
                        (UInt64 time, UInt64 i, UInt64 o);
                
                [DllImport ("gdv")]
                internal static extern Int64 gdv_time_get_int64
                        (UInt64 time);
                
                // Fields /////////////////////////////////////////////////////
                
                UInt64 val;
                
                // Properties /////////////////////////////////////////////////
                
                public double Seconds {
                        get { return gdv_time_to_seconds (val); }
                        set { val = gdv_time_from_seconds (value); }
                }
                
                public UInt64 NSeconds {
                        get { return val; }
                        set { val = value; }
                }

                public static Time Zero {
                        get { Time t; t.val = 0; return t; }
                }
                
                public static Time OneSecond {
                        get { Time t = Zero ; t.Seconds = 1; return t; }
                }
                
                public static Time Empty {
                        get { Time t; t.val = 0xFFFFFFFFFFFFFFFF; return t; }
                }

                public static Time Infinity {
                        get { return Empty; }
                }
                                
                // Methods ////////////////////////////////////////////////////
                
                public Time RelativeTo (Time time)
                {
                        Time t;
                        t.val = gdv_time_relative_to_absolute (time.val, val);
                        return t;
                }
                
                public Time AbsoluteTo (Time time)
                {
                        Time t;
                        t.val = gdv_time_absolute_to_relative (time.val, val);
                        return t;
                }
                
                public Time Round (Time precision)
                {
                        Time t;
                        t.val = gdv_time_round (this.val, precision);
                        return t;
                }
                
                public bool InSpan (Time i, Time o)
                {
                        return gdv_time_inspan (val, i.val, o.val);
                }
                
                public static Time FromSeconds (double sec)
                {
                        Time t;
                        t.val = gdv_time_from_seconds (sec);
                        return t;                        
                }
                
                public static Time FromNSeconds (UInt64 nsecs)
                {
                        Time t;
                        t.val = nsecs; 
                        return t;
                }
                
                public override string ToString ()
                {
                        // FIXME: Hacky
                        return String.Format ("{0:f2}" , (float) val / (float) OneSecond.val );
                }
                
                // Operators //////////////////////////////////////////////////
                
                public static explicit operator Value (Time t)
                {
                        Value val = new Value (t.val);
                        return val;
                }
                
                public static explicit operator Time (Value val)
                {
                        Time t;
                        t.val = (UInt64) val;
                        return t;
                }

                public static implicit operator UInt64 (Time t)
                {
                        return t.val;
                }

                public static implicit operator Time (ulong val)
                {
                        return Time.FromNSeconds (val);
                }
                
                public static implicit operator Int64 (Time t)
                {
                        return gdv_time_get_int64 (t.val);
                }
                
                public static Time operator + (Time a, Time b)
                {
                        Time t;
                        t.val = a.val + b.val;
                        return t;
                }                
                               
                public static Time operator - (Time a, Time b)
                {
                        Time t;
                        t.val = a.val - b.val;
                        return t;
                }
                
                public static Time operator * (Time a, UInt64 b)
                {
                        Time t;
                        t.val = a.val * b;
                        return t;
                }                
                
                public static Time operator / (Time a, UInt64 b)
                {
                        Time t;
                        t.val = a.val / b;
                        return t;
                }                
                
                public static bool operator < (Time a, Time b)
                {
                        if (a.val < b.val) 
                                return true;
                        else
                                return false;
                }
                
                public static bool operator > (Time a, Time b)
                {
                        if (a.val > b.val) 
                                return true;
                        else
                                return false;
                }

                public static bool operator == (Time a, Time b)
                {
                        if (a.val == b.val)
                                return true;
                        else 
                                return false;
                } 
                
                public static bool operator != (Time a, Time b)
                {
                        if (a.val != b.val)
                                return true;
                        else 
                                return false;
                } 
                
                public static bool operator >= (Time a, Time b)
                {
                        if (a.val >= b.val)
                                return true;
                        else
                                return false;
                } 

                public static bool operator <= (Time a, Time b)
                {
                        if (a.val <= b.val) 
                                return true;
                        else
                                return false;
                } 
                
                public override bool Equals (object o)
                {
                        if (! (o is Time))
                                return false;
                        
                        Time other = (Time) o;
                        if (val == other.val)
                                return true;
                        else 
                                return false;
                } 
                
                public override int GetHashCode ()
                {
                        return val.GetHashCode ();
                }
                
        }
        
}
                                 
                       
                
                
                
                
