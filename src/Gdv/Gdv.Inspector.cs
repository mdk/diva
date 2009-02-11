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
        
        public delegate bool InspectorHandler ();

        public class Inspector : GLib.Object {
                
                public struct MediaTags {
                        public int TrackNumber;
                        public string Title;
                        public string Artist;
                        public string Album;
                        public string Genre;
                        public string Comment;
                        
                        public bool HasTrackNumber { get { return (TrackNumber != -1); }}

                        public bool HasTitle { get { return (Title != null && 
                                                             Title != String.Empty); }}
                        
                        public bool HasArtist { get { return (Artist != null &&
                                                              Artist != String.Empty); }}
                        
                        public bool HasAlbum { get { return (Album != null &&
                                                             Album != String.Empty); }}
                        
                        public bool HasGenre { get { return (Genre != null &&
                                                             Genre != String.Empty); }}
                        
                        public bool HasComment { get { return (Comment != null &&
                                                               Comment != String.Empty); }}
                
                        public bool HasAnyTag { 
                                get {
                                        if (HasTrackNumber ||
                                            HasTitle       ||
                                            HasArtist      ||
                                            HasAlbum       ||
                                            HasGenre       ||
                                            HasComment)
                                                return true;
                                        else
                                                return false;
                                }
                                
                        }
                                        
                }
            
                // Imports ////////////////////////////////////////////////////
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_inspector_new
                        (IntPtr filename);

                [DllImport ("gdv")]
                internal static extern int gdv_inspector_get_tag_type
                        (IntPtr obj, [MarshalAs(UnmanagedType.LPStr)] string tag);

                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_get_int_tag
                        (IntPtr obj, [MarshalAs(UnmanagedType.LPStr)] string tag, out Int64 val);

                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_get_uint_tag
                        (IntPtr obj, [MarshalAs(UnmanagedType.LPStr)] string tag, out UInt64 val);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_set_done_func
                        (IntPtr obj, InspectorHandler handlerc);

                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_set_error_func
                        (IntPtr obj, InspectorHandler handlerc);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_start
                        (IntPtr obj);
                
                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_abort
                        (IntPtr obj);

                [DllImport ("gdv")]
                internal static extern bool gdv_inspector_get_string_tag
                        (IntPtr obj, [MarshalAs(UnmanagedType.LPStr)] string tag,
                         [MarshalAs(UnmanagedType.LPStr)] out string val);
                
                // Propeties //////////////////////////////////////////////////
                
                public string Url {
                        get { return (string) GetProperty ("url"); }
                }
                
                public string Mime {
                        get { return (string) GetProperty ("mime"); }
                }
                
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
                
                public Time Length {
                        get { return (Time) GetProperty ("length"); }
                }
                
                public string Error {
                        get { 
                                IntPtr ptr = (IntPtr) GetProperty ("error");
                                return Glue.GErrorGetString (ptr);
                        }
                }

                public MediaTags Tags {
                        get {
                                MediaTags t = new MediaTags();
                                gdv_inspector_get_string_tag (this.Handle, "title", out t.Title);
                                gdv_inspector_get_string_tag (this.Handle, "artist", out t.Artist);
                                gdv_inspector_get_string_tag (this.Handle, "album", out t.Album);
                                gdv_inspector_get_string_tag (this.Handle, "genre", out t.Genre);
                                gdv_inspector_get_string_tag (this.Handle, "comment", out t.Comment);

                                Int64 tmp;
                                if (gdv_inspector_get_int_tag (this.Handle, "track-number", out tmp))
                                        t.TrackNumber = (int) tmp;
                                else
                                        t.TrackNumber = -1;

                                return t;
                        }
                }

                // Public methods /////////////////////////////////////////////
                
                public Inspector (IntPtr ptr) : base (ptr)
                {
                }
                
                public Inspector (string fileName) : base (IntPtr.Zero)
                {
                        IntPtr fileNamePtr = Marshaller.StringToPtrGStrdup (fileName);
                        IntPtr raw = gdv_inspector_new (fileNamePtr);
                        Marshaller.Free (fileNamePtr);
                        
                        this.Raw = raw;
                }
                
                public void SetDoneFunc (InspectorHandler handler)
                {
                        gdv_inspector_set_done_func (Handle, handler);
                }
                
                public void SetErrorFunc (InspectorHandler handler)
                {
                        gdv_inspector_set_error_func (Handle, handler);
                }
                
                public void Start ()
                {
                        gdv_inspector_start (Handle);
                }
                
                public void Abort ()
                {
                        gdv_inspector_abort (Handle);
                }


                
        }
        
}
                
