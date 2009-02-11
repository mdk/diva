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
        public struct Fraction {
                
                // Imports ////////////////////////////////////////////////////
                 
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_fraction_new 
                        (int num, int den);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_fraction_new_empty 
                        ();

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_fraction_invert
                        (ref Fraction f);

                [DllImport ("gdv")]
                internal static extern double gdv_fraction_to_double
                        (ref Fraction f);

                [DllImport ("gdv")]
                internal static extern int gdv_fraction_compare
                        (ref Fraction a, ref Fraction b);

                [DllImport ("gdv")]
                internal static extern bool gdv_fraction_simplify
                        (ref Fraction f);

                [DllImport ("gdv")]
                internal static extern ulong gdv_fraction_fps_frame_duration
                        (ref Fraction f);

                [DllImport ("gdv")]
                internal static extern ulong gdv_fraction_fps_time_at_frame
                        (ref Fraction f, int no);

                [DllImport ("gdv")]
                internal static extern int gdv_fraction_fps_frame_at_time_rnd
                        (ref Fraction f, ulong t);

                [DllImport ("gdv")]
                internal static extern int gdv_fraction_fps_frame_at_time
                        (ref Fraction f, ulong t);

                [DllImport ("gdv")]
                internal static extern ulong gdv_fraction_fps_normalize_time_rnd
                        (ref Fraction f, ulong t);

                [DllImport ("gdv")]
                internal static extern ulong gdv_fraction_fps_normalize_time
                        (ref Fraction f, ulong t);

                [DllImport ("gdv")]
                internal static extern int gdv_fraction_fps_digitize
                        (ref Fraction f);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_fraction_aspect_pixelize
                        (ref Fraction f1, ref Fraction f2, ref FrameDimensions dimensions);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_fraction_get_type
                        ();
                
                [DllImport ("gdv")]
                internal static extern void gdv_glue_gvalue_set_boxed
                        (ref Value val, ref Fraction f);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_glue_gvalue_get_boxed
                        (ref Value val);

                // Fields /////////////////////////////////////////////////////
                
                int numerator;
                int denominator;
                bool simple;

                // Properties /////////////////////////////////////////////////
                
                public int Numerator {
                        get { return numerator; }
                        set { numerator = value; simple = false; }
                }
                
                public int Denominator {
                        get { return denominator; }
                        set { denominator = value; simple = false; }
                }

                public bool Simple {
                        get { return simple; }
                }

                public static Fraction Empty {
                        get { return FromPtr (gdv_fraction_new_empty ()); }
                }

                public static GType GType {
                        get {
                                IntPtr ptr = gdv_fraction_get_type ();
                                return new GType (ptr);
                        }
                }
                
                // Methods ////////////////////////////////////////////////////
                
                public Fraction (int numerator, int denominator)
                {
                        IntPtr ptr = gdv_fraction_new (numerator, denominator);
                        // FIXME: Exception
                        
                        Fraction self = (Fraction)
                                Marshal.PtrToStructure (ptr, typeof (Fraction));

                        this.numerator = self.Numerator;
                        this.denominator = self.Denominator;
                        this.simple = self.Simple;
                }
                
                public static Fraction FromPtr (IntPtr ptr)
                {
                        // FIXME: Exception rather
                        if (ptr == IntPtr.Zero)
                                return new Fraction ();
                        
                        Fraction self = (Fraction)
                                Marshal.PtrToStructure (ptr, typeof (Fraction));
                        return self;
                }

                public bool Simplify ()
                {
                        return gdv_fraction_simplify (ref this);
                }

                public Fraction Invert ()
                {
                        IntPtr ptr = gdv_fraction_invert (ref this);
                        return FromPtr (ptr);
                }

                public Time FpsFrameDuration ()
                {
                        return gdv_fraction_fps_frame_duration (ref this);
                }

                public Time FpsTimeAtFrame (int frame)
                {
                        return gdv_fraction_fps_time_at_frame (ref this, frame);
                }

                public int FpsFrameAtTimeRnd (Time t)
                {
                        return gdv_fraction_fps_frame_at_time_rnd (ref this, t);
                }

                public int FpsFrameAtTime (Time t)
                {
                        return gdv_fraction_fps_frame_at_time (ref this, t);

                }

                public Time FpsNormalizeTimeRnd (Time t)
                {
                        return gdv_fraction_fps_normalize_time_rnd (ref this, t);
                }

                public Time FpsNormalizeTime (Time t)
                {
                        return gdv_fraction_fps_normalize_time (ref this, t);
                }

                public int FpsDigitize ()
                {
                        return gdv_fraction_fps_digitize (ref this);
                }
                
                public override string ToString ()
                {
                        return (this == Empty) ? "(empty)" : String.Format ("{0}/{1}", numerator, denominator);
                }
                
                // Static methods /////////////////////////////////////////////

                /* NUNIT */
                public static FrameDimensions AspectPixelize (Fraction f1, Fraction f2, FrameDimensions dimensions)
                {
                        IntPtr ptr = gdv_fraction_aspect_pixelize (ref f1, ref f2,
                                                                   ref dimensions);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        else
                                return FrameDimensions.New (ptr);
                }
                
                // Operators //////////////////////////////////////////////////
                
                public static explicit operator Value (Fraction f)
                {
                        Value val = Value.Empty;
                        val.Init (Fraction.GType);
                        gdv_glue_gvalue_set_boxed (ref val, ref f);
                        return val;
                }

                public static explicit operator Fraction (Value val)
                {
                        IntPtr ptr = gdv_glue_gvalue_get_boxed (ref val);
                        return FromPtr (ptr);
                }
                
                public static explicit operator double (Fraction f)
                {
                        return gdv_fraction_to_double (ref f);
                }
                
                public override bool Equals (object o)
                {
                        if (! (o is Fraction))
                                return false;
                        
                        Fraction other = (Fraction) o;
                        return (gdv_fraction_compare (ref this, ref other) == 0) ? true : false;
                }
                
                public override int GetHashCode() 
                {
                        return numerator * denominator;
                }
                
                public static bool operator == (Fraction a, Fraction b)
                {
                        return (gdv_fraction_compare (ref a, ref b) == 0) ? true : false;
                }
                
                public static bool operator != (Fraction a, Fraction b)
                {
                        return (gdv_fraction_compare (ref a, ref b) != 0) ? true : false;
                }

                // FIXME: More operators!
                
        }
}

 
