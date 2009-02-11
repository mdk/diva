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

/* Test cases for VideoFormat, AudioFormat, ProjectFormat */

namespace Gdv.NUnit {

        using global::NUnit.Framework;
        using Gdv;
        
        [TestFixture]
        public class FormatTest {
                
                [Test]
                public void VideoFormat ()
                {
                        Application.Init ();
                                                
                        VideoFormat vformat = new VideoFormat ();
                        
                        vformat.FrameDimensions = new FrameDimensions (768, 576);
                        vformat.Fps = new Fraction (30, 1);
                        vformat.PixelAspect = new Fraction (1, 10);
                        
                        Assert.AreEqual (768, vformat.FrameDimensions.Width, "Wrong width!");
                        Assert.AreEqual (576, vformat.FrameDimensions.Height, "Wrong height!");
                        Assert.AreEqual (new Fraction (30, 1), vformat.Fps, "Wrong fps!");
                        Assert.AreEqual (new Fraction (1, 10), vformat.PixelAspect, "Wrong aspect!");
                }
                
                [Test]
                public void AudioFormat ()
                {
                        Application.Init ();
                        
                        AudioFormat aformat = new AudioFormat ();
                        
                        aformat.SampleRate = 22000;
                        aformat.Depth = 8;
                        aformat.Channels = 1;
                        
                        Assert.AreEqual (22000, aformat.SampleRate, "Wrong samplerate!");
                        Assert.AreEqual (8, aformat.Depth, "Wrong depth!");
                        Assert.AreEqual (1, aformat.Channels, "Wrong channels!");
                }

                [Test]
                public void VideoCloning ()
                {
                        Application.Init ();

                        VideoFormat vFormat = new VideoFormat ();
                        vFormat.FrameDimensions = new FrameDimensions (768, 576);
                        vFormat.Fps = new Fraction (30, 1);
                        vFormat.PixelAspect = new Fraction (1, 10);

                        VideoFormat clonedFormat = vFormat.Clone ();

                        Assert.IsFalse (vFormat.Handle == clonedFormat.Handle, "Ptr's should not be equal");
                        Assert.AreEqual (vFormat.FrameDimensions.Width, clonedFormat.FrameDimensions.Width, "Wrong width!");
                        Assert.AreEqual (vFormat.FrameDimensions.Height, clonedFormat.FrameDimensions.Height, "Wrong height!");
                        Assert.AreEqual (vFormat.Fps, clonedFormat.Fps, "Wrong fps!");
                        Assert.AreEqual (vFormat.PixelAspect, clonedFormat.PixelAspect, "Wrong aspect!");
                }

                [Test]
                public void AudioCloning ()
                {
                        Application.Init ();

                        AudioFormat aFormat = new AudioFormat ();
                        aFormat.SampleRate = 22000;
                        aFormat.Depth = 8;
                        aFormat.Channels = 1;
                        
                        AudioFormat clonedFormat = aFormat.Clone ();

                        Assert.IsFalse (aFormat.Handle == clonedFormat.Handle, "Ptr's should not be equal");
                        Assert.AreEqual (aFormat.SampleRate, clonedFormat.SampleRate, "Wrong samplerate!");
                        Assert.AreEqual (aFormat.Depth, clonedFormat.Depth, "Wrong depth!");
                        Assert.AreEqual (aFormat.Channels, clonedFormat.Channels, "Wrong channels!");
                }

        }
                
}
