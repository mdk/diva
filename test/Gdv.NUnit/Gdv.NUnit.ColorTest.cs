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

/* Test cases for Gdv.Color */

namespace Gdv.NUnit {

        using global::NUnit.Framework;
        using Gdv;

        [TestFixture]
        public class ColorTests {

                [Test]
                public void Basic ()
                {
                        Color color = new Color (1,1,1);
                        Assert.AreEqual (1, color.Red);
                        Assert.AreEqual (255, color.Red8);
                        Assert.AreEqual (1, color.Green);
                        Assert.AreEqual (255, color.Green8);
                        Assert.AreEqual (1, color.Blue);
                        Assert.AreEqual (255, color.Blue8);
                }
                
                [Test]
                public void Tango ()
                {
                        Color Chameleon1 = Color.FromTango (ColorTango.Chameleon1);
                        Assert.AreEqual (138, Chameleon1.Red8);
                        Assert.AreEqual (226, Chameleon1.Green8);
                        Assert.AreEqual (52, Chameleon1.Blue8);
                        
                        Color SkyBlue2 = Color.FromTango (ColorTango.SkyBlue2);
                        Assert.AreEqual (52, SkyBlue2.Red8);
                        Assert.AreEqual (101, SkyBlue2.Green8);
                        Assert.AreEqual (164, SkyBlue2.Blue8);
                        
                        Assert.IsTrue (SkyBlue2 != Chameleon1);
                }
                
                [Test]
                public void Html ()
                {
                        Color Chameleon1 = Color.FromTango (ColorTango.Chameleon1);
                        Color Chameleon2 = Color.FromTango (ColorTango.Chameleon2);
                        Color Chameleon3 = Color.FromTango (ColorTango.Chameleon3);
                        
                        Assert.AreEqual ("8AE234", Chameleon1.ToHtml ());
                        Assert.AreEqual ("73D216", Chameleon2.ToHtml ());
                        Assert.AreEqual ("4E9A06", Chameleon3.ToHtml ());
                }
                
        }

}
