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

        public sealed class MessageArea : HBox {
                                
                // Fields //////////////////////////////////////////////////////

                Model.Root modelRoot = null;
                ProgressBar progressBar = null;
                Label messageLabel = null;
                bool switched = false;

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public MessageArea (Model.Root root) : base (false, 0)
                {
                        modelRoot = root;
                        progressBar = new ProgressBar ();
                        messageLabel = new Label (modelRoot.Window.CurrentMessage);
                        messageLabel.Xalign = 0.0f;
                        
                        PackStart (messageLabel, true, true, 0);
                        
                        // Model bind
                        modelRoot.Window.UpdateProgress += OnUpdateProgress;
                        modelRoot.Window.ProgressBarRequest += OnProgressBarRequested;
                        modelRoot.Window.UpdateMessage += OnUpdateMessage;
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnProgressBarRequested (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true && (! switched)) {
                                Remove (messageLabel);
                                PackStart (progressBar, true, true, 0);
                                ShowAll ();
                                switched = true;
                        } else if (args.Requisition == false && switched) {
                                Remove (progressBar);
                                PackStart (messageLabel, true, true, 0);
                                ShowAll ();
                                switched = false;
                        }
                }
                
                void OnUpdateProgress (object o, Model.ProgressArgs args)
                {
                        if (args.PulseOnly) 
                                progressBar.Pulse ();
                        else {
                                progressBar.Text = args.Message;
                                progressBar.Fraction = args.Progress;
                        }
                }

                void OnUpdateMessage (object o, Model.MessageArgs args)
                {
                        messageLabel.Text = args.Message;
                }
                
        }
        
}
