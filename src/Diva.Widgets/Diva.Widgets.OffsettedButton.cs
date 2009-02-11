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

/* A normal button that contains an icon and a label. Unlike the stock Gtk.Button,
 * it's always left-aligned.
 * Can be constructed from a stock icon or a custom pixbuf.
 */

namespace Diva.Widgets {

        using System;
        using Gtk;
        using Gdk;
        using Util;
        
        public sealed class OffsettedButton : Button {
                
                // Public methods  ////////////////////////////////////////////
                                
                /* CONSTRUCTOR */
                public OffsettedButton (string label, string stock) : this (label,
                                                                            IconFu.GetStockIcon (stock, IconSize.Button))
                {
                }

                /* CONSTRUCTOR */
                public OffsettedButton (string label, Gdk.Pixbuf pixbuf) : base ()
                {
                        Gtk.Image iconImage = new Gtk.Image (pixbuf);
                        Label textLabel = new Label (label);

                        HBox hBox = new HBox (false, 6);
                        hBox.PackStart (iconImage, false, false, 0);
                        hBox.PackStart (textLabel, false, false, 0);
                        
                        Add (hBox);
                }
                
        }

}
