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
        using Mono.Unix;
        using Gtk;
        using Util;

        /* This structure is used to represent a format template for a new project */
        public struct FormatTemplate {
                
                public Gdv.FrameDimensions Dimensions;
                public Gdv.Fraction PixelAspect;
                public Gdv.Fraction Fps;

                public Gdv.ProjectFormat ToProjectFormat ()
                {
                        Gdv.ProjectFormat prj = new Gdv.ProjectFormat ();
                        prj.VideoFormat.FrameDimensions = Dimensions;
                        prj.VideoFormat.Fps = Fps;
                        prj.VideoFormat.PixelAspect = PixelAspect;
                        prj.AudioFormat.SampleRate = 48000;

                        return prj;
                }
                        
        }
        
        public class FormatsComboBox : ComboBox {

                // Events and delegates ////////////////////////////////////////

                public delegate void FormatTemplateHandler (object sender, FormatTemplate format);

                public event FormatTemplateHandler FormatChanged;

                // Translatable ////////////////////////////////////////////////
                
                readonly static string fullPalSS = Catalog.GetString
                        ("Full PAL video");

                readonly static string halfPalSS = Catalog.GetString
                        ("Half PAL video");

                readonly static string fullNtscSS = Catalog.GetString
                        ("Full NTSC video");

                readonly static string halfNtscSS = Catalog.GetString
                        ("Half NTSC video");

                readonly static string fullPalWideSS = Catalog.GetString
                        ("Full PAL widescreen video");

                readonly static string fullNtscWideSS = Catalog.GetString
                        ("Full NTSC widescreen video");

                readonly static string lowAnimSS = Catalog.GetString
                        ("Low resolution animation");

                readonly static string quicktimeSS = Catalog.GetString
                        ("Quicktime camera format");

                // Fields //////////////////////////////////////////////////////

                ListStore store; // List store to handle the data

                // Properties //////////////////////////////////////////////////

                public FormatTemplate ActiveTemplate {
                        get { 
                                TreeIter iter;
                                GetActiveIter (out iter);
                                return (FormatTemplate) store.GetValue (iter, 1);
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public FormatsComboBox () : base ()
                {
                        // Create the store
                        store = new ListStore (typeof (string),
                                               typeof (FormatTemplate));

                        // Set it
                        Model = store;

                        // Set the renderer
                        CellRendererText col1Renderer = new CellRendererText ();
                        PackStart (col1Renderer, false);
                        AddAttribute (col1Renderer, "text", 0);
                }

                /* Add new template with a given description/pixbuf */
                public void AddFormat (string text, FormatTemplate format)
                {
                        store.AppendValues (text, format);
                }

                /* Fill the combobox with default, stock formats. Add additional format here! */
                public void InitializeDefaults ()
                {
                        // FIXME: In future we should move all that into some extra file
                        // schemas perhaps? Sounds like a good use case
                        
                        // Full pal 720x576
                        FormatTemplate fullPalFormat;
                        fullPalFormat.Dimensions = new Gdv.FrameDimensions (720, 576);
                        fullPalFormat.PixelAspect = new Gdv.Fraction (59, 54);
                        fullPalFormat.Fps = new Gdv.Fraction (25, 1);
                        AddFormat (fullPalSS, fullPalFormat);

                        // Half pal 360x288
                        FormatTemplate halfPalFormat;
                        halfPalFormat.Dimensions = new Gdv.FrameDimensions (360, 288);
                        halfPalFormat.PixelAspect = new Gdv.Fraction (59, 54);
                        halfPalFormat.Fps = new Gdv.Fraction (25, 1);
                        AddFormat (halfPalSS, halfPalFormat);

                        // Full NTSC 720x480
                        FormatTemplate fullNtscFormat;
                        fullNtscFormat.Dimensions = new Gdv.FrameDimensions (720, 480);
                        fullNtscFormat.PixelAspect = new Gdv.Fraction (10, 11);
                        fullNtscFormat.Fps = new Gdv.Fraction (30000, 1001);
                        AddFormat (fullNtscSS, fullNtscFormat);

                        // Half NTSC 360x240
                        FormatTemplate halfNtscFormat;
                        halfNtscFormat.Dimensions = new Gdv.FrameDimensions (360, 240);
                        halfNtscFormat.PixelAspect = new Gdv.Fraction (10, 11);
                        halfNtscFormat.Fps = new Gdv.Fraction (30000, 1001);
                        AddFormat (halfNtscSS, halfNtscFormat);
                        
                        // Full pal widescreen 720x576
                        FormatTemplate fullPalWideFormat;
                        fullPalWideFormat.Dimensions = new Gdv.FrameDimensions (720, 576);
                        fullPalWideFormat.PixelAspect = new Gdv.Fraction (142, 100);
                        fullPalWideFormat.Fps = new Gdv.Fraction (25, 1);
                        AddFormat (fullPalWideSS, fullPalWideFormat);

                        // Full ntsc widescreen 720x480
                        FormatTemplate fullNtscWideFormat;
                        fullNtscWideFormat.Dimensions = new Gdv.FrameDimensions (720, 480);
                        fullNtscWideFormat.PixelAspect = new Gdv.Fraction (120, 100);
                        fullNtscWideFormat.Fps = new Gdv.Fraction (30000, 1001);
                        AddFormat (fullNtscWideSS, fullNtscWideFormat);

                        // Low animation 320x240
                        FormatTemplate lowAnimFormat;
                        lowAnimFormat.Dimensions = new Gdv.FrameDimensions (320, 240);
                        lowAnimFormat.PixelAspect = new Gdv.Fraction (1, 1);
                        lowAnimFormat.Fps = new Gdv.Fraction (15, 1);
                        AddFormat (lowAnimSS, lowAnimFormat);

                        // Quicktime
                        FormatTemplate quicktimeFormat;
                        quicktimeFormat.Dimensions = new Gdv.FrameDimensions (640, 480);
                        quicktimeFormat.PixelAspect = new Gdv.Fraction (1, 1);
                        quicktimeFormat.Fps = new Gdv.Fraction (30, 1);
                        AddFormat (quicktimeSS, quicktimeFormat);

                        // Full ntsc wide pixelaspect = 120/100

                        Active = 0;
                }

                // Public methods //////////////////////////////////////////////

                /* Here we just fire our custom event */
                protected override void OnChanged ()
                {
                        TreeIter iter;
                        GetActiveIter (out iter);
                        FormatTemplate template = (FormatTemplate) store.GetValue (iter, 1);
                        if (FormatChanged != null)
                                FormatChanged (this, template);
                }

                

        }

}
