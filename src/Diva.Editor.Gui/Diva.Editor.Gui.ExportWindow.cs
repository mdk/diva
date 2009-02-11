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

        using Gtk;
        using Gdv;
        using System;
        using Mono.Unix;
        using Util;

        public class ExportWindow : Gtk.Window {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("Exporting");

                readonly static string exportSS = Catalog.GetString
                        ("Please wait while exporting movie");

                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot;    // App model
                ProgressBar progressBar; // Porgress bar we're updating
                Time maximumTime;        // Maximum time we want to reach

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ExportWindow (Model.Root root, Gtk.Window parent) : base (titleSS)
                {
                        modelRoot = root;
                        maximumTime = modelRoot.Export.ExportMax;
                        
                        // Abort button
                        Button abortButton = new Button (Stock.Cancel);
                        abortButton.Clicked += OnAbortClicked;

                        // Lower button box
                        HButtonBox buttonBox = new HButtonBox ();
                        buttonBox.Layout = ButtonBoxStyle.End;
                        buttonBox.Add (abortButton);
                        
                        // Label
                        Label messageLabel = new Label (exportSS);

                        // Progress
                        progressBar = new ProgressBar ();

                        // Upper VBox
                        VBox upperVBox = new VBox (false, 6);
                        upperVBox.PackStart (messageLabel, false, false, 0);
                        upperVBox.PackStart (progressBar, false, false, 0);
                                                
                        // Main VBox
                        VBox mainVBox = new VBox (false, 12);
                        mainVBox.PackStart (upperVBox, false, false, 0);
                        mainVBox.PackStart (buttonBox, false, false, 0);

                        BorderWidth = 12;
                        Add (mainVBox);
                        
                        // Set modal
                        Modal = true;
                        TransientFor = parent;
                        Resizable = false;
                        WindowPosition = WindowPosition.CenterOnParent;

                        // Model bind
                        modelRoot.Pipeline.Ticker += OnTicker;
                }

                public void UnBind ()
                {
                        modelRoot.Pipeline.Ticker -= OnTicker;
                }

                // Private methods /////////////////////////////////////////////
                
                void OnTicker (object o, Model.PipelineTickerArgs args)
                {
                        double fraction = args.Time.Seconds / maximumTime.Seconds;
                        fraction = Math.Min (1.0, fraction);

                        progressBar.Fraction = (fraction);
                        progressBar.Text = TimeFu.ToSMPTE (args.Time,
                                                           modelRoot.ProjectDetails.Format.VideoFormat.Fps);
                }

                void OnAbortClicked (object o, EventArgs args)
                {
                        modelRoot.Export.Abort ();
                }

        }

}