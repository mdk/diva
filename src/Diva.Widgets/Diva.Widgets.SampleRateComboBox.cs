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

        public class SampleRateComboBox : IntyComboBox {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string nineSixSS = Catalog.GetString
                        ("96 khz");

                readonly static string fourEightSS = Catalog.GetString
                        ("48 khz");

                readonly static string fourFourSS = Catalog.GetString
                        ("44.1 khz");

                readonly static string twoTwoSS = Catalog.GetString
                        ("22 khz");

                readonly static string oneOneSS = Catalog.GetString
                        ("11 khz");

                readonly static string eightSS = Catalog.GetString
                        ("8 khz");
                
                // Properties //////////////////////////////////////////////////

                public int ActiveSampleRate {
                        get { return ActiveValue; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public SampleRateComboBox (AudioFormat format) : base (format.SampleRate)
                {
                }
                
                // Private methods /////////////////////////////////////////////
                
                protected override void InitializeDefaults ()
                {
                        AddInt (nineSixSS, 96000); // 96khz
                        AddInt (fourEightSS, 48000); // 48khz
                        AddInt (fourFourSS, 44100); // 44.1khz
                        AddInt (twoTwoSS, 22000); // 22khz
                        AddInt (oneOneSS, 11000); // 11khz
                        AddInt (eightSS, 8000); // 8khz
                }

        }

}