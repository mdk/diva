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

/* Test cases for functions that convert Time to string (SMPTE) */

namespace Diva.Util.NUnit {

        using global::NUnit.Framework;
        using Util;
        using Gdv;
        using System;

        [TestFixture]
        public class ToSMPTETests {

                [Test]
                public void One ()
                {
                        string oneMinTwoSecs = TimeFu.ToSMPTE (Time.FromSeconds (60) +
                                                               Time.FromSeconds (2),
                                                               new Fraction (25, 1),
                                                               true);
                        Assert.AreEqual ("00:01:02:00", oneMinTwoSecs);
                }

                [Test]
                public void Two ()
                {
                        string oneHourTwoMinThreeSecs = TimeFu.ToSMPTE (Time.FromSeconds (3600) +
                                                                        Time.FromSeconds (120) +
                                                                        Time.FromSeconds (3),
                                                                        new Fraction (25, 1),
                                                                        true);
                        Assert.AreEqual ("01:02:03:00", oneHourTwoMinThreeSecs);
                }

                [Test]
                public void Three ()
                {
                        string oneHourTwoMinThreeSecsFourFrames = TimeFu.ToSMPTE (Time.FromSeconds (3600) +
                                                                                  Time.FromSeconds (120) +
                                                                                  Time.FromSeconds (3) +
                                                                                  (new Fraction (25, 1)).FpsFrameDuration () * 4,
                                                                                  new Fraction (25, 1),
                                                                                  true);
                        Assert.AreEqual ("01:02:03:04", oneHourTwoMinThreeSecsFourFrames);
                }

                [Test]
                public void Four ()
                {
                        string zero = TimeFu.ToSMPTE (Time.FromSeconds (0), 
                                                      new Fraction (25, 1),
                                                      true);
                        Assert.AreEqual ("00:00:00:00", zero);
                }

        }

}