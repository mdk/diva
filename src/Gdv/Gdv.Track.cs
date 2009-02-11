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

        public class Track : GLib.Object {
                
                // Imports /////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_track_new
                        (IntPtr handle, int layer);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_track_add_clip
                        (IntPtr handle, IntPtr clipHandle);

                [DllImport ("gdv")]
                internal static extern bool gdv_track_remove_clip
                        (IntPtr handle, IntPtr clipHandle);
                
                // Properties //////////////////////////////////////////////////

                public ClipStore ClipStore {
                        get { return (ClipStore) GetProperty ("clipstore"); }
                }
                
                public ProjectFormat ProjectFormat {
                        get { return (ProjectFormat) GetProperty ("projectformat"); }
                }

                public int Layer {
                        get { return (int) GetProperty ("layer"); }
                }

                public double Opacity {
                        get { return (double) GetProperty ("opacity"); }
                        set { SetProperty ("opacity", new Value (value)); }
                }
                
                public Clip[] Clips {
                        get { 
                                IntPtr ptr = (IntPtr) GetProperty ("clips");
                                if (ptr == IntPtr.Zero) 
                                        return new Clip[0];
                                        
                                List list = new List (ptr, typeof (Clip), true, false);
                                Clip[] clips = new Clip [list.Count];
                                list.CopyTo (clips, 0);
                                
                                return clips;
                        }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Track (IntPtr ptr) : base (ptr)
                {
                }
                
                /* CONSTRUCTOR */
                public Track (ProjectFormat format, int layer) : base (IntPtr.Zero)
                {
                        IntPtr ptr = gdv_track_new (format.Handle, layer);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        
                        Raw = ptr;
                }
                
                public bool AddClip (Clip clip)
                {
                        return gdv_track_add_clip (Handle, clip.Handle);
                }

                public bool RemoveClip (Clip clip)
                {
                        return gdv_track_remove_clip (Handle, clip.Handle);
                }

        }
        
}
                
