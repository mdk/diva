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

        using global::NUnit.Framework;
        using Gdv;
        
        [TestFixture]
        public class FrameDimensionsTest {

                [Test]
                public void Basic () 
                {
                        FrameDimensions dim1 = new FrameDimensions (320,200);
                        FrameDimensions dim2 = new FrameDimensions (640,400);
                        
                        FrameDimensions fit1 = dim2.FitWidth (320);
                        FrameDimensions fit2 = dim1.FitWidth (640);
                
                        Assert.AreEqual (dim1, fit1);
                        Assert.AreEqual (dim2, fit2);
                        
                        FrameDimensions fit3 = dim2.FitHeight (200);
                        FrameDimensions fit4 = dim1.FitHeight (400);
                        
                        Assert.AreEqual (dim1, fit3);
                        Assert.AreEqual (dim2, fit4);
                        
                        // Some duplication checking
                        FrameDimensions cpy1 = dim1;
                        FrameDimensions cpy2 = dim1;
                        cpy1.Width = 1024;
                        cpy1.Height = 768;
                        cpy2.Width = 320;
                        cpy2.Height = 200;
                        
                        Assert.IsFalse (cpy1 == dim1);
                        Assert.IsTrue (cpy2 == dim1);
                }
                
        }
        
}
