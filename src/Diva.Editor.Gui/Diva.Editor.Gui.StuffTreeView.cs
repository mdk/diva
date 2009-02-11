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

/* An overriden TreeView that uses cairo-rendered thumbnails as the pixbufs. 
 * MediaItems ("clips") & other things are availible here. Can be "organized" 
 * using labels or some generic methods
 */

namespace Diva.Editor.Gui {

        using System;
        using Gtk;
        using Gdk;
        using System.Collections;
        using Widgets;
        using Model;
        using Mono.Unix;
        
        public class StuffTreeView : TreeView {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string whatIsItSS = Catalog.GetString
                        ("Manage and organize your media assets");
                
                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                long messageToken = -1; // Token 

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public StuffTreeView (Model.Root root) : base ()
                {
                        modelRoot = root;
                        Model = modelRoot.Stuff.GtkModel;
                                                
                        ThmbRenderer thmbRenderer = new ThmbRenderer ();
                        TaggyRenderer taggyRenderer = new TaggyRenderer ();
                        TreeViewColumn clmn = new TreeViewColumn ();

                        clmn.PackStart (taggyRenderer, true);
                        clmn.PackStart (thmbRenderer, false);
                        clmn.AddAttribute (thmbRenderer, "pixbuf", 0);
                        clmn.AddAttribute (thmbRenderer, "border", 1);
                        clmn.AddAttribute (taggyRenderer, "major", 2);
                        clmn.AddAttribute (taggyRenderer, "minor", 3);
                        clmn.AddAttribute (taggyRenderer, "tags", 4);

                        //clmn.Sizing = TreeViewColumnSizing.Autosize;

                        AppendColumn (clmn);

                        HeadersVisible = false;
                        RulesHint = false;
                        
                        // D&D
                        SetupDragAndDrop ();
                        
                        // Bind to model
                        modelRoot.Stuff.GtkModelChange += OnGtkModelChange;
                        
                        // For messages
                        AddEvents ((int) Gdk.EventMask.LeaveNotifyMask);
                        AddEvents ((int) Gdk.EventMask.EnterNotifyMask);
                }

                // Private methods /////////////////////////////////////////////
                
                void OnGtkModelChange (object o, GtkModelArgs args)
                {
                        Model = args.Model;
                }
                                                
                /* Setup the TreeView for drag operations. This is called by the 
                 * constructor actually */
                void SetupDragAndDrop ()
                {
                        // Targets
                        TargetEntry timelineTarget = new TargetEntry ("timeline", TargetFlags.App,
                                                                      0);
                        
                        TargetEntry[] targets = new TargetEntry[1];
                        targets[0] = timelineTarget;
                        
                        EnableModelDragSource (ModifierType.Button1Mask, targets, DragAction.Link);
                }
                
                protected override void OnDragBegin (Gdk.DragContext context)
                {
                        TreeIter iter;
                        Core.Stuff clickedStuff = null;
                        
                        if (Selection.GetSelected (out iter) == false)
                                return;
                        else
                                clickedStuff = modelRoot.Stuff.GetStuffForIter (iter);
                                
                        if (clickedStuff == null)
                                return;
                        
                        modelRoot.Drag.Start (clickedStuff);
                        
                        if (clickedStuff.DragPixbuf != null)
                                Gtk.Drag.SetIconPixbuf (context, clickedStuff.DragPixbuf, 1, 1);
                }
                
                protected override void OnDragEnd (Gdk.DragContext context)
                {
                        modelRoot.Drag.SoftStop ();
                }
                
                // Events /////////////////////////////////////////////////////
                
                protected override bool OnButtonPressEvent (Gdk.EventButton evnt)
                {
                        base.OnButtonPressEvent (evnt);
                        
                        // Right mouse button
                        if (evnt.Button == 3) {
                                
                                TreeIter iter;
                                Core.Stuff clickedStuff = null;
                                
                                if (Selection.GetSelected (out iter) == false)
                                        return true;
                                else
                                        clickedStuff = modelRoot.Stuff.GetStuffForIter (iter);
                                
                                // That means we have clicked a node
                                if (clickedStuff == null)
                                        return true;

                                StuffMenu popupMenu = new StuffMenu (modelRoot, clickedStuff);
                                popupMenu.Popup (null, null, null, 3, evnt.Time);
                                popupMenu.ShowAll ();
                        }
                        
                        return true;
                }

                protected override bool OnEnterNotifyEvent (Gdk.EventCrossing evnt)
                {
                        if (messageToken != -1) {
                                modelRoot.Window.PopMessage (messageToken);
                                messageToken = -1;
                        }
                        
                        messageToken = modelRoot.Window.PushMessage (whatIsItSS, Editor.Model.MessageLayer.Widget1);
                        return true;
                }

                protected override bool OnLeaveNotifyEvent (Gdk.EventCrossing evnt)
                {
                        if (messageToken != -1) {
                                modelRoot.Window.PopMessage (messageToken);
                                messageToken = -1;
                        }
                        
                        return true;
                }
                                
        }

}
