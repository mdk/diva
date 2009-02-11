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

        public class AudioFormatsComboBox : ComboBox {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string cdQualityFormatSS = Catalog.GetString
                        ("CD quality");

                readonly static string radioQualityFormatSS = Catalog.GetString
                        ("Radio quality");

                readonly static string phoneQualityFormatSS = Catalog.GetString
                        ("Telephone quality");

                readonly static string customFormatSS = Catalog.GetString
                        ("Custom");

                // Fields //////////////////////////////////////////////////////

                ListStore store;                                 // List store to handle the data
                Dictionary <TreeIter, AudioFormat> iterToFormat; // TreeIter => Format
                TreeIter iterCustom;
                AudioFormat customFormat = null;

                // Properties //////////////////////////////////////////////////

                public AudioFormat ActiveFormat {
                        get { 
                                TreeIter iter;
                                GetActiveIter (out iter);

                                // FIXME: Add cloning for fuck's sake

                                if (store.GetPath (iter).ToString () == (string) store.GetPath (iterCustom).ToString ())
                                        return customFormat;
                                else
                                        return iterToFormat [iter];
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public AudioFormatsComboBox (string text, AudioFormat format) : base ()
                {
                        iterToFormat = new Dictionary <TreeIter, AudioFormat> ();
                        
                        // Create the store
                        store = new ListStore (typeof (string), typeof (string));
                        
                        // Set it
                        Model = store;

                        // Set the renderer
                        CellRendererText col1Renderer = new CellRendererText ();
                        PackStart (col1Renderer, false);
                        AddAttribute (col1Renderer, "text", 0);

                        CellRendererText col2Renderer = new CellRendererText ();
                        PackStart (col2Renderer, false);
                        AddAttribute (col2Renderer, "text", 1);

                        AddFormat (text, format);
                        InitializeDefaults ();
                }
                
                // Private methods /////////////////////////////////////////////
                
                /* Here we just fire our custom event */
                protected override void OnChanged ()
                {
                        TreeIter iter;
                        GetActiveIter (out iter);

                        if (store.GetPath (iter).ToString () == (string) store.GetPath (iterCustom).ToString ()) {
                                AudioFormat newFormat = (customFormat != null) ? customFormat : new AudioFormat ();
                                AudioFormatDialog dialog = new AudioFormatDialog (this, newFormat);
                                dialog.Run ();
                                UpdateCustomFormat (dialog.AudioFormat);
                                dialog.Destroy ();
                        }
                }

                /* Add new template with a given description/pixbuf */
                void AddFormat (string text, AudioFormat format)
                {
                        TreeIter iter = store.AppendValues (text, String.Format ("[{0}]", format));
                        iterToFormat [iter] = format;
                }

                void InitializeDefaults ()
                {
                        // Cd quality
                        AudioFormat cdQualityFormat = new AudioFormat ();
                        cdQualityFormat.SampleRate = 44100;
                        cdQualityFormat.Depth = 16;
                        cdQualityFormat.Channels = 2;
                        AddFormat (cdQualityFormatSS, cdQualityFormat);

                        // Radio quality
                        AudioFormat radioQualityFormat = new AudioFormat ();
                        radioQualityFormat.SampleRate = 22000;
                        radioQualityFormat.Depth = 16;
                        radioQualityFormat.Channels = 2;
                        AddFormat (radioQualityFormatSS, radioQualityFormat);

                        // Phone quality
                        AudioFormat phoneQualityFormat = new AudioFormat ();
                        phoneQualityFormat.SampleRate = 8000;
                        phoneQualityFormat.Depth = 8;
                        phoneQualityFormat.Channels = 1;
                        AddFormat (phoneQualityFormatSS, phoneQualityFormat);

                        // Custom...
                        iterCustom = store.AppendValues (customFormatSS);
                        
                        Active = 0;
                }

                void UpdateCustomFormat (AudioFormat format)
                {
                        customFormat = format;
                        store.SetValue (iterCustom, 1, String.Format ("[{0}]", format.ToString ()));
                }
                
        }

}