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
        using Util;
        using Mono.Unix;
        
        public class VolumePopupWindow : Gtk.Window {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string plusSS = Catalog.GetString
                        ("+");

                readonly static string minusSS = Catalog.GetString
                        ("-");

                // Events //////////////////////////////////////////////////////

                public event EventHandler PopDown;

                // Fields //////////////////////////////////////////////////////

                Button plusButton = null;
                Button minusButton = null;
                VBox vBox = null;
                Frame frame = null;
                VScale scroller = null;
                bool grabbed = true;
                Model.Root modelRoot = null;
                double storedVolume = 0.0;
                uint volumeSetTimeout = 0;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public VolumePopupWindow (Model.Root root) : base (WindowType.Popup)
                {
                        modelRoot = root;
                        
                        // Plus button
                        plusButton = new Button (plusSS);
                        plusButton.Relief = ReliefStyle.None;
                        plusButton.Clicked += OnPlusClicked;

                        // Minus button
                        minusButton = new Button (minusSS);
                        minusButton.Relief = ReliefStyle.None;
                        minusButton.Clicked += OnMinusClicked;

                        // Scroller
                        scroller = new VScale (0.0, 1.0, 0.1);
                        storedVolume = modelRoot.Pipeline.Volume;
                        scroller.Value = storedVolume;
                        scroller.DrawValue = false;
                        scroller.HeightRequest = 100;
                        scroller.Inverted = true;
                        scroller.KeyPressEvent += OnScrollerKeyPressed;
                        scroller.ValueChanged += OnScrollerValueChanged;
                        
                        // VBox
                        vBox = new VBox (false, 0);
                        vBox.PackStart (plusButton, false, false, 0);
                        vBox.PackStart (scroller, false, false, 0);
                        vBox.PackEnd (minusButton, false, false, 0);

                        // Frame
                        frame = new Frame ();
                        frame.ShadowType = ShadowType.Out;
                        frame.Add (vBox);

                        Add (frame);
                        grabbed = false;
                }

                public void GrabMe ()
                {
                        if (grabbed == true)
                                return;
                        
                        scroller.GrabFocus ();
                        Grab.Add (this);

                        Gdk.GrabStatus result = Gdk.Pointer.Grab (GdkWindow, true,
                                                                  Gdk.EventMask.ButtonPressMask   |
                                                                  Gdk.EventMask.ButtonReleaseMask |
                                                                  Gdk.EventMask.ButtonMotionMask,
                                                                  null, null, 0); // FIXME: No gtk-sharp equiv

                        if (result == Gdk.GrabStatus.Success) {
				result = Gdk.Keyboard.Grab (GdkWindow, 
                                                            true, 0);

				if (result != Gdk.GrabStatus.Success)
					Grab.Remove (this);
                                else
                                        grabbed = true;
			} else 
				Grab.Remove (this);
                }
                
                public void UnGrabMe ()
                {
                        if (grabbed == false)
                                
                        Grab.Remove (this);
                        Gdk.Pointer.Ungrab (0);  // FIXME: Not gtk-sharp equiv
                        Gdk.Keyboard.Ungrab (0); // FIXME: Not gtk-sharp equiv
                        grabbed = false;
                }
                                
                // Private methods /////////////////////////////////////////////

                protected override bool OnButtonPressEvent (Gdk.EventButton evnt)
                {
                        Kill (true);
                        return true;
                }

                void RestoreVolume ()
                {
                        modelRoot.Pipeline.Volume = storedVolume;
                }

                bool OnVolumeSetTimeout ()
                {
                        modelRoot.Pipeline.Volume = scroller.Value;
                        volumeSetTimeout = 0;
                        
                        return false;
                }

                void OnScrollerKeyPressed (object obj, KeyPressEventArgs args)
		{
			switch (args.Event.Key) {
                                
                                case Gdk.Key.Escape:
                                        RestoreVolume ();
                                        Kill (false);
                                        break;
                                        
                                case Gdk.Key.KP_Enter:
                                case Gdk.Key.ISO_Enter:
                                case Gdk.Key.Key_3270_Enter:
                                case Gdk.Key.Return:
                                case Gdk.Key.space:
                                case Gdk.Key.KP_Space:
                                        Kill (true);
                                        break;
                                        
                                default:
                                        break;
			}
		}

                void Kill (bool setVolume)
                {
                        if (setVolume == true && volumeSetTimeout != 0) {
                                modelRoot.Pipeline.Volume = scroller.Value;
                                GLib.Source.Remove (volumeSetTimeout);
                                volumeSetTimeout = 0;
                        }
                        
                        UnGrabMe ();

                        if (PopDown != null)
                                PopDown (this, new EventArgs ());
                                        
                        Destroy ();
                }

                void OnScrollerValueChanged (object o, EventArgs e)
                {
                        if (volumeSetTimeout != 0)
                                GLib.Source.Remove (volumeSetTimeout);

                        volumeSetTimeout = GLib.Timeout.Add (100, OnVolumeSetTimeout);
                }

                void OnPlusClicked (object o, EventArgs args)
                {
                        scroller.Value = Math.Min (scroller.Value + 0.1, 1.0);
                }

                void OnMinusClicked (object o, EventArgs args)
                {
                        scroller.Value = Math.Max (scroller.Value - 0.1, 0.0);
                }
                
        }
        
}