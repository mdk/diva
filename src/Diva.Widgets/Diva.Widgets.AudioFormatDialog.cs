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

namespace Diva.Widgets {

        using System;
        using Gtk;
        using Mono.Unix;
        using Util;
        using Gdv;

        public sealed class AudioFormatDialog : Dialog {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string sampleRateSS = Catalog.GetString
                        ("Samplerate:");
                
                readonly static string depthSS = Catalog.GetString
                        ("Depth:");

                readonly static string channelsSS = Catalog.GetString
                        ("Channels:");

                readonly static string titleSS = Catalog.GetString
                        ("Audio format");

                // Fields //////////////////////////////////////////////////////

                SampleRateComboBox sampleRateCombo = null;
                DepthComboBox depthCombo = null;
                ChannelsComboBox channelsCombo = null;

                // Properties //////////////////////////////////////////////////

                public AudioFormat AudioFormat {
                        get {
                                AudioFormat format = new AudioFormat ();
                                format.SampleRate = sampleRateCombo.ActiveSampleRate;
                                format.Depth = depthCombo.ActiveDepth;
                                format.Channels = channelsCombo.ActiveChannels;
                                
                                return format;
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public AudioFormatDialog (Widget source, AudioFormat format) :
                base  (titleSS, GtkFu.GetParentForWidget (source), DialogFlags.Modal)
                {
                        HasSeparator = false;
                        Resizable = false;
                        
                        BorderWidth = 6;
                        WidthRequest = 320;

                        HBox hBox = new HBox (false, 12);
                        VBox leftVBox = new VBox (false, 6);
                        VBox rightVBox = new VBox (false, 12);

                        // Image
                        Image iconImage = new Image (IconFu.GetStockIcon ("audio-card", IconSize.Dialog));
                        
                        // Samplerate
                        sampleRateCombo = new SampleRateComboBox (format);
                        Label sampleRateLabel = new Label (sampleRateSS);
                        sampleRateLabel.Xalign = 0.0f;

                        // Depth combo
                        depthCombo = new DepthComboBox (format);
                        Label depthLabel = new Label (depthSS);
                        depthLabel.Xalign = 0.0f;

                        // CHannels combo
                        channelsCombo = new ChannelsComboBox (format);
                        Label channelsLabel = new Label (channelsSS);
                        channelsLabel.Xalign = 0.0f;

                        // Table
                        Table alignTable = new Table (3, 2, false);
                        alignTable.ColumnSpacing = 12;
                        alignTable.RowSpacing = 6;
                        
                        alignTable.Attach (sampleRateLabel,
                                           0, 1, 0, 1,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (sampleRateCombo,
                                           1, 2, 0, 1,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);
                        
                        alignTable.Attach (depthLabel,
                                           0, 1, 1, 2,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (depthCombo,
                                           1, 2, 1, 2,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);

                        alignTable.Attach (channelsLabel,
                                           0, 1, 2, 3,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (channelsCombo,
                                           1, 2, 2, 3,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);

                        leftVBox.PackStart (iconImage, false, false, 0);
                        rightVBox.PackStart (alignTable, false, false, 0);
                        
                        hBox.PackStart (leftVBox, false, false, 0);
                        hBox.PackStart (rightVBox, true, true, 0);
                        VBox.PackStart (hBox, false, false, 0);
                        VBox.Spacing = 12;
                                                
                        AddButton (Stock.Ok, (int) ResponseType.Ok);
                        
                        ShowAll ();
                }

        }

}