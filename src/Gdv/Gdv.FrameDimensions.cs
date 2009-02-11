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
        public struct FrameDimensions {
                
                // Imports ////////////////////////////////////////////////////
 
                [DllImport ("gdv")]
                internal static extern int gdv_framedimensions_compare
                        (ref FrameDimensions a, ref FrameDimensions b);
                
                [DllImport ("gdv")]
                internal static extern void gdv_framedimensions_fit_w
                        (ref FrameDimensions fd, Int32 w, out FrameDimensions fdout);
                
                [DllImport ("gdv")]
                internal static extern void gdv_framedimensions_fit_h
                        (ref FrameDimensions fd, Int32 h, out FrameDimensions fdout);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_framedimensions_get_type
                        ();
                
                [DllImport ("gdv")]
                internal static extern void gdv_glue_gvalue_set_boxed
                        (ref Value val, ref FrameDimensions fd);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_glue_gvalue_get_boxed
                        (ref Value val);

                // Fields /////////////////////////////////////////////////////
                
                Int32 width;
                Int32 height;
                
                // Properties /////////////////////////////////////////////////

                public Int32 Width {
                        get { return width; }
                        set { width = value; }
                }
                
                public Int32 Height {
                        get { return height; }
                        set { height = value; }
                }
                
                // Methods ////////////////////////////////////////////////////
                
                public FrameDimensions (Int32 w, Int32 h)
                {
                        width = w;
                        height = h;
                }
                
                public static FrameDimensions New (IntPtr ptr)
                {
                        if (ptr == IntPtr.Zero)
                                return new FrameDimensions ();
                        
                        FrameDimensions self = (FrameDimensions) 
                                Marshal.PtrToStructure (ptr, typeof (FrameDimensions));
                        return self;
                }
                
                public FrameDimensions FitWidth (Int32 w)
                {
                        FrameDimensions fdout;
                        gdv_framedimensions_fit_w (ref this, w, out fdout);
                        return fdout;
                }
                
                public FrameDimensions FitHeight (Int32 h)
                {
                        FrameDimensions fdout;
                        gdv_framedimensions_fit_h (ref this, h, out fdout);
                        return fdout;
                }
                
                public override string ToString ()
                {
                        return String.Format ("{0}x{1}", width, height);
                }
                
                public static GType GType {
                        get {
                                IntPtr ptr = gdv_framedimensions_get_type ();
                                return new GType (ptr);
                        }
                }
                
                // Operators //////////////////////////////////////////////////
                
                public static explicit operator Value (FrameDimensions fd)
                {
                        Value val = Value.Empty;
                        val.Init (FrameDimensions.GType);
                        gdv_glue_gvalue_set_boxed (ref val, ref fd);
                        return val;
                }

                public static explicit operator FrameDimensions (Value val)
                {
                        IntPtr ptr = gdv_glue_gvalue_get_boxed (ref val);
                        return New (ptr);
                }
                
                public override bool Equals (object o)
                {
                        if (! (o is FrameDimensions))
                                return false;
                        
                        FrameDimensions other = (FrameDimensions) o;
                        if (gdv_framedimensions_compare (ref this, ref other) == 0)
                                return true;
                        else
                                return false;
                }
                
                public override int GetHashCode() 
                {
                        return width * height;
                }
                
                public static bool operator == (FrameDimensions a, FrameDimensions b)
                {
                        if (gdv_framedimensions_compare (ref a, ref b) == 0)
                                return true;
                        else 
                                return false;
                }
                
                public static bool operator != (FrameDimensions a, FrameDimensions b)
                {
                        if (gdv_framedimensions_compare (ref a, ref b) != 0)
                                return true;
                        else 
                                return false;
                }
                
        }
}
