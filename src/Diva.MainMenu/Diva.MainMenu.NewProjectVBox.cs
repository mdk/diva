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
        using Widgets;
        using AspectFrame = Widgets.AspectFrame;
        using Core;
        using System.IO;
        using Util;
        
        public class NewProjectVBox : VBox {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string locationSS = Catalog.GetString
                        ("Location:");

                readonly static string defaultNameSS = Catalog.GetString
                        ("My movie");

                readonly static string nameSS = Catalog.GetString
                        ("Name:");
                
                readonly static string formatSS = Catalog.GetString
                        ("Format:");

                readonly static string folderSS = Catalog.GetString
                        ("Create or select a folder");
                
                readonly static string overwriteSS = Catalog.GetString
                        ("Another project with the same name is present in the directory. " +
                         "Are you sure that you want to overwrite it?");
                
                readonly static string overwriteHeaderSS = Catalog.GetString
                        ("Overwrite project?");
                
                readonly static string invalidNameSS = Catalog.GetString
                        ("This is not a valid project name. " +
                         "Project name can contain letters, digits and whitespace. " + 
                         "Please provide a valid name.");
                
                readonly static string invalidNameHeaderSS = Catalog.GetString
                        ("Invalid name");
                
                readonly static string invalidDirectorySS = Catalog.GetString
                        ("The directory doesn't exsist or is not writable. " +
                         "Please select a different directory.");
                
                readonly static string invalidDirectoryHeaderSS = Catalog.GetString
                        ("Invalid directory");

                // Fields //////////////////////////////////////////////////////

                AspectFrame aspectFrame;          // This displays the current aspect/resolution
                FileChooserButton locationButton; // Project location (directory)
                Entry nameEntry;                  // Name of the project
                FormatsComboBox formatsComboBox;  // The combo with project format
                Model model;                      // App model

                // Public methods //////////////////////////////////////////////
                                
                /* CONSTRUCTOR */
                public NewProjectVBox (Model model) : base (false, 12)
                {
                        this.model = model;
                        
                        aspectFrame = new AspectFrame ();
                        
                        // Container with the main labels/edits
                        Table medTable = new Table (3, 2, false);
                        medTable.ColumnSpacing = 6;
                        medTable.RowSpacing = 6;
                        
                        // Project name
                        Label nameLabel = new Label (nameSS);
                        nameLabel.Xalign = 0.0f;
                        nameEntry = new Entry ();
                        nameEntry.Text = defaultNameSS;
                        medTable.Attach (nameLabel, 0, 1, 0, 1);
                        medTable.Attach (nameEntry, 1, 2, 0, 1);

                        // Location 
                        Label locationLabel = new Label (locationSS);
                        locationLabel.Xalign = 0.0f;
                        locationButton = new FileChooserButton (folderSS, FileChooserAction.SelectFolder);
                        locationButton.SetCurrentFolder (GetDefaultDir ());
                        
                        medTable.Attach (locationLabel, 0, 1, 1, 2);
                        medTable.Attach (locationButton, 1, 2, 1, 2);

                        // Format
                        Label formatLabel = new Label (formatSS);
                        formatLabel.Xalign = 0.0f;
                        formatsComboBox = new FormatsComboBox ();
                        formatsComboBox.InitializeDefaults ();

                        aspectFrame.Dimensions = formatsComboBox.ActiveTemplate.Dimensions;
                        aspectFrame.PixelAspect = formatsComboBox.ActiveTemplate.PixelAspect;
                        aspectFrame.Fps = formatsComboBox.ActiveTemplate.Fps;
                        
                        medTable.Attach (formatLabel, 0, 1, 2, 3);
                        medTable.Attach (formatsComboBox, 1, 2, 2, 3);
                        
                        // Buttons 
                        Button okButton = new Button (Stock.Ok);
                        okButton.Clicked += OnOkClicked;
                        Button backButton = new Button (Stock.GoBack);
                        backButton.Clicked += OnBackClicked;

                        // Button box
                        HButtonBox buttonBox = new HButtonBox ();
                        buttonBox.Add (backButton);
                        buttonBox.Add (okButton);
                                                
                        // Pack it
                        PackStart (aspectFrame, false, false, 0);
                        PackStart (medTable, false, false, 6);
                        PackEnd (buttonBox, false, false, 0);

                        // Delegates!
                        formatsComboBox.FormatChanged += FormatChanged;
                }
                
                // Private methods /////////////////////////////////////////////
                
                /* The desired format of the project was changed */
                void FormatChanged (object sender, FormatTemplate template)
                {
                        aspectFrame.Dimensions = template.Dimensions;
                        aspectFrame.PixelAspect = template.PixelAspect;
                        aspectFrame.Fps = template.Fps;
                }

                /* Ok was clicked. We need to start a new project */
                void OnOkClicked (object sender, EventArgs args)
                {
                        string projectName = StringFu.MakeSane (nameEntry.Text);
                        string directory = locationButton.CurrentFolder;
                        
                        // Wrong data or aborted
                        if (! ValidateInput (projectName, directory))
                                return;
                        
                        // Save our current path
                        Config.Projects.LastPath = locationButton.CurrentFolder;
                        
                        model.StartEditor (projectName, directory, formatsComboBox.ActiveTemplate.ToProjectFormat ());
                }
                
                void OnBackClicked (object sender, EventArgs args)
                {
                        model.SwitchToComponent (MenuComponent.Welcome);
                }

                string GetDefaultDir ()
                {
                        string ret = String.Empty;
                        try {
                                ret = Config.Projects.LastPath;
                                if (ret.StartsWith ("~")) {
                                        ret = ret.Remove (0, 1);
                                        ret = Environment.GetEnvironmentVariable ("HOME") + ret;

                                }
                                if (! Directory.Exists (ret))
                                        ret = Directory.GetCurrentDirectory ();
                        } catch {
                                ret = "/";
                        }

                        return ret;
                }
                
                /* Validate the data the used entered. That means project name
                 * and directory checks */
                bool ValidateInput (string name, string directory)
                {        
                        // Check the name first
                        if (! (StringFu.IsLetterOrDigitOrWhiteSpace (name)) || 
                            name == String.Empty) {
                                FastDialog.WarningOk (null, 
                                                      invalidNameHeaderSS, 
                                                      invalidNameSS);
                                return false;
                        }
                        
                        // Check directory
                        if (! (IOFu.DirPresentAndWritable (directory))) {
                                FastDialog.WarningOk (null, 
                                                      invalidDirectoryHeaderSS,
                                                      invalidDirectorySS);
                                return false;
                        }
                        
                        // Check if we're not overwriting anything
                        // FIXME: Use decent path building here!
                        if (System.IO.File.Exists (directory + "/" + name + ".div"))
                                if (FastDialog.WarningYesNo (null, 
                                                             overwriteHeaderSS, 
                                                             overwriteSS) != ResponseType.Yes)
                                        return false;
                        
                        return true;
                }
                
        }

}
