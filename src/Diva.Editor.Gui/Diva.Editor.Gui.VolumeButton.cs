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
        
        public class VolumeButton : Button {

                // Fields //////////////////////////////////////////////////////

                VolumePopupWindow popupWindow = null;
                Model.Root modelRoot = null;
                Image iconImage = null;
                string currentIconId;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public VolumeButton (Model.Root root)
                {
                        modelRoot = root;

                        currentIconId = GetStringForVolume (modelRoot.Pipeline.Volume);
                        Gdk.Pixbuf pixbuf = IconFu.GetStockIcon (currentIconId, IconSize.Menu);
                        iconImage = new Image (pixbuf);
                        Add (iconImage);

                        Relief = ReliefStyle.None;
                        CanFocus = false;

                        // Model bind
                        modelRoot.Pipeline.VolumeChange += OnPipelineVolumeChanged;
                }

                // Private methods /////////////////////////////////////////////

                protected override void OnClicked ()
                {
                        if (popupWindow == null) {
                                
                                popupWindow = new VolumePopupWindow (modelRoot);
                                ConfigurePopup (popupWindow);
                                                                
                                popupWindow.ShowAll ();
                                popupWindow.GrabMe ();
                                popupWindow.PopDown += OnPopDown;
                        }
                }

                void OnPopDown (object o, EventArgs args)
                {
                        popupWindow = null;
                }

                void ConfigurePopup (VolumePopupWindow popup)
                {
                        int x, y;
                                                
                        GdkWindow.GetOrigin (out x, out y);

                        popup.Gravity = Gdk.Gravity.SouthWest;
                        popup.Move (x + Allocation.X, y + Allocation.Y - 150);
                }
                
                string GetStringForVolume (double volume)
                {
                        string id = "audio-volume-";
                                
                        if (volume <= 0)
                                id += "muted";
                        else if (volume <= 0.3)
                                id += "low";
                        else if (volume <= 0.6)
                                id += "medium";
                        else
                                id += "high";

                        return id;
                }

                void OnPipelineVolumeChanged (object o, Model.PipelineVolumeArgs args)
                {
                        string newId = GetStringForVolume (args.Volume);

                        if (newId != currentIconId) {
                                iconImage.SetFromStock (newId, IconSize.LargeToolbar);
                                currentIconId = newId;
                                Gdk.Pixbuf pixbuf = IconFu.GetStockIcon (currentIconId, IconSize.Menu);
                                iconImage.Pixbuf = pixbuf;
                        }
                }
                
        }
        
}