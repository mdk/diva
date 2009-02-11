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
        using System;
        using Util;
        using Mono.Unix;

        public class AboutDialog : Gtk.AboutDialog {

                public readonly static string[] authors = {
                        "Michael Dominic K.", 
                        "Senko Rasic",
                        "Travis Hansen",
                        "Mario Danic" }; 

                public readonly static string website =
                        "http://www.diva-project.org";

                public readonly static string copyright =
                        "Copyright 2005-2006 Michael Dominic K.";

                public readonly static string[] artists = {
                        "Jakub Steiner", 
                        "Andreas Nilsson" };

                public readonly static string license =
                        "Permission is hereby granted, free of charge, to any person obtaining a " +
                        "copy of this software and associated documentation files (the \"Software\"), " +
                        "to deal in the Software without restriction, including without limitation " +
                        "the rights to use, copy, modify, merge, publish, distribute, sublicense, " +
                        "and/or sell copies of the Software, and to permit persons to whom the " +
                        "Software is furnished to do so, subject to the following conditions:" +
                        "\n\n" +
                        "The above copyright notice and this permission notice shall be included" +
                        "in all copies or substantial portions of the Software." +
                        "\n\n" + 
                        "THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND, EXPRESS" +
                        "OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF" + 
                        "MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN" +
                        "NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM," +
                        "DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR" +
                        "OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE" +
                        "USE OR OTHER DEALINGS IN THE SOFTWARE.";

                readonly static string translatorSS = Catalog.GetString
                        ("TRANSLATOR'S NAME GOES HERE (ie. John Doe)");

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public AboutDialog () : base ()
                {
                        Artists = artists;
                        Authors = authors;
                        Comments = VersionFu.GetCallingSlugline ();
                        Copyright = copyright;
                        Version = VersionFu.GetCallingVersion ();
                        Name = "Diva";
                        Website = website;
                        WebsiteLabel  = website;
                        License = license;
                        WrapLicense = true;
                        Logo = Util.IconFu.MainDivaIcon96;

                        // Add translator credits, but only if filled
                        if (translatorSS != "TRANSLATOR'S NAME GOES HERE (ie. John Doe)")
                                TranslatorCredits = translatorSS;
                }

        }

}