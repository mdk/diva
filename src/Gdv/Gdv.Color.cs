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
        
        public enum ColorConst { Black, White, Red, Green, Blue }
        
        public enum ColorTango { Butter1, Butter2, Butter3,
                                 Chameleon1, Chameleon2, Chameleon3, 
                                 Orange1, Orange2, Orange3,
                                 SkyBlue1, SkyBlue2, SkyBlue3, 
                                 Plum1, Plum2, Plum3,
                                 Chocolate1, Chocolate2, Chocolate3,
                                 ScarletRed1, ScarletRed2, ScarletRed3, 
                                 Aluminium1, Aluminium2, Aluminium3, 
                                 Aluminium4, Aluminium5, Aluminium6 }
        
        [StructLayout (LayoutKind.Sequential)]
        public struct Color {
                
                // Imports ////////////////////////////////////////////////////
 
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_color_new_const
                        (ColorConst colorconst);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_color_new_tango
                        (ColorTango colorconst);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_color_equals
                        (ref Color self, ref Color other);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_color_get_type
                        ();
                
                [DllImport ("gdv")]
                internal static extern void gdv_glue_gvalue_set_boxed
                        (ref Value val, ref Color color);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_glue_gvalue_get_boxed
                        (ref Value val);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_color_to_gdkcolor
                        (ref Color color);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_color_from_gdkcolor
                        (ref Gdk.Color color);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_color_to_html
                        (ref Color color);

                // Fields /////////////////////////////////////////////////////
                
                float red;
                float green;
                float blue;
                float alpha;
                
                // Properties /////////////////////////////////////////////////
                
                public static Color Zero {
                        get { return new Color (0.0f, 0.0f, 0.0f, 0.0f); }
                }
                
                public static Color Empty {
                        get { return new Color (-1.0f, -1.0f, -1.0f, -1.0f); }
                }
                
                /* Red */
                public float Red { 
                        get { return red; }
                        set { red = value; }
                }
                
                public byte Red8 {
                        get { return (byte) (red * 255.0); }
                        set { red = (float) value / 255.0f; }
                }
                
                /* Green */
                public float Green {
                        get { return green; }
                        set { green = value; }
                }
                
                public byte Green8 {
                        get { return (byte) (green * 255.0); }
                        set { green = (float) value / 255.0f; }
                }
                
                /* Blue */
                public float Blue {
                        get { return blue; }
                        set { blue = value; }
                }
                
                public byte Blue8 {
                        get { return (byte) (blue * 255.0); }
                        set { blue = (float) value / 255.0f; }
                }
                
                /* Alpha */
                public float Alpha {
                        get { return alpha; }
                        set { alpha = value; }
                }
                
                public byte Alpha8 {
                        get { return (byte) (alpha * 255.0); }
                        set { alpha = (float) value / 255.0f; }
                }
                
                // Methods ////////////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Color (float r, float g, float b) : this (r, g, b, 1.0f)
                {
                }
                
                /* CONSTRUCTOR */
                public Color (float r, float g, float b, float a)
                {
                        red = r;
                        green = g;
                        blue = b;
                        alpha = a;
                }
                
                /* CONSTRUCTOR */
                public Color (Byte r, Byte g, Byte b, Byte a)
                {
                        red = (float) r / 255.0f;
                        green = (float) g / 255.0f;
                        blue = (float) b / 255.0f;
                        alpha= (float) a / 255.0f;
                }
                
                /* CONSTRUCTOR */
                public Color (Byte r, Byte g, Byte b) : this (r, g, b, 1.0f)
                {
                }
                
                public static Color FromConst (ColorConst colorconst)
                {
                        IntPtr ptr = gdv_color_new_const (colorconst);
                        return New (ptr);
                }
                
                public static Color FromTango (ColorTango colortango)
                {
                        IntPtr ptr = gdv_color_new_tango (colortango);
                        return New (ptr);
                }

                public static Color FromGdk (Gdk.Color color)
                {
                        IntPtr ptr = gdv_color_from_gdkcolor (ref color);
                        return New (ptr);
                }
                
                public static Color New (IntPtr ptr)
                {
                        if (ptr == IntPtr.Zero)
                                return new Color ();
                        
                        Color self = (Color) 
                                Marshal.PtrToStructure (ptr, typeof (Color));
                        return self;
                }
                
                public static GType GType {
                        get {
                                IntPtr ptr = gdv_color_get_type ();
                                return new GType (ptr);
                        }
                }
                
                public override string ToString ()
                {
                        return String.Format ("RGBA {0},{1},{2},{3}", Red8, Green8, Blue8, Alpha8);
                }
                
                public string ToHtml ()
                {
                        IntPtr ptr = gdv_color_to_html (ref this);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        
                        return Marshaller.PtrToStringGFree (ptr);
                }
                
                // Operators //////////////////////////////////////////////////
                
                public static implicit operator Gdk.Color (Color color)
                {
                        return Gdk.Color.New (gdv_color_to_gdkcolor (ref color));
                }
                
                public static implicit operator Color (Gdk.Color color)
                {
                        return New (gdv_color_from_gdkcolor (ref color));
                }
                                
                public static explicit operator Value (Color color)
                {
                        Value val = Value.Empty;
                        val.Init (Color.GType);
                        gdv_glue_gvalue_set_boxed (ref val, ref color);
                        return val;
                }

                public static explicit operator Color (Value val)
                {
                        IntPtr ptr = gdv_glue_gvalue_get_boxed (ref val);
                        return New (ptr);
                }
                
                public override bool Equals (object o)
                {
                        if (! (o is Color))
                                return false;
                        
                        Color other = (Color) o;
                        return gdv_color_equals (ref this, ref other);
                }
                
                public static bool operator == (Color a, Color b)
                {
                        return gdv_color_equals (ref a, ref b);
                }
                
                public static bool operator != (Color a, Color b)
                {
                        return (! gdv_color_equals (ref a, ref b));
                }
                
                public override int GetHashCode ()
                {
                        return Red8 * Green8 * Blue8;
                }
                
        }
}
