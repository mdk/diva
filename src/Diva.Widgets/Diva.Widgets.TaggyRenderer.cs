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
        using System.Runtime.InteropServices;
        using GLib;
        
        public class TaggyRenderer : Gtk.CellRenderer {
                
                // Imports ////////////////////////////////////////////////////
                
                [DllImport ("divawidgets")]
                internal static extern IntPtr diva_widgets_taggy_renderer_new
                        ();

                // Propeties //////////////////////////////////////////////////

                public string Tags {
                        get { return (string) GetProperty ("tags"); }
                        set { SetProperty ("tags", new Value (value)); }
                }

                public string Major {
                        get { return (string) GetProperty ("major"); }
                        set { SetProperty ("major", new Value (value)); }
                }
                
                public string Minor {
                        get { return (string) GetProperty ("minor"); }
                        set { SetProperty ("minor", new Value (value)); }
                }

                // Public methods /////////////////////////////////////////////
                
                public TaggyRenderer () : base (IntPtr.Zero)
                {
                        Raw = diva_widgets_taggy_renderer_new ();
                        // FIXME: Exceptions, etc!!!
                }
                
        }
        
}