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

/* A normal button that contains an icon and (possibly) a label */

namespace Diva {

        using System;
        using Gtk;
        using Gdk;
        using Mono.Unix;
        using Util;
        
        public class MediaButton : Button {
                
                // Public methods  /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public MediaButton (string stock, string label) : base ()
                {
                        HBox hBox = new HBox (false, 6);
                                                
                        // Create 
                        Gtk.Image iconImage = new Gtk.Image (IconFu.GetStockIcon (stock, IconSize.Button));
                        Label labelLabel = new Label (label);
                        
                        hBox.PackStart (iconImage, false, false, 0);
                        hBox.PackStart (labelLabel, true, true, 0);
                        
                        Add (hBox);
                }
                
                /* CONSTRUCTOR */
                public MediaButton (string stock) : base ()
                {
                        Gtk.Image iconImage = new Gtk.Image (IconFu.GetStockIcon (stock, IconSize.Button));
                        Add (iconImage);
                }
                
        }

}
