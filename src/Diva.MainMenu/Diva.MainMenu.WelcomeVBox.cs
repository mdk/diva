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

/* This VBox contains the main menu just after the splash screen 
 */

namespace Diva.MainMenu {

        using System;
        using Mono.Unix;
        using Gtk;
	using Util;
	using Widgets;
	
        public class WelcomeVBox : VBox {
                
                // Fields //////////////////////////////////////////////////////
                
                Button helpButton = null;
                Button closeButton = null;
                HButtonBox buttonBox = null;
                VButtonBox menuBox = null;
                Model model = null;
                                
                // Translatable ///////////////////////////////////////////////
                
                readonly static string welcomeSS = Catalog.GetString 
                        ("<b>Welcome to Diva</b>\n");
		
                readonly static string newSS = Catalog.GetString
                        ("Start editing a new movie");
                
                readonly static string preferencesSS = Catalog.GetString
                        ("Preferences");

                readonly static string openSS = Catalog.GetString
                        ("Open a project");
                
                readonly static string captureSS = Catalog.GetString 
                        ("Capture media");
                
                readonly static string pruneSS = Catalog.GetString 
                        ("Prune projects and media files");

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public WelcomeVBox (Model model) : base (false, 6)
                {
                        this.model = model;
                                                                        
			// Image
			Image welcomeLogo = new Gtk.Image (Util.IconFu.MainDivaIcon96);

		        // Labels
       			Label welcomeLabel = new Gtk.Label (welcomeSS);
			welcomeLabel.UseMarkup = true;
                        welcomeLabel.Justify = Justification.Center;

                        // Buttons
                        helpButton = new Button (Stock.Help);
                        closeButton = new Button (Stock.Close);
                        closeButton.Clicked += OnCloseClicked;
                        
                        // Button box
                        buttonBox = new HButtonBox ();
                        buttonBox.Add (helpButton);
                        buttonBox.Add (closeButton);
                        buttonBox.Spacing = 6;

                        // Menu
                        OffsettedButton newButton = new OffsettedButton (newSS, "gnome-multimedia");
                        newButton.Clicked += OnNewProjectClicked;
                        OffsettedButton preferencesButton = new OffsettedButton (preferencesSS, Stock.Preferences);
                        preferencesButton.Clicked += OnPreferencesClicked;
			OffsettedButton openButton = new OffsettedButton (openSS, Stock.Open);
                        openButton.Clicked += OnOpenProjectClicked;
                        OffsettedButton captureButton = new OffsettedButton (captureSS, Stock.MediaRecord);
                        captureButton.Clicked += OnCaptureClicked;
			OffsettedButton pruneButton = new OffsettedButton (pruneSS, "gnome-fs-trash-empty");
			pruneButton.Clicked += OnPruneClicked;
                        
                        // Menu button box
                        menuBox = new VButtonBox ();
                        menuBox.Layout = ButtonBoxStyle.Start;
                        menuBox.Spacing = 6;
                        menuBox.PackStart (newButton);
                        menuBox.PackStart (openButton);
                        menuBox.PackStart (captureButton);
                        menuBox.PackStart (pruneButton);
                        menuBox.PackStart (preferencesButton);
                                                                        
                        // Pack it
			PackStart (welcomeLogo, false, false, 0);
                        PackStart (welcomeLabel, false, false, 0);
                        PackStart (menuBox, true, true, 0);
                        PackEnd (buttonBox, false, false, 0);
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnNewProjectClicked (object o, EventArgs args)
                {
                        model.SwitchToComponent (MenuComponent.NewProject);
                }
                
                void OnOpenProjectClicked (object o, EventArgs args)
                {
                        model.SwitchToComponent (MenuComponent.OpenProject);
                }
                
                void OnCloseClicked (object o, EventArgs args)
                {
                        model.QuitApplication (0);
                }
                
                void OnCaptureClicked (object o, EventArgs args)
                {
                        FastDialog.InfoOkNotImplemented (null);
                }
                
                void OnPruneClicked (object o, EventArgs args)
                {
                        FastDialog.InfoOkNotImplemented (null);
                }
                
                void OnPreferencesClicked (object o, EventArgs args)
                {
                        FastDialog.InfoOkNotImplemented (null);
                }
                
        }

}
