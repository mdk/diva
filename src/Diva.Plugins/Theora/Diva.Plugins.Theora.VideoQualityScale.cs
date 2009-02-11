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

        using System;
        using Gtk;
        using Mono.Unix;

        public class VideoQualityScale : HScale {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string veryPoorSS = Catalog.GetString 
                        ("Very poor");
                
                readonly static string poorSS = Catalog.GetString 
                        ("Poor");
                
                readonly static string averageSS = Catalog.GetString 
                        ("Average");

                readonly static string goodSS = Catalog.GetString 
                        ("Good");

                readonly static string veryGoodSS = Catalog.GetString 
                        ("Very good");

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public VideoQualityScale () : base (0, 64, 1)
                {
                }

                /* WTF is that? A bug in gtk-sharp? */
                protected VideoQualityScale (IntPtr ptr) : base (ptr)
                {
                }

                protected override string OnFormatValue (double val)
                {
                        if (val <= 10)
                                return veryPoorSS;
                        else if (val <= 15)
                                return poorSS;
                        else if (val <= 24)
                                return averageSS;
                        else if (val <= 35)
                                return goodSS;
                        else
                                return veryGoodSS;
                }

        }

}