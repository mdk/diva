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

namespace Diva.Plugins.Theora {

        using Gtk;
        using System;
        using Mono.Unix;
        using Widgets;
        using Gdv;
        
        public sealed class Dialog : Gtk.Dialog {

                // Fields //////////////////////////////////////////////////////

                FileChooserWidget fileChooser = null;
                VideoFormatsComboBox videoFormatCombo = null;
                SampleRateComboBox audioRateCombo = null;
                HScale videoQualityScale = null;
                HScale audioQualityScale = null;
                ProjectFormat projectFormat = null;

                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("Export to Theora");
                
                readonly static string descriptionSS = Catalog.GetString
                        ("Ogg is a patent-free, fully open multimedia bitstream container" +
                         "format designed for efficient streaming and file compression.");
                
                readonly static string overwriteSS = Catalog.GetString
                        ("Are you sure that you want to overwrite that file?");
                
                readonly static string overwriteHeaderSS = Catalog.GetString
                        ("Overwrite file?");

                readonly static string exportFormatSS = Catalog.GetString
                        ("Export format");

                readonly static string audioRateSS = Catalog.GetString
                        ("Audio rate:");
                
                readonly static string videoFormatSS = Catalog.GetString
                        ("Video format:");

                readonly static string videoQualitySS = Catalog.GetString
                        ("Video quality:");

                readonly static string audioQualitySS = Catalog.GetString
                        ("Audio quality:");

                readonly static string filterSS = Catalog.GetString
                        ("Ogg files");

                // Statics /////////////////////////////////////////////////////

                readonly static FrameDimensions minFrame = new FrameDimensions
                        (32, 32);

                readonly static FrameDimensions maxFrame = new FrameDimensions
                        (2048, 2048);

                readonly static int stepWidth = 4;

                readonly static int stepHeight = 4;
                                
                // Properties //////////////////////////////////////////////////

                public Gdk.Pixbuf XiphLogo {
                        get { return new Gdk.Pixbuf (null, "xiph-logo.png"); }
                }

                public string FileName {
                        get { return fileChooser.Filename; }
                }

                public VideoFormat VideoFormat {
                        get { return videoFormatCombo.ActiveFormat; }
                }

                public AudioFormat AudioFormat {
                        get {
                                AudioFormat frmt = projectFormat.AudioFormat.Clone ();
                                frmt.SampleRate = audioRateCombo.ActiveValue;
                                return frmt;
                        }
                }
                
                public int VideoQuality {
                        get { return 20; }
                }

                public float AudioQuality {
                        get { return 0.3f; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public Dialog (Window parent, Gdv.ProjectFormat format) : base (titleSS, parent,
                                                      DialogFlags.Modal | DialogFlags.NoSeparator)
                {
                        // Border
                        BorderWidth = 6;
                        
                        // Xiph image logo
                        Image xiphImage = new Image (XiphLogo);

                        // Theora/ogg description
                        Label descriptionLabel = new Label (descriptionSS);
                        descriptionLabel.Wrap = true;
                        descriptionLabel.Xalign = 0.0f;

                        // Upper hbox
                        HBox logoBox = new HBox (false, 6);
                        logoBox.PackStart (xiphImage, false, false, 0);
                        logoBox.PackStart (descriptionLabel, true, true, 0);

                        // Filechooser
                        fileChooser = new FileChooserWidget (FileChooserAction.Save);
                        Gtk.FileFilter oggFilter = new FileFilter ();
                        oggFilter.AddPattern ("*.ogg");
                        oggFilter.Name = filterSS;
                        fileChooser.AddFilter (oggFilter);
                        fileChooser.ExtraWidget = CreateCustomWidgetPart (format);
                        
                        VBox.PackStart (logoBox, false, false, 6);
                        VBox.PackStart (fileChooser, true, true, 0);

                        // Buttons
                        Button okButton = new Button (Stock.Ok);
                        okButton.Clicked += OnOkButtonClicked;
                        AddButton (Stock.Cancel, ResponseType.Cancel);
                        ActionArea.Add (okButton);

                        projectFormat = format;

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

                Widget CreateCustomWidgetPart (Gdv.ProjectFormat format)
                {
                        // Video format
                        Label videoFormatLabel = new Label (videoFormatSS);
                        videoFormatLabel.Xalign = 0.0f;
                        videoFormatLabel.Yalign = 0.5f;
                        videoFormatCombo = new VideoFormatsComboBox (format.VideoFormat,
                                                                     minFrame, maxFrame,
                                                                     stepWidth, stepHeight);


                        // Audio rate
                        Label audioRateLabel = new Label (audioRateSS);
                        audioRateLabel.Xalign = 0.0f;
                        audioRateLabel.Yalign = 0.5f;
                        audioRateCombo = new SampleRateComboBox (format.AudioFormat);


                        // Video quality
                        Label videoQualityLabel = new Label (videoQualitySS);
                        videoQualityLabel.Xalign = 0.0f;
                        videoQualityLabel.Yalign = 0.5f;
                        videoQualityScale = new VideoQualityScale ();
                        videoQualityScale.Value = 20;

                        // Audio quality
                        Label audioQualityLabel = new Label (audioQualitySS);
                        audioQualityLabel.Xalign = 0.0f;
                        audioQualityLabel.Yalign = 0.5f;
                        audioQualityScale = new AudioQualityScale ();
                        audioQualityScale.Value = 0.3;
                        
                        // Table
                        Table alignTable = new Table (4, 2, false);
                        alignTable.ColumnSpacing = 12;
                        alignTable.RowSpacing = 6;
                        
                        alignTable.Attach (videoFormatLabel,
                                           0, 1, 0, 1,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (videoFormatCombo,
                                           1, 2, 0, 1,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);
                        
                        alignTable.Attach (audioRateLabel,
                                           0, 1, 1, 2,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (audioRateCombo,
                                           1, 2, 1, 2,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);

                        alignTable.Attach (videoQualityLabel,
                                           0, 1, 2, 3,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (videoQualityScale,
                                           1, 2, 2, 3,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);

                        alignTable.Attach (audioQualityLabel,
                                           0, 1, 3, 4,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (audioQualityScale,
                                           1, 2, 3, 4,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);

                        // Expander
                        Expander formatExpander = new Expander (exportFormatSS);
                        formatExpander.Child = alignTable;

                        return formatExpander;
                }
                
        }

}
