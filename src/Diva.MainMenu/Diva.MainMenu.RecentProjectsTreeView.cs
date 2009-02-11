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

namespace Diva.MainMenu {

        using System;
        using Gtk;
        using Core;
        using Util;
        using Mono.Unix;
        
        public class RecentProjectsTreeView : TreeView {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string formatSS =
                        Catalog.GetString ("<b>{0}</b>\nRunning time: {1}");

                // Fields //////////////////////////////////////////////////////

                public static readonly Gdk.Pixbuf iconReel =
                        IconFu.GetStockIcon ("gnome-multimedia", IconSize.SmallToolbar);

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public RecentProjectsTreeView ()
                {
                        // Cell renderers
                        CellRendererPixbuf pixbufRenderer = new CellRendererPixbuf ();
                        CellRendererText textRenderer1 = new CellRendererText ();
                        CellRendererText textRenderer2 = new CellRendererText ();

                        TreeViewColumn clmn1 = new TreeViewColumn ();
                        clmn1.PackStart (pixbufRenderer, false);
                        clmn1.PackStart (textRenderer1, true);
                        clmn1.PackEnd (textRenderer2, false);
                        clmn1.AddAttribute (pixbufRenderer, "pixbuf", 0);
                        clmn1.AddAttribute (textRenderer1, "markup", 1);
                        clmn1.AddAttribute (textRenderer2, "text", 2);
                        clmn1.SortColumnId = 4;
                        AppendColumn (clmn1);

                        HeadersVisible = false;
                        RulesHint = true;
                        Reset ();
                }

                /* Reset the contents */
                public void Reset ()
                {
                        Model = new ListStore (typeof (Gdk.Pixbuf), typeof (string), typeof (string), typeof (string),
                                               typeof (long));
                        (Model as ListStore).SetSortColumnId (4, SortType.Descending);
                }

                public void AddSpoofer (ProjectSpoofer spoofer)
                {
                        (Model as ListStore).AppendValues (iconReel,
                                                           String.Format (formatSS, spoofer.Name, spoofer.Length),
                                                           spoofer.LastSaved.ToShortDateString (),
                                                           spoofer.FileName,
                                                           spoofer.LastSaved.ToFileTime ());
                }

                
                

        }

}
                
                   

                
        
