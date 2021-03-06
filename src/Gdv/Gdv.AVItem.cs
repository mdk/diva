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
        
        public class AVItem : MediaItem, ITimeable, IThumbnailable {
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_avitem_new
                        (IntPtr src);
                
                // Propeties //////////////////////////////////////////////////
                
                public Time Length {
                        get { return (Time) GetProperty ("length"); }
                }
                
                public Gdk.Pixbuf Thumbnail {
                        get { return (Gdk.Pixbuf) GetProperty ("thumbnail"); }
                }

                public Gdk.Pixbuf SmallThumbnail {
                        get { return (Gdk.Pixbuf) GetProperty ("smallthumbnail"); }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                public AVItem (Source src) : base (IntPtr.Zero)
                {
                        IntPtr raw = gdv_avitem_new (src.Handle);
                        // FIXME: Different exception
                        if (raw == IntPtr.Zero) 
                                throw new Exception ();
                        Raw = raw;
                }
                
                public AVItem (IntPtr ptr) : base (ptr)
                {
                }

        }
        
}
                
