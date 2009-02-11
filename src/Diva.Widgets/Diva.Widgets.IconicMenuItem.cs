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
        using Util;

        public class IconicMenuItem : Gtk.MenuItem {

                // Fields //////////////////////////////////////////////////////

                Image image; // Here we hold the icon
                Label label; // Our label

                // Properties //////////////////////////////////////////////////

                public string Text {
                        get { return label.Text; }
                        set { label.Text = value; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public IconicMenuItem (string icon, string text) : base ()
                {
                        
                        HBox box = new HBox (false, 6);
                        label = new Gtk.Label (text);
                        label.Xalign = 0.0f;

                        if (icon != null)
                                image = new Image (IconFu.GetStockIcon (icon, Gtk.IconSize.Menu));
                        else {
                                image = new Image ();
                                image.WidthRequest = 16;
                                image.HeightRequest = 16;
                        }
                        
                        box.PackStart (image, false, false, 0);
                        box.PackStart (label, true, true, 0);
                        
                        Add (box);
                }
                
        }
        
}
