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
        
        public class AudioFormat : GLib.Object {
                
                // Imports ////////////////////////////////////////////////////
               
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_audioformat_serialize_to_string
                        (IntPtr format);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_audioformat_new_from_string
                        (IntPtr str);
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_audioformat_new_default
                        ();

                [DllImport ("gdv")]
                internal static extern IntPtr gdv_audioformat_clone
                        (IntPtr clonedPtr);
 
                // Propeties //////////////////////////////////////////////////
                
                public Int32 SampleRate {
                        get { return (Int32) GetProperty ("samplerate"); }
                        set { SetProperty ("samplerate", new Value(value)); }
                }
                
                public Int32 Depth {
                        get { return (Int32) GetProperty ("depth"); }
                        set { SetProperty ("depth", new Value(value)); }
                }
                
                public Int32 Channels {
                        get { return (Int32) GetProperty ("channels"); }
                        set { SetProperty ("channels", new Value(value)); }
                }
                
                // Public methods /////////////////////////////////////////////
                
                public AudioFormat (IntPtr ptr) : base (ptr)
                {
                }

                public AudioFormat () : base (IntPtr.Zero)
                {
                        IntPtr ptr = gdv_audioformat_new_default ();
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        else
                                Raw = ptr;
                }
                                
                public override string ToString ()
                {
                        string mode = String.Empty;
                        switch (Channels) {
                                case 1:
                                mode = "mono";
                                break;
                                
                                case 2: 
                                mode = "stereo";
                                break;
                                
                                case 3: 
                                mode = "surround";
                                break;
                                
                                case 4:
                                mode = "quadro";
                                break;
                                
                                case 5:
                                mode = "3d sound";
                                break;
                        }
                        
                        return String.Format ("{0}hz {1}bpp {2}" , SampleRate, Depth, mode);
                }
               
                public string SerializeToString ()
                {
                        IntPtr stringPtr = gdv_audioformat_serialize_to_string (this.Handle);
                        if (stringPtr == IntPtr.Zero)
                                throw new Exception ();

                        string str = GLib.Marshaller.Utf8PtrToString (stringPtr);
                        Marshaller.Free (stringPtr);
                        return str;
                }
                
                public static AudioFormat NewFromString (string str)
                {
                        IntPtr strPtr = Marshaller.StringToPtrGStrdup (str);
                        IntPtr ptr = gdv_audioformat_new_from_string (strPtr);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ("FIXME: Null pointer");
                        
                        // FIXME: Free the string or not?!
                        return new AudioFormat (ptr);
                }

                public AudioFormat Clone ()
                {
                        IntPtr newPtr = gdv_audioformat_clone (Handle);
                        if (newPtr == IntPtr.Zero)
                                throw new Exception ("FIXME: Null pointer");
                        
                        return new AudioFormat (newPtr);
                }

        }
        
}
                
