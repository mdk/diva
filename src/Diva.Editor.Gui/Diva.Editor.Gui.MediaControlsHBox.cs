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

/* This HBox constains the buttons used to control the timeline playback and
 * the counter that displays the current position */

namespace Diva.Editor.Gui {

        using System;
        using Gtk;
        using Mono.Unix;
        using Gdv;
        using Util;
                
        public class MediaControlsHBox : HBox {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string nextMessageSS = Catalog.GetString
                        ("Seek to next cut");

                readonly static string prevMessageSS = Catalog.GetString
                        ("Seek to previous cut");

                readonly static string rewindMessageSS = Catalog.GetString
                        ("Rewind to start");

                readonly static string counterMessageSS = Catalog.GetString
                        ("Displays current playback cursor position");
                
                // Fields //////////////////////////////////////////////////////

                Label counterLabel;          // The label with the counter
                Model.Root modelRoot = null; // App model

                long nextMessageToken = -1;
                long prevMessageToken = -1;
                long rewindMessageToken = -1;
                long counterMessageToken = -1;
                
                
                // Properties //////////////////////////////////////////////////

                public Time Counter {
                        set { 
                                string str = TimeFu.ToSMPTE (value,
                                                             modelRoot.ProjectDetails.Format.VideoFormat.Fps);
                                counterLabel.Markup = String.Format ("<b><big><big><big>[{0}]</big></big></big></b>",
                                                                     str); 
                        }
                }
                
                // Public methods //////////////////////////////////////////////

                public MediaControlsHBox (Model.Root root) : base (false, 6)
                {
                        modelRoot = root;
                        
                        MediaButton prevCutButton = new MediaButton ("stock_media-prev");
                        prevCutButton.Clicked += OnPrevCutClicked;
                        prevCutButton.Entered += OnPrevCutEntered;
                        prevCutButton.Left += OnPrevCutLeft;
                        
                        MediaButton nextCutButton = new MediaButton ("stock_media-next");
                        nextCutButton.Clicked += OnNextCutClicked;
                        nextCutButton.Entered += OnNextCutEntered;
                        nextCutButton.Left += OnNextCutLeft;
                        
                        MediaButton rewindButton = new MediaButton ("stock_media-rew");
                        rewindButton.Clicked += OnRewindClicked;
                        rewindButton.Entered += OnRewindEntered;
                        rewindButton.Left += OnRewindLeft;
                        
                        PlayButton playButton = new PlayButton (root);

                        counterLabel = new Label ();
                        counterLabel.AddEvents ((int) Gdk.EventMask.LeaveNotifyMask);
                        counterLabel.AddEvents ((int) Gdk.EventMask.EnterNotifyMask);
                        counterLabel.EnterNotifyEvent += OnCounterEntered;
                        counterLabel.LeaveNotifyEvent += OnCounterLeft;
                        
                        PackStart (rewindButton, false, false, 0);
                        PackStart (prevCutButton, false, false, 0);
                        PackStart (playButton, false, false, 0);
                        PackStart (counterLabel, true, true, 0);
                        PackStart (nextCutButton, false, false, 0);
                        
                        Counter = Gdv.Time.Zero;
                        
                        modelRoot.Pipeline.Ticker += OnTicker;
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnTicker (object o, Model.PipelineTickerArgs args)
                {
                        Counter = args.Time;
                }

                void OnPrevCutClicked (object o, EventArgs args)
                {
                        modelRoot.Pipeline.SeekToPrevCut ();
                }

                void OnNextCutClicked (object o, EventArgs args)
                {
                        modelRoot.Pipeline.SeekToNextCut ();
                }
                
                void OnRewindClicked (object o, EventArgs args)
                {
                        modelRoot.Pipeline.SeekToZero ();
                }

                void OnPrevCutEntered (object o, EventArgs args)
                {
                        if (prevMessageToken != -1) {
                                modelRoot.Window.PopMessage (prevMessageToken);
                                prevMessageToken = -1;
                        }
                        
                        prevMessageToken = modelRoot.Window.PushMessage (prevMessageSS, Editor.Model.MessageLayer.Widget2);
                }
                
                void OnPrevCutLeft (object o, EventArgs args)
                {
                        if (prevMessageToken != -1) {
                                modelRoot.Window.PopMessage (prevMessageToken);
                                prevMessageToken = -1;
                        }
                }

                void OnNextCutEntered (object o, EventArgs args)
                {
                        if (nextMessageToken != -1) {
                                modelRoot.Window.PopMessage (nextMessageToken);
                                nextMessageToken = -1;
                        }
                        
                        nextMessageToken = modelRoot.Window.PushMessage (nextMessageSS, Editor.Model.MessageLayer.Widget2);
                }
                
                void OnNextCutLeft (object o, EventArgs args)
                {
                        if (nextMessageToken != -1) {
                                modelRoot.Window.PopMessage (nextMessageToken);
                                nextMessageToken = -1;
                        }
                }

                void OnRewindEntered (object o, EventArgs args)
                {
                        if (rewindMessageToken != -1) {
                                modelRoot.Window.PopMessage (rewindMessageToken);
                                rewindMessageToken = -1;
                        }
                        
                        rewindMessageToken = modelRoot.Window.PushMessage (rewindMessageSS, Editor.Model.MessageLayer.Widget2);
                }
                
                void OnRewindLeft (object o, EventArgs args)
                {
                        if (rewindMessageToken != -1) {
                                modelRoot.Window.PopMessage (rewindMessageToken);
                                rewindMessageToken = -1;
                        }
                }

                void OnCounterEntered (object o, EnterNotifyEventArgs args)
                {
                        if (counterMessageToken != -1) {
                                modelRoot.Window.PopMessage (counterMessageToken);
                                counterMessageToken = -1;
                        }
                        
                        counterMessageToken = modelRoot.Window.PushMessage (counterMessageSS, Editor.Model.MessageLayer.Widget2);
                }
                
                void OnCounterLeft (object o, LeaveNotifyEventArgs args)
                {
                        if (counterMessageToken != -1) {
                                modelRoot.Window.PopMessage (counterMessageToken);
                                counterMessageToken = -1;
                        }
                }

        }

}


