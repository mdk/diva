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

namespace Gdv.NUnit {

        using System;
        using global::NUnit.Framework;
        using Gdv;

        [TestFixture]
        public class FractionTest {

                [Test]
                public void Simplyfying () 
                {
                        Fraction pal = new Fraction (25, 1);
                        Fraction fifty = new Fraction (50, 2);
                        Fraction hundred = new Fraction (100, 4);
                        
                        Assert.AreEqual (pal, fifty, "50/2 should be equal 25/1");
                        Assert.AreEqual (pal, hundred, "100/4 should be equal 25/1");
                        
                        Fraction complex = new Fraction (33, 2);
                        Assert.AreEqual (33, complex.Numerator, "How come this fraction was simplified?");
                        Assert.AreEqual (2, complex.Denominator, "How come this fraction was simplified?");
                        
                        Fraction complex2 = new Fraction (33, 3);
                        Assert.AreEqual (11, complex2.Numerator, "The numerator should be 11 after simplification");
                        Assert.AreEqual (1, complex2.Denominator, "The denominator should be 1 after simplification");
                }
                
                [Test]
                public void Empty () 
                {
                        Fraction empt = new Fraction (25, 0);
                        Assert.AreEqual (Fraction.Empty, empt, "This fraction should report as empty");
                }
                
                [Test]
                public void Double () 
                {
                        Fraction pal = new Fraction (25, 1);
                        pal = pal.Invert ();
                        Assert.AreEqual ((double) 1 / 25, (double) pal, "1/25 should equal 0.04");
                        
                        Fraction ntscinv = new Fraction (1, 30);
                        Assert.AreEqual ((double) 1/ 30, (double) ntscinv, "1/30 should equal 0.03333333(3)");
                }
                
                [Test]
                public void Fps ()
                {
                        Fraction pal = new Fraction (25, 1);
                        Assert.AreEqual (Time.FromNSeconds (40000000), pal.FpsFrameDuration (), "Borked frame duration");
                        Assert.AreEqual (Time.FromNSeconds (40000000), pal.FpsNormalizeTimeRnd (Time.FromNSeconds (21000000)));
                        Assert.AreEqual (Time.FromNSeconds (0), pal.FpsNormalizeTime(Time.FromNSeconds (21000000)));
                        Assert.AreEqual (1, pal.FpsFrameAtTimeRnd (Time.FromNSeconds (41000000)));
                        Assert.AreEqual (1, pal.FpsFrameAtTime (Time.FromNSeconds (41000000)));
                        Assert.AreEqual (1, pal.FpsFrameAtTime (Time.FromNSeconds (61000000)));
                        Assert.AreEqual (2, pal.FpsFrameAtTimeRnd (Time.FromNSeconds (61000000)));
                        Assert.AreEqual (Time.FromNSeconds (40000000), pal.FpsTimeAtFrame (1));
                }

                [Test]
                public void FpsDigitize ()
                {
                         Fraction pal = new Fraction (25, 1);
                         Fraction ntsc = new Fraction (30000, 1001);
                         Fraction some = new Fraction (31, 2);

                         Assert.AreEqual (16, some.FpsDigitize ());
                         Assert.AreEqual (25, pal.FpsDigitize ());
                         Assert.AreEqual (30, ntsc.FpsDigitize ());
                }
                
        }

}
