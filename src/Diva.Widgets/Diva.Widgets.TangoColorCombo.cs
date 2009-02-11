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
        using Gdk;
        using Mono.Unix;
        using System.Collections.Generic;
        using System.Collections;

        public enum TangoColorSet { All, MidTones };

        public class TangoColorCombo : ComboBox {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string selectionTitleSS = Catalog.GetString 
                        ("Tag color");
                
                // Fields //////////////////////////////////////////////////////
                
                TreeIter customizableIter = TreeIter.Zero;
                bool trickSwitch = false;
 
                // Properties //////////////////////////////////////////////////
                
                public Gdv.Color SelectedColor {
                        get { 
                                TreeIter iter = TreeIter.Zero;
                                if (! GetActiveIter (out iter))
                                        return Gdv.Color.Zero;
                                
                                GLib.Value val = new GLib.Value ();
                                (Model as ListStore).GetValue (iter, 0, ref val);
                                return (Gdv.Color) val;
                        }
                        set {
                                // First trt by finding...
                                TreeIter iter;
                                (Model as ListStore).GetIterFirst (out iter);
                                
                                do {
                                        GLib.Value val = new GLib.Value ();
                                        (Model as ListStore).GetValue (iter, 0, ref val);
                                        if ((Gdv.Color) val == value) {
                                                trickSwitch = true;
                                                SetActiveIter (iter);
                                                trickSwitch = false;
                                                return;
                                        }
                                } while ((Model as ListStore).IterNext (ref iter));
                                
                                // We weren't able to find it...
                                if (customizableIter.Stamp == TreeIter.Zero.Stamp)
                                        return;
                                
                                (Model as ListStore).SetValue (customizableIter, 0, value);
                                trickSwitch = true;
                                SetActiveIter (customizableIter);
                                trickSwitch = false;
                        }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TangoColorCombo (TangoColorSet set, bool customizable) : base ()
                {
                        // Model
                        Model = new ListStore (typeof (Gdv.Color), typeof (bool));

                        // Let's see what initial color set we need...
                        switch (set) {
                                case TangoColorSet.All:
                                        FillWithAll ();
                                        break;

                                case TangoColorSet.MidTones:
                                        FillWithMidtones ();
                                        break;
                        }

                        if (customizable)
                                AddCustomizable ();

                        TreeIter firstColorIter;
                        (Model as ListStore).GetIterFirst (out firstColorIter);
                        SetActiveIter (firstColorIter);
                        
                        // Renderers
                        PaintRenderer paintRenderer = new PaintRenderer ();
                        paintRenderer.Customize = true;
                        PackStart (paintRenderer, true);
                        AddAttribute (paintRenderer, "color", 0);
                        AddAttribute (paintRenderer, "customize", 1);
                }
                
                /* Select a random color */
                public void Randomize ()
                {
                        List <TreeIter> iters = new List <TreeIter> ();
                        
                        TreeIter currentIter;
                        if (! (Model as ListStore).GetIterFirst (out currentIter))
                                return;
                        
                        // Iterate over all. Not nice
                        do {
                                if ((Model as ListStore).GetPath (currentIter) != 
                                    (Model as ListStore).GetPath (customizableIter))
                                        iters.Add (currentIter);
                        } while ((Model as ListStore).IterNext (ref currentIter));
                        
                        if (iters.Count <= 1)
                                return;
                        
                        Random rand = new Random ();
                        int val = rand.Next (0, iters.Count - 1);
                        
                        trickSwitch = true;
                        SetActiveIter (iters [val]);
                        trickSwitch = false;
                }

                // Private methods /////////////////////////////////////////////
                
                /* Fill our combo/store with all the default tango colors */
                void FillWithAll ()
                {
                        foreach (Gdv.ColorTango color in Enum.GetValues (typeof (Gdv.ColorTango)))
                                (Model as ListStore).AppendValues (Gdv.Color.FromTango (color), false);
                }

                /* Fill our combo/store with all the default tango colors */
                void FillWithMidtones ()
                {
                        Gdv.ColorTango[] colors = (Gdv.ColorTango []) Enum.GetValues (typeof (Gdv.ColorTango));
                        
                        for (int i = 1; i < colors.Length; i += 2)
                                (Model as ListStore).AppendValues (Gdv.Color.FromTango (colors [i]), false);
                }

                void AddCustomizable ()
                {
                        customizableIter = (Model as ListStore).AppendValues (new Gdv.Color (0.0f, 0.0f, 0.0f, 0.0f), true);
                }

                /* The selection was changed */
                protected override void OnChanged ()
                {
                        if (trickSwitch)
                                return;
                        
                        TreeIter iter = TreeIter.Zero;
                        if (! GetActiveIter (out iter))
                                return;
                        
                        bool customize = (bool) (Model as ListStore).GetValue (iter, 1);

                        if (customize == true) {
                                // Fire up the color selection window
                                ColorSelectionDialog selectionDialog = new ColorSelectionDialog (selectionTitleSS);
                                selectionDialog.ColorSelection.HasOpacityControl = false;
                                selectionDialog.ColorSelection.HasPalette = false;
                                
                                // If the user actually selected the color...
                                if (selectionDialog.Run () == (int) ResponseType.Ok) {
                                        Gdv.Color newColor = Gdv.Color.FromGdk (selectionDialog.ColorSelection.CurrentColor);
                                        (Model as ListStore).SetValue (iter, 0, newColor);
                                }

                                selectionDialog.Destroy ();
                        }
                }

        }

}

