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
        
        public class VideoFormat : GLib.Object {
                
                // Imports ////////////////////////////////////////////////////
               
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_videoformat_serialize_to_string
                        (IntPtr format);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_videoformat_new_from_string
                        (IntPtr str);

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_videoformat_new_default
                        ();

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_videoformat_clone
                        (IntPtr clonedPtr);
 
                // Propeties //////////////////////////////////////////////////
                
                public Fraction Fps {
                        get { return (Fraction) GetProperty ("fps"); }
                        set { SetProperty ("fps", new Value(value)); }
                }

                public Fraction PixelAspect {
                        get { return (Fraction) GetProperty ("pixelaspect"); }
                        set { SetProperty ("pixelaspect", new Value(value)); }
                }
                
                public FrameDimensions FrameDimensions {
                        get { return (FrameDimensions) GetProperty ("framedimensions"); }
                        set { SetProperty ("framedimensions", new Value(value)); }
                }

                public Fraction FrameAspect {
                        get { return (Fraction) GetProperty ("frameaspect"); }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                public VideoFormat (IntPtr ptr) : base (ptr)
                {
                }

                public VideoFormat () : base (IntPtr.Zero)
                {
                        IntPtr ptr = gdv_videoformat_new_default ();
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        else
                                Raw = ptr;
                }
                
                public override string ToString ()
                {
                        return String.Format ("{0} {1}fps px:{2}" , FrameDimensions, Fps, PixelAspect);
                }

                public string SerializeToString ()
                {
                        IntPtr stringPtr = gdv_videoformat_serialize_to_string (this.Handle);
                        if (stringPtr == IntPtr.Zero)
                                throw new Exception ();

                        string str = GLib.Marshaller.Utf8PtrToString (stringPtr);
                        Marshaller.Free (stringPtr);
                        return str;
                }
                
                public static VideoFormat NewFromString (string str)
                {
                        IntPtr strPtr = Marshaller.StringToPtrGStrdup (str);
                        IntPtr ptr = gdv_videoformat_new_from_string (strPtr);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ("FIXME: Null pointer");
                        
                        // FIXME: Free the string or not?!
                        return new VideoFormat (ptr);
                }

                public VideoFormat Clone ()
                {
                        IntPtr newPtr = gdv_videoformat_clone (Handle);
                        if (newPtr == IntPtr.Zero)
                                throw new Exception ("FIXME: Null pointer");

                        return new VideoFormat (newPtr);
                }
                
                // Protected methods //////////////////////////////////////////
                
        }
        
}
                
