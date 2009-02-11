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

namespace Diva.Widgets {
        
        using System;
        using Gtk;
        using System.Collections;
        using Mono.Unix;
        
        public class OpenMediaDialog : FileChooserDialog {

                // Properties ///////////////////////////////////////////////////

                public string[] Files {
                        get {
                                // We "copy" the array not to get affected by Destroy
                                
                                string[] ret = new string [Filenames.Length];
                                
                                for (int i = 0; i < Filenames.Length; i++)
                                        ret[i] = Filenames[i];
                                
                                return ret;
                        }
                }
                                
                // Translatable ////////////////////////////////////////////////

                readonly static string addMediaSS = Catalog.GetString
                        ("Select media files");

                readonly static string allMediaSS = Catalog.GetString
                        ("All digital media");

                readonly static string videoClipsSS = Catalog.GetString
                        ("Video clips");

                readonly static string soundAndMusicSS = Catalog.GetString
                        ("Sound & music");

                readonly static string photosSS = Catalog.GetString
                        ("Photos");



                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public OpenMediaDialog (Window parent) : base (addMediaSS, parent, FileChooserAction.Open,
                                                               Stock.Cancel, ResponseType.Cancel,
                                                               Stock.Ok, ResponseType.Accept)
                {
                        SelectMultiple = true;

                        // FIXME: Filters should be dependent on Gdv.Application.Has* 
                        
                        Gtk.FileFilter allFilter = new FileFilter ();
                        allFilter.AddMimeType ("video/x-msvideo");
                        allFilter.AddMimeType ("video/mpeg");
                        allFilter.AddMimeType ("audio/mpeg");
                        allFilter.AddMimeType ("application/ogg"); 
                        allFilter.AddPattern ("*.dv"); 
                        allFilter.AddMimeType ("audio/x-wav");
                        allFilter.AddMimeType ("image/jpeg");
                        allFilter.AddMimeType ("image/png");
                        allFilter.Name = allMediaSS;
                        AddFilter (allFilter);
                        
                        Gtk.FileFilter movieFilter = new FileFilter ();
                        movieFilter.AddMimeType ("video/x-msvideo");
                        movieFilter.AddMimeType ("video/mpeg");
                        movieFilter.AddPattern ("*.dv"); 
                        movieFilter.Name = videoClipsSS;
                        AddFilter (movieFilter);

                        Gtk.FileFilter audioFilter = new FileFilter ();
                        audioFilter.AddMimeType ("audio/mpeg");
                        audioFilter.AddMimeType ("application/ogg"); 
                        audioFilter.AddMimeType ("audio/x-wav"); 
                        audioFilter.Name = soundAndMusicSS;
                        AddFilter (audioFilter);
                        
                        Gtk.FileFilter imageFilter = new FileFilter ();
                        imageFilter.AddMimeType ("image/jpeg");
                        imageFilter.AddMimeType ("image/png");
                        imageFilter.Name = photosSS;
                        AddFilter (imageFilter);
                }

        }
        
}
                
