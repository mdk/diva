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

        public class DecodeBin : Source, IFileBased {
                
                // Imports /////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_decodebin_new
                        (IntPtr filename, IntPtr vf, IntPtr af, 
                         UInt64 l, ref IntPtr err);

                // Properties //////////////////////////////////////////////////
                
                public string BaseFile {
                        get { return (string) GetProperty ("basefile"); }
                }

                // Public methods //////////////////////////////////////////////
                
                public DecodeBin (IntPtr ptr) : base (ptr)
                {
                }
                
                public DecodeBin (string fileName, VideoFormat vf, AudioFormat af, 
                                  Time len) : base (IntPtr.Zero)
                {
                        IntPtr fileNamePtr = Marshaller.StringToPtrGStrdup (fileName);
                        IntPtr error = IntPtr.Zero;
                        IntPtr raw = gdv_decodebin_new (fileNamePtr, 
                                                        (vf != null) ? vf.Handle : IntPtr.Zero, 
                                                        (af != null) ? af.Handle : IntPtr.Zero,
                                                        len, ref error);
                        Marshaller.Free (fileNamePtr);
                        
                        if (error != IntPtr.Zero)
                                throw new UserException (error);
                        
                        this.Raw = raw;
                }

        }
        
}
                
