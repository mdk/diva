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
        using Gdv;
        using Mono.Unix;
        using System.Collections.Generic;

        public class VideoFormatsComboBox : ComboBox {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string fullSS = Catalog.GetString
                        ("Full resolution");

                readonly static string halfSS = Catalog.GetString
                        ("Half resolution");

                readonly static string quarterSS = Catalog.GetString
                        ("Quarter resolution");

                readonly static string customSS = Catalog.GetString
                        ("Custom");

                // Fields //////////////////////////////////////////////////////

                ListStore store;                                 // List store to handle the data
                Dictionary <TreeIter, VideoFormat> iterToFormat; // TreeIter => Format
                TreeIter iterCustom;
                VideoFormat customFormat = null;

                FrameDimensions minFrame;  // Minimal frame size
                FrameDimensions maxFrame;  // Maximum frame size
                int stepWidth;             // Step (increament) of width;
                int stepHeight;            // Step (increament) of height;

                // Properties //////////////////////////////////////////////////

                public VideoFormat ActiveFormat {
                        get { 
                                TreeIter iter;
                                GetActiveIter (out iter);

                                if (store.GetPath (iter).ToString () == (string) store.GetPath (iterCustom).ToString ())
                                        return customFormat;
                                else
                                        return iterToFormat [iter].Clone ();
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public VideoFormatsComboBox (VideoFormat format, FrameDimensions minF, FrameDimensions maxF,
                                             int stepW, int stepH) : base ()
                {
                        iterToFormat = new Dictionary <TreeIter, VideoFormat> ();
                        
                        // Create the store
                        store = new ListStore (typeof (string), typeof (string));
                        
                        // Set it
                        Model = store;

                        // Set the params for later
                        minFrame = minF;
                        maxFrame = maxF;
                        stepWidth = stepW;
                        stepHeight = stepH;

                        // Set the renderer
                        CellRendererText col1Renderer = new CellRendererText ();
                        PackStart (col1Renderer, false);
                        AddAttribute (col1Renderer, "text", 0);

                        CellRendererText col2Renderer = new CellRendererText ();
                        PackStart (col2Renderer, false);
                        AddAttribute (col2Renderer, "text", 1);

                        customFormat = format;
                        InitializeDefaults (format);
                }
                
                // Private methods /////////////////////////////////////////////
                
                /* Here we just fire our custom event */
                protected override void OnChanged ()
                {
                        TreeIter iter;
                        GetActiveIter (out iter);

                        if (store.GetPath (iter).ToString () == (string) store.GetPath (iterCustom).ToString ()) {
                                VideoFormat newFormat = (customFormat != null) ? customFormat : new VideoFormat ();
                                VideoFormatDialog dialog = new VideoFormatDialog (this, newFormat, minFrame, maxFrame,
                                                                                  stepWidth, stepHeight);
                                dialog.Run ();
                                UpdateCustomFormat (dialog.VideoFormat);
                                dialog.Destroy ();
                        }
                }

                /* Add new template with a given description/pixbuf */
                void AddFormat (string text, VideoFormat format)
                {
                        TreeIter iter = store.AppendValues (text, String.Format ("[{0}]", format.FrameDimensions));
                        iterToFormat [iter] = format;
                }

                void InitializeDefaults (VideoFormat format)
                {
                        // FIXME: Check decimation
                        // FIXME: Include the minimal setting
                        
                        // Full resolution
                        VideoFormat fullQualityFormat = format.Clone ();
                        AddFormat (fullSS, fullQualityFormat);

                        // Half resolution
                        VideoFormat halfQualityFormat = format.Clone ();
                        FrameDimensions halfDimensions = new FrameDimensions (format.FrameDimensions.Width / 2,
                                                                              format.FrameDimensions.Height / 2);
                        halfQualityFormat.FrameDimensions = halfDimensions;
                        AddFormat (halfSS, halfQualityFormat);

                        // Quarter resolution
                        VideoFormat quarterQualityFormat = format.Clone ();
                        FrameDimensions quarterDimensions = new FrameDimensions (format.FrameDimensions.Width / 4,
                                                                                 format.FrameDimensions.Height / 4);
                        quarterQualityFormat.FrameDimensions = quarterDimensions;
                        AddFormat (quarterSS, quarterQualityFormat);
                        
                        // Custom...
                        iterCustom = store.AppendValues (customSS);
                        
                        Active = 0;
                }

                void UpdateCustomFormat (VideoFormat format)
                {
                        customFormat = format;
                        store.SetValue (iterCustom, 1, String.Format ("[{0}]", format.FrameDimensions.ToString ()));
                }
                
        }

}