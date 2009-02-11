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

/* Utilities for managing/obtaining the icons */



namespace Diva.Util {

        using System;
        using Gtk;

        public static class IconFu {

                // Fields //////////////////////////////////////////////////////

                static Gdk.Pixbuf diva96 = null;
                static Gdk.Pixbuf diva48 = null;
                static Gdk.Pixbuf diva24 = null;
                static Gdk.Pixbuf diva16 = null;
                static Gdk.Pixbuf diva12 = null;

                // Properties //////////////////////////////////////////////////

                public static Gdk.Pixbuf MainDivaIcon96 {
                        get {
                                if (diva96 == null)
                                        diva96 = new Gdk.Pixbuf (null, "diva96.png");
                                return diva96;
                        }
                }

                public static Gdk.Pixbuf MainDivaIcon48 {
                        get {
                                if (diva48 == null)
                                        diva48 = new Gdk.Pixbuf (null, "diva48.png");
                                return diva48;
                        }
                }

                public static Gdk.Pixbuf MainDivaIcon24 {
                        get {
                                if (diva24 == null)
                                        diva24 = new Gdk.Pixbuf (null, "diva24.png");
                                return diva24;
                        }
                }

                public static Gdk.Pixbuf MainDivaIcon16 {
                        get {
                                if (diva16 == null)
                                        diva16 = new Gdk.Pixbuf (null, "diva16.png");
                                return diva16;
                        }
                }

                public static Gdk.Pixbuf MainDivaIcon12 {
                        get {
                                if (diva12 == null)
                                        diva12 = new Gdk.Pixbuf (null, "diva12.png");
                                return diva12;
                        }
                }

                // Public methods //////////////////////////////////////////////
                
                /* Get an icon from the Gnome icon set */
                public static Gdk.Pixbuf GetStockIcon (string icon, Gtk.IconSize size)
                {
                        Gtk.IconSource iconSource = new Gtk.IconSource ();
                        Gtk.IconSet iconSet = new Gtk.IconSet (); 
                        
                        iconSource.IconName = icon;
                        iconSet.AddSource (iconSource);
                        
                        Gdk.Pixbuf pixbuf = iconSet.RenderIcon (Widget.DefaultStyle, TextDirection.None, 
                                                                StateType.Normal, size,
                                                                null, null);

                        return pixbuf;
                }

                /* Initializes the default icon collection to use for all
                 * spawned windows */
                public static void InitDefaultWindowIcon ()
                {
                        Gdk.Pixbuf[] arr = { MainDivaIcon12, MainDivaIcon16,
                                             MainDivaIcon24, MainDivaIcon48 };

                        Window.DefaultIconList = arr;
                }
                
        }
        
}
