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

namespace Diva.Editor.Gui {

        using System;
        using Gtk;
        using Mono.Unix;
        using Util;
        using Model;
        
        public class OrganizeByComboBox : Gtk.ComboBox {
                
                // Translatable ///////////////////////////////////////////////
                                
                readonly static string byTypeSS = Catalog.GetString
                        ("Everything by type");
                
                readonly static string byLengthSS = Catalog.GetString
                        ("Everything by length");

                readonly static string byUsageSS = Catalog.GetString
                        ("Everything by usage");
                
                readonly static string byTagSS = Catalog.GetString
                        ("Everything by tag");

                readonly static string whatIsItSS = Catalog.GetString
                        ("Change the media organizing method");

                // Properties /////////////////////////////////////////////////
                
                Model.OrganizeByMethod SelectedMethod {
                        get {
                                TreeIter iter;
                                GetActiveIter (out iter);
                                return (OrganizeByMethod) listStore.GetValue (iter, 1);
                        }
                        set {
                                TreeIter methodIter = GtkFu.TreeModelIterByInt (listStore, 
                                                                                (int) value, 1);
                                trickSwitch = true;
                                SetActiveIter (methodIter);
                                trickSwitch = false;
                        }
                }

                // Fields /////////////////////////////////////////////////////

                Model.Root modelRoot = null;
                Gtk.ListStore listStore = new Gtk.ListStore (typeof (string), 
                                                             typeof (Model.OrganizeByMethod));
                bool trickSwitch = false; // To handle recursive events, Gtk suckage
                long messageToken = -1;   // Token 

                // Public methods /////////////////////////////////////////////

                /* Constructor */
                public OrganizeByComboBox (Model.Root root) : base ()
                {
                        modelRoot = root;
                        HasFrame = false;
                        // Append the various organization methods
                        listStore.AppendValues (byTypeSS, OrganizeByMethod.ByType);
                        listStore.AppendValues (byLengthSS, OrganizeByMethod.ByLength);
                        listStore.AppendValues (byUsageSS, OrganizeByMethod.ByUsage);
                        listStore.AppendValues (byTagSS, OrganizeByMethod.ByTag);
                        
                        Model = listStore;
       
                        // Renderers 
                        Gtk.CellRendererText textRenderer = new Gtk.CellRendererText ();
 
                        // Pack them
                        PackStart (textRenderer, true);
                        
                        // Set the cell renderers
                        AddAttribute (textRenderer, "text", 0);
                        
                        SelectedMethod = modelRoot.Stuff.OrganizeByMethod;

                        // Bind to model
                        modelRoot.Stuff.OrganizeByMethodChange += OnOrganizeByMethodChanged;
                }

                // Private methods ////////////////////////////////////////////
 
                /* Selection was changed */
                protected override void OnChanged ()
                {
                        if (trickSwitch)
                                return;
                        
                        modelRoot.Stuff.OrganizeByMethod = SelectedMethod;
                }
                
                /* Changed on the model side */
                public void OnOrganizeByMethodChanged (object o, Model.OrganizeByMethodArgs args)
                {
                        SelectedMethod = args.Method;
                }

                protected override bool OnEnterNotifyEvent (Gdk.EventCrossing evnt)
                {
                        if (messageToken != -1) {
                                modelRoot.Window.PopMessage (messageToken);
                                messageToken = -1;
                        }

                        messageToken = modelRoot.Window.PushMessage (whatIsItSS, Editor.Model.MessageLayer.Widget2);
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
