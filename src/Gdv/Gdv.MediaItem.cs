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
        
        public abstract class MediaItem : GLib.Object {
                
                // Imports ////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_mediaitem_get_fresh_source
                        (IntPtr handle);
                
                // Propeties //////////////////////////////////////////////////
                
                public bool HasVideo {
                        get { return (bool) GetProperty ("hasvideo"); }
                }
                
                public bool HasAudio {
                        get { return (bool) GetProperty ("hasaudio"); }
                }
                
                public VideoFormat VideoFormat {
                        get { return (VideoFormat) GetProperty ("videoformat"); }
                }
                
                public AudioFormat AudioFormat {
                        get { return (AudioFormat) GetProperty ("audioformat"); }
                }
                
                public string Name {
                        get { return (string) GetProperty ("name"); }
                        set { SetProperty ("name", new Value (value)); }
                }

                public Source BaseSource {
                        get { return (Source) GetProperty ("basesource"); }
                }
                
                // Public methods /////////////////////////////////////////////
                
                public MediaItem (IntPtr ptr) : base (ptr)
                {
                }
                
                public Source GetFreshSource ()
                {
                        IntPtr ptr = IntPtr.Zero;
                        ptr = gdv_mediaitem_get_fresh_source (this.Handle);
                        if (ptr == IntPtr.Zero)
                                throw new InternalException ("Failed to get source");
                        else 
                                return ItemGenerator.AutowrapSource (ptr);
                }
                
        }
        
}
                
