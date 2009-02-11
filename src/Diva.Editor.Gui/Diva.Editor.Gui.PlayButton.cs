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
        using Gdk;
        using Mono.Unix;
        using Util;
        
        public class PlayButton : ToggleButton {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string playSS = Catalog.GetString
                        ("Play");

                readonly static string whatIsItSS = Catalog.GetString
                        ("Play or pause the playback");
                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                bool trickSwitch = false;

                long messageToken = -1;
                
                // Public methods  /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public PlayButton (Model.Root root) : base ()
                {
                        modelRoot = root;
                        HBox hBox = new HBox (false, 6);
                                                
                        // Create
                        Gtk.Image iconImage = new Gtk.Image (IconFu.GetStockIcon ("stock_media-play", IconSize.Button));
                        Label labelLabel = new Label (playSS);
                        
                        hBox.PackStart (iconImage, false, false, 0);
                        hBox.PackStart (labelLabel, true, true, 0);
                        
                        Add (hBox);
                                                
                        CanFocus = false;
                        
                        // Model bind
                        modelRoot.Pipeline.StateChange += OnStateChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                protected override void OnPressed ()
                {
                        if (trickSwitch)
                                return;
                        
                        modelRoot.Pipeline.PlayPause ();
                }
                
                void OnStateChanged (object o, Model.PipelineStateArgs args)
                {
		        
                        trickSwitch = true;
                        Active = args.Playing;
                        trickSwitch = false;
                }

                protected override void OnEntered ()
                {
                        if (messageToken != -1) {
                                modelRoot.Window.PopMessage (messageToken);
                                messageToken = -1;
                        }

                        messageToken = modelRoot.Window.PushMessage (whatIsItSS, Editor.Model.MessageLayer.Widget2);
                }

                protected override void OnLeft ()
                {
                        if (messageToken != -1) {
                                modelRoot.Window.PopMessage (messageToken);
                                messageToken = -1;
                        }
                }

                
                
        }

}
