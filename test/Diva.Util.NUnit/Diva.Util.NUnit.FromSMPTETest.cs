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

/* Test cases for functions that convert string (SMPTE) to Time */

namespace Diva.Util.NUnit {

        using global::NUnit.Framework;
        using Util;
        using Gdv;
        using System;

        [TestFixture]
        public class FromSMPTETests {

                [Test]
                public void CompletitionOne ()
                {
                        Time oneMinTwoSecs = TimeFu.FromSMPTE ("01:02", new Fraction (25, 1));
                        Assert.AreEqual (Time.FromSeconds (60) +
                                         Time.FromSeconds (2),
                                         oneMinTwoSecs);
                }
                
                [Test]
                public void CompletitionTwo ()
                {
                        Time oneHourTwoMinThreeSecs = TimeFu.FromSMPTE ("01:02:03", new Fraction (25, 1));
                        Assert.AreEqual (Time.FromSeconds (3600) +
                                         Time.FromSeconds (120) +
                                         Time.FromSeconds (3),
                                         oneHourTwoMinThreeSecs);
                }

                [Test]
                public void CompletitionThree ()
                {
                        Time oneHourTwoMinThreeSecsFourFrames = TimeFu.FromSMPTE ("01:02:03:04", new Fraction (25, 1));
                        Assert.AreEqual (Time.FromSeconds (3600) +
                                         Time.FromSeconds (120) +
                                         Time.FromSeconds (3) +
                                         (new Fraction (25, 1)).FpsFrameDuration () * 4,
                                         oneHourTwoMinThreeSecsFourFrames);
                }

                [Test]
                public void CompletitionFour ()
                {
                        Time oneSec = TimeFu.FromSMPTE ("01", new Fraction (25, 1));
                        Assert.AreEqual (Time.FromSeconds (1), 
                                         oneSec);
                }

                [Test]
                [ExpectedException (typeof (FormatException))]
                public void BorkedStringOne ()
                {
                        string borkOne = "00::01";
                        TimeFu.FromSMPTE (borkOne, new Fraction (25, 1));
                }

                [Test]
                [ExpectedException (typeof (FormatException))]
                public void BorkedStringTwo ()
                {
                        string borkTwo = "01:002";
                        TimeFu.FromSMPTE (borkTwo, new Fraction (25, 1));
                }

                [Test]
                [ExpectedException (typeof (FormatException))]
                public void BorkedStringThree ()
                {
                        string borkThree = "bork bork bork";
                        TimeFu.FromSMPTE (borkThree, new Fraction (25, 1));
                }

        }

}
