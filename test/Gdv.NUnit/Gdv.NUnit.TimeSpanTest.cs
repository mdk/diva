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

/* Test cases for Gdv.Timespan */

namespace Gdv.NUnit {

        using global::NUnit.Framework;
        using Gdv;
        
        [TestFixture]
        public class TimeSpanTest {
                
                [Test]
                public void Basics () 
                {
                        TimeSpan ZeroNearlyOne = new TimeSpan (Time.FromSeconds (0), 
                                                               Time.FromSeconds (1) - 1);
                        
                        TimeSpan OneTwo = new TimeSpan (Time.FromSeconds (1), 
                                                        Time.FromSeconds (2));
                        
                        TimeSpan TwoFour = new TimeSpan (Time.FromSeconds (2), 
                                                         Time.FromSeconds (4));
                        
                        TimeSpan OneThree = new TimeSpan (Time.FromSeconds (1), 
                                                          Time.FromSeconds (3)); 
                        
                        if (OneTwo.IntersectsWith (TwoFour))
                                Assert.Fail ("1-2 doesn't overlap with 2-4 !");
                        
                        if (! OneThree.IntersectsWith (OneTwo))
                                Assert.Fail ("1-3 should overlap with 1-2 !");
                        if (! OneTwo.IntersectsWith (OneThree))
                                Assert.Fail ("1-2 should overlap with 1-3 !");
                        
                        if (! TwoFour.IntersectsWith (OneThree))
                                Assert.Fail ("2-4 should overlap with 1-3 !");
                        
                        if (! OneThree.IntersectsWith (TwoFour))
                                Assert.Fail ("1-3 should overlap with 2-4 !");
                        
                        if (ZeroNearlyOne.IntersectsWith (OneThree))
                                Assert.Fail ("0-0.99 doesn't overlap with 1-3 !");
                        
                        if (ZeroNearlyOne.IntersectsWith (TwoFour))
                                Assert.Fail ("0-0.99 doesn't overlap with 2-4 !");
                }
                
        
                [Test]
                public void Contains () 
                {       
                        TimeSpan OneFive = new TimeSpan (Time.FromSeconds (1), 
                                                         Time.FromSeconds (5));
                        
                        Time Zero = Time.FromSeconds (0);
                        Time One = Time.FromSeconds (1); 
                        Time Two = Time.FromSeconds (2); 
                        Time Six = Time.FromSeconds (6); 
                        
                        if (OneFive.Contains (Zero))
                                Assert.Fail ("1-5 doesn't contain 0!");
                        
                        if (! OneFive.Contains (One))
                                Assert.Fail ("1-5 contains 1!");
                        
                        if (! OneFive.Contains (Two))
                                Assert.Fail ("1-5 contains 2!");
                        
                        if (OneFive.Contains (Six))
                                Assert.Fail ("1-5 doesn't contain 6!");
                        
                }
                
                [Test]
                public void Intersections () 
                {       
                        TimeSpan OneFive = new TimeSpan (Time.FromSeconds (1), 
                                                         Time.FromSeconds (5));
                        
                        TimeSpan TwoSix = new TimeSpan (Time.FromSeconds (2),
                                                        Time.FromSeconds (6));
                        
                        TimeSpan ZeroSeven = new TimeSpan (Time.FromSeconds (0),
                                                           Time.FromSeconds (7));
                        
                        if (! OneFive.IntersectsWith (TwoSix))
                                Assert.Fail ("1-5 should intersect with 2-6!");
                        
                        if (! TwoSix.IntersectsWith (OneFive))
                                Assert.Fail ("2-6 should intersect with 1-5!");
                        
                        if (! OneFive.IntersectsWith (ZeroSeven))
                                Assert.Fail ("1-5 should intersect with 0-7!");
                        
                        if (! ZeroSeven.IntersectsWith (OneFive))
                        Assert.Fail ("0-7 should intersect with 1-5!");
                        
                        TimeSpan z = OneFive.Intersect (TwoSix);
                        TimeSpan y = OneFive.Intersect (ZeroSeven);
                        TimeSpan x = ZeroSeven.Intersect (OneFive);
                        
                        TimeSpan TwoFive = new TimeSpan (Time.FromSeconds (2),
                                                         Time.FromSeconds (5));
                        
                        TimeSpan OneFive2 = new TimeSpan (Time.FromSeconds (1),
                                                          Time.FromSeconds (5));
                        
                        Assert.AreEqual (TwoFive, z);
                        Assert.AreEqual (OneFive, y);
                        Assert.AreEqual (OneFive2, x);
                        Assert.AreEqual (y, x);

                        // FIXME: More tests for various interesctions                
                }
        
                [Test]
                public void Halving ()
                {
                        TimeSpan ZeroSix = new TimeSpan (Time.FromSeconds (0), 
                                                         Time.FromSeconds (6));
                        
                        TimeSpan leftHalf, rightHalf;
                        ZeroSix.Halve (out leftHalf, out rightHalf);
                        
                        if (leftHalf.Start != Time.FromSeconds (0) || 
                            leftHalf.End   != Time.FromSeconds (3))
                                Assert.Fail ("Left half broken!");
                        
                        if (rightHalf.Start != Time.FromSeconds (3) || 
                            rightHalf.End   != Time.FromSeconds (6))
                                Assert.Fail ("Right half broken!");
                        
                        // Let's check duration in the meantime...
                        Assert.AreEqual (Time.FromSeconds (6), ZeroSix.Duration);
                        Assert.AreEqual (Time.FromSeconds (3), leftHalf.Duration);
                        Assert.AreEqual (Time.FromSeconds (3), rightHalf.Duration);
                }
                
                [Test]
                public void CuttingLeftRight ()
                {
                        TimeSpan OneSix = new TimeSpan (Time.FromSeconds (1),
                                                        Time.FromSeconds (6));
                        
                        TimeSpan TwoSix = new TimeSpan (Time.FromSeconds (2),
                                                        Time.FromSeconds (6));
                        
                        TimeSpan TwoFive = new TimeSpan (Time.FromSeconds (2),
                                                         Time.FromSeconds (5));
                        
                        OneSix.CutLeft (Time.FromSeconds (1));
                        Assert.AreEqual (TwoSix, OneSix);
                        OneSix.CutRight (Time.FromSeconds (1));
                        Assert.AreEqual (TwoFive, OneSix);
                }

        }

}


