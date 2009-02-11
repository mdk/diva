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

namespace Diva.Widgets.Test {

        using Gtk;
        using System;
        using Widgets;

        public class Window : Gtk.Window {

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public Window () : base ("Widgets test")
                {
                        BorderWidth = 6;

                        TreeView tree = new TreeView ();
                        ListStore store = new ListStore (typeof (string),
                                                         typeof (string),
                                                         typeof (string));

                        store.AppendValues ("Major", "Minor", "brother sister apple");
                        //store.AppendValues ("Clip one", "15s", "blah");
                        //store.AppendValues ("Another clip", "1min", "pear think just enough diva asa nisi");
                        //store.AppendValues (String.Empty, String.Empty, "tag");
                        //store.AppendValues (String.Empty, "Minor goes here", "tag");
                        //store.AppendValues ("Clip name", "Minor goes here", String.Empty);

                        // Renderers
                        TaggyRenderer taggyRenderer = new TaggyRenderer ();
                        
                        // Column
                        TreeViewColumn clmn = new TreeViewColumn ();
                        clmn.PackStart (taggyRenderer, true);
                        clmn.AddAttribute (taggyRenderer, "major", 0);
                        clmn.AddAttribute (taggyRenderer, "minor", 1);
                        clmn.AddAttribute (taggyRenderer, "tags", 2);
                        tree.AppendColumn (clmn);

                        tree.Model = store;
                        tree.HeadersVisible = false;
                        tree.RulesHint = true;

                        SetDefaultSize (320, 240);
                        Add (tree);
                }

                // Private methods /////////////////////////////////////////////

                protected override bool OnDeleteEvent (Gdk.Event evnt)
                {
                        Application.Quit ();
                        return true;
                }

        }
        
}