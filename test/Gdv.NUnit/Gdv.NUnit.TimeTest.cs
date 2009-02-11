////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIT X11 license, Copyright (c) 2005-2006 by:                               //
//                                                                            //
// Authors:                                                                   //
//      Michael Dominic K. <michaldominik@gmail.com>                          //
//      Senko Rasic <senko@senko.net>                                         //
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

/* Test cases for Time */

namespace Gdv.NUnit {

        using global::NUnit.Framework;
        using Gdv;

        [TestFixture]
        public class TimeTest {

                [Test]
                public void Operators ()
                {
                        Time OneSec = Time.FromSeconds (1);
                        Time TwoSec = Time.FromSeconds (2);
                        Time OneSecPlus = Time.FromSeconds (1) + 1;
                        Time ThreeSec = Time.FromSeconds (3);
                        
                        Assert.AreEqual (OneSec + TwoSec, ThreeSec);
                        Assert.AreEqual (ThreeSec - OneSec, TwoSec);
                        Assert.AreEqual (ThreeSec - TwoSec, OneSec);
                        
                        if (OneSec >= TwoSec)
                                Assert.Fail ("One second is more than two seconds?!");
                        
                        if (OneSecPlus <= OneSec)
                                Assert.Fail ("One second + nanosecond == one second?!");
                        
                        if (OneSec != OneSec)
                                Assert.Fail ("One second is different than one second?!");
                        
                        Time OneSomething = Time.FromSeconds (1.34);
                        Time TwoMinusSomething = Time.FromSeconds (1.76);
                        
                        Assert.AreEqual (Time.FromSeconds (1), OneSomething.Round (Time.OneSecond));
                        Assert.AreEqual (Time.FromSeconds (2), TwoMinusSomething.Round (Time.OneSecond));
                        
                        if (Time.FromNSeconds(42).NSeconds != 42)
                                Assert.Fail ("Something's wrong with counting nanoseconds?!");
                        
                        Assert.AreEqual (OneSec, TwoSec / 2);
                        Assert.AreEqual (OneSec, ThreeSec / 3);
                        Assert.AreEqual (TwoSec, OneSec * 2);

                        if (!TwoSec.InSpan(OneSec, ThreeSec))
                                Assert.Fail ("Two seconds is not between one and three seconds?!");
                        
                }
        
                [Test]
                public void RelativeAbsolute ()
                {
                        Time t1 = Time.FromSeconds (3);
                        Time t2 = Time.FromSeconds (1);
                        
                        Assert.AreEqual (Time.FromSeconds (4), t1.RelativeTo (t2));
                        Assert.AreEqual (Time.FromSeconds (2), t1.AbsoluteTo (t2));
                }
                
        }

}
