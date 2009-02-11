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
        
        public abstract class Source : GLib.Object {
                
                // Imports ////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_source_clone
                        (IntPtr handle);
                
                [DllImport ("gdv")]
                internal static extern void gdv_source_get_thumb_dimensions
                        (IntPtr handle, out FrameDimensions dimensions);

                // Propeties //////////////////////////////////////////////////
                
                public Time Length { 
                        get { return (Time) GetProperty ("length"); } 
                }
                
                public bool HasVideo {
                        get { return (bool) GetProperty ("hasvideo"); }
                }
                
                public bool HasAudio {
                        get { return (bool) GetProperty ("hasaudio"); }
                }

                public bool Consistent {
                        get { return (bool) GetProperty ("consistent"); }
                }

                public bool ReadySeekable {
                        get { return (bool) GetProperty ("readyseekable"); }
                }
                
                public VideoFormat VideoFormat {
                        get { return (VideoFormat) GetProperty ("videoformat"); }
                }
                
                public AudioFormat AudioFormat {
                        get { return (AudioFormat) GetProperty ("audioformat"); }
                }

                // Public methods /////////////////////////////////////////////
                
                public Source (IntPtr ptr) : base (ptr)
                {
                }
                
                public FrameDimensions GetThumbnailDimensions ()
                {
                        FrameDimensions dimensions;
                        gdv_source_get_thumb_dimensions (this.Handle, out dimensions);
                        // FIXME: Check for expception if null
                        return dimensions;
                }
                                
                public Source Clone ()
                {
                        IntPtr ptr = gdv_source_clone (this.Handle);
                        if (ptr == IntPtr.Zero)
                                throw new InternalException ("Failed to clone source");
                        else 
                                return ItemGenerator.AutowrapSource (ptr);
                }
                
        }
        
}
                
