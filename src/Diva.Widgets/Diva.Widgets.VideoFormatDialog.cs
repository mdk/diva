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

        public sealed class VideoFormatDialog : Dialog {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string widthSS = Catalog.GetString
                        ("Width:");
                
                readonly static string heightSS = Catalog.GetString
                        ("Height:");

                readonly static string aspectSS = Catalog.GetString
                        ("Aspect ratio:");

                readonly static string titleSS = Catalog.GetString
                        ("Video format");

                // Fields //////////////////////////////////////////////////////

                SpinButton widthSpin = null;
                SpinButton heightSpin = null;
                AspectFrame aspectFrame = null;

                FrameDimensions minFrame;  // Minimal frame size
                FrameDimensions maxFrame;  // Maximum frame size
                int stepWidth;             // Step (increament) of width;
                int stepHeight;            // Step (increament) of height;

                VideoFormat orginalFormat; // Original video format

                // Properties //////////////////////////////////////////////////

                public FrameDimensions Dimensions {
                        get {
                                return new FrameDimensions ((int) widthSpin.Value,
                                                            (int) heightSpin.Value);
                        }
                }
                
                public VideoFormat VideoFormat {
                        get {
                                VideoFormat videoFormat = orginalFormat.Clone ();
                                videoFormat.FrameDimensions = Dimensions;
                                return videoFormat;
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public VideoFormatDialog (Widget source, VideoFormat format, FrameDimensions minF, FrameDimensions maxF,
                                          int stepW, int stepH) :
                base  (titleSS, GtkFu.GetParentForWidget (source), DialogFlags.Modal)
                {
                        HasSeparator = false;
                        Resizable = false;
                        
                        BorderWidth = 6;

                        // Set
                        minFrame = minF;
                        maxFrame = maxF;
                        stepWidth = stepW;
                        stepHeight = stepH;
                        orginalFormat = format;

                        HBox hBox = new HBox (false, 12);
                        VBox leftVBox = new VBox (false, 6);
                        VBox rightVBox = new VBox (false, 12);
                        
                        // Aspect frame
                        aspectFrame = new AspectFrame ();
                        aspectFrame.Dimensions = format.FrameDimensions ;
                        aspectFrame.Fps = format.Fps;
                        aspectFrame.PixelAspect = format.PixelAspect;
                        aspectFrame.WidthRequest = 200;
                        
                        // Width
                        widthSpin = new SpinButton ((double) minF.Width, (double) maxF.Width, stepW);
                        widthSpin.Value = format.FrameDimensions.Width;
                        Label widthLabel = new Label (widthSS);
                        widthLabel.Xalign = 0.0f;

                        // Height
                        heightSpin = new SpinButton ((double) minF.Height, (double) maxF.Height, stepH);
                        heightSpin.Value = format.FrameDimensions.Height;
                        Label heightLabel = new Label (heightSS);
                        heightLabel.Xalign = 0.0f;

                        Roundify ();

                        // Bind
                        heightSpin.ValueChanged += OnHeightValueChanged;
                        widthSpin.ValueChanged += OnWidthValueChanged;

                        // Table
                        Table alignTable = new Table (2, 2, false);
                        alignTable.ColumnSpacing = 12;
                        alignTable.RowSpacing = 6;
                        
                        alignTable.Attach (widthLabel,
                                           0, 1, 0, 1,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (widthSpin,
                                           1, 2, 0, 1,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);
                        
                        alignTable.Attach (heightLabel,
                                           0, 1, 1, 2,
                                           AttachOptions.Fill, AttachOptions.Expand, 0, 0);
                        
                        alignTable.Attach (heightSpin,
                                           1, 2, 1, 2,
                                           AttachOptions.Expand | AttachOptions.Fill, AttachOptions.Fill, 0, 0);

                        leftVBox.PackStart (aspectFrame, false, false, 0);
                        rightVBox.PackStart (alignTable, false, false, 0);
                        
                        hBox.PackStart (leftVBox, false, false, 0);
                        hBox.PackStart (rightVBox, true, true, 0);
                        VBox.PackStart (hBox, false, false, 0);
                        VBox.Spacing = 12;
                                                
                        AddButton (Stock.Ok, (int) ResponseType.Ok);
                        
                        ShowAll ();
                }

                // Private methods /////////////////////////////////////////////

                void OnWidthValueChanged (object o, EventArgs args)
                {
                        UpdateFrame ();
                }

                void OnHeightValueChanged (object o, EventArgs args)
                {
                        UpdateFrame ();
                }

                void UpdateFrame ()
                {
                        FrameDimensions dimensions = new FrameDimensions ((int) widthSpin.Value,
                                                                          (int) heightSpin.Value);
                        aspectFrame.Dimensions = dimensions;
                }

                void Roundify ()
                {
                        // Check min/max size
                        double width = Math.Min (widthSpin.Value, (double) maxFrame.Width);
                        width = Math.Max (width, (double) minFrame.Width);

                        // Check min/max size
                        double height = Math.Min (heightSpin.Value, (double) maxFrame.Height);
                        height = Math.Max (height, (double) minFrame.Height);

                        // Roundify
                        width = ((int) (width / (double) stepWidth)) * stepWidth;
                        height = ((int) (height / (double) stepHeight)) * stepHeight;

                        // Reassign
                        widthSpin.Value = width;
                        heightSpin.Value = height;
                }

        }

}