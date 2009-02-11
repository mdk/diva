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

namespace Diva.Plugins.Wav {

        using Gtk;
        using System;
        using Mono.Unix;
        using Widgets;
        
        public sealed class Dialog : Gtk.Dialog {

                // Fields //////////////////////////////////////////////////////

                FileChooserWidget fileChooser = null;
                AudioFormatsComboBox formatCombo = null;

                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("Export to Wav");
                
                readonly static string descriptionSS = Catalog.GetString
                        ("Exports the sound of the composition to a raw wave file.");
                
                readonly static string overwriteSS = Catalog.GetString
                        ("Are you sure that you want to overwrite that file?");
                
                readonly static string overwriteHeaderSS = Catalog.GetString
                        ("Overwrite file?");

                readonly static string filterSS = Catalog.GetString
                        ("Wave files");

                readonly static string exportFormatSS = Catalog.GetString
                        ("Export format");

                readonly static string audioFormatSS = Catalog.GetString
                        ("Audio format:");

                readonly static string projectFormatSS = Catalog.GetString
                        ("Project format");
                
                // Properties //////////////////////////////////////////////////

                public string FileName {
                        get { return fileChooser.Filename; }
                }
                
                public Gdv.AudioFormat AudioFormat {
                        get { return formatCombo.ActiveFormat; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public Dialog (Window parent, Gdv.ProjectFormat format) : base (titleSS, parent,
                                                                                DialogFlags.Modal | DialogFlags.NoSeparator)
                {
                        // Border
                        BorderWidth = 6;
                                                                        
                        // Wav description
                        Label descriptionLabel = new Label (descriptionSS);
                        descriptionLabel.Wrap = true;
                        descriptionLabel.Xalign = 0.0f;

                        // Format label
                        Label formatLabel = new Label (audioFormatSS);
                        
                        // Audio formats combo
                        formatCombo = new AudioFormatsComboBox (projectFormatSS, format.AudioFormat);

                        // Format hbox
                        HBox formatHBox = new HBox (false, 12);
                        formatHBox.PackStart (formatLabel, false, false, 0);
                        formatHBox.PackEnd (formatCombo, true, true, 0);

                        // Expander
                        Expander formatExpander = new Expander (exportFormatSS);
                        formatExpander.Child = formatHBox;
                        
                        // Filechooser
                        fileChooser = new FileChooserWidget (FileChooserAction.Save);
                        Gtk.FileFilter wavFilter = new FileFilter ();
                        wavFilter.AddPattern ("*.wav"); 
                        wavFilter.Name = filterSS;
                        fileChooser.AddFilter (wavFilter);
                        fileChooser.ExtraWidget = formatExpander;
                                                
                        VBox.PackStart (descriptionLabel, false, false, 0);
                        VBox.PackStart (fileChooser, true, true, 12);

                        // Buttons
                        Button okButton = new Button (Stock.Ok);
                        okButton.Clicked += OnOkButtonClicked;
                        AddButton (Stock.Cancel, ResponseType.Cancel);
                        ActionArea.Add (okButton);

                        ShowAll ();
                }

                // Private methods /////////////////////////////////////////////

                void OnOkButtonClicked (object sender, EventArgs args)
                {
                        if (ValidateInput ())
                                Respond (ResponseType.Ok);
                        else 
                                return;
                }

                bool ValidateInput ()
                {
                        string name = fileChooser.Filename;
                        
                        // Empty name
                        if (name == String.Empty || name == null)
                                return false;
                        
                        // Overwrite
                        if (System.IO.File.Exists (name))
                            if (FastDialog.WarningYesNo (null, 
                                                         overwriteHeaderSS, 
                                                         overwriteSS) != ResponseType.Yes)
                                    return false;
                        
                        return true;
                }

        }

}
