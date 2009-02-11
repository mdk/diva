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
        using System;

        public sealed class TestHelper {

                // Fields //////////////////////////////////////////////////////

                string url;
                string mime;
                bool hasVideo;
                bool hasAudio;
                int width;
                int height;
                double fps;
                int audioRate;
                int audioDepth;
                int channels;
                Time length;
                Fraction aspect;

                Inspector inspector;
                bool success;

                // Public methods //////////////////////////////////////////////

                public TestHelper (string url, string mime, int width, int height,  double fps, Fraction aspect,
                                   int audioRate, int audioDepth, int channels, Time length)
                {
                        this.hasAudio = true;
                        this.hasVideo = true;
                        
                        this.url = url;
                        this.mime = mime;
                        this.width = width;
                        this.height = height;
                        this.fps = fps;
                        this.audioRate = audioRate;
                        this.audioDepth = audioDepth;
                        this.channels = channels;
                        this.length = length;
                        this.aspect = aspect;

                        inspector = new Inspector (url);
                        inspector.SetDoneFunc (DoneFunc);
                        inspector.SetErrorFunc (ErrorFunc);
                                                
                        success = false;
                }

                public TestHelper (string url, string mime, int width, int height,  double fps, Fraction aspect,
                                   Time length)
                {
                        this.hasAudio = false;
                        this.hasVideo = true;
                        
                        this.url = url;
                        this.mime = mime;
                        this.width = width;
                        this.height = height;
                        this.fps = fps;
                        this.length = length;
                        this.aspect = aspect;

                        inspector = new Inspector (url);
                        inspector.SetDoneFunc (DoneFunc);
                        inspector.SetErrorFunc (ErrorFunc);
                                                
                        success = false;
                }

                public TestHelper (string url, string mime, int audioRate, int depth, int channels,
                                   Time length)
                {
                        this.hasAudio = true;
                        this.hasVideo = false;
                        
                        this.url = url;
                        this.mime = mime;
                        this.audioRate = audioRate;
                        this.audioDepth = depth;
                        this.length = length;
                        this.channels = channels;

                        inspector = new Inspector (url);
                        inspector.SetDoneFunc (DoneFunc);
                        inspector.SetErrorFunc (ErrorFunc);
                                                
                        success = false;
                }

                public void Start ()
                {
                        inspector.Start ();
                        Gtk.Application.Run ();
                }

                public bool DoneFunc ()
                {
                        success = true;
                        Gtk.Application.Quit ();
                        
                        return false;
                }

                public bool ErrorFunc ()
                {
                        success = false;
                        Gtk.Application.Quit ();

                        return false;
                }

                public void Test ()
                {
                        if (! success)
                                Assert.Fail (String.Format ("Inspecting failed - {0}", inspector.Error));

                        // FIXME: We should modify that (mime) downstream
                        Assert.AreEqual (mime, inspector.Mime.Split (',') [0], "Bad mime type");
                        Assert.AreEqual (url, inspector.Url, "Bad url");

                        if (length != Time.Empty)
                                Assert.AreEqual ((ulong) length, (ulong) inspector.Length,
                                                 (ulong) Time.FromSeconds (1), "Bad length");

                        // Check video format params
                        if (hasVideo) {
                                Assert.IsTrue (inspector.HasVideo, "Video expected but not detected");

                                Assert.AreEqual (width, inspector.VideoFormat.FrameDimensions.Width,
                                                 "Bad width");
                                
                                Assert.AreEqual (height, inspector.VideoFormat.FrameDimensions.Height,
                                                 "Bad height");

                                Assert.AreEqual (fps, (double) inspector.VideoFormat.Fps, 1.0, "Bad fps");

                                Assert.AreEqual (aspect, inspector.VideoFormat.PixelAspect,
                                                 "Bad pixel aspect");
                        }

                        // Check audio format params
                        if (hasAudio) {
                                Assert.IsTrue (inspector.HasAudio, "Audio expected but not detected");
                                
                                Assert.AreEqual (audioRate, inspector.AudioFormat.SampleRate,
                                                 "Bad audio rate");
                                
                                Assert.AreEqual (audioDepth, inspector.AudioFormat.Depth,
                                                 "Bad audio depth");

                                Assert.AreEqual (channels, inspector.AudioFormat.Channels,
                                                 "Bad channel count");
                        }
                }
                

        }

        [TestFixture]
        public class InspectorTests {

                [Test]
                public void DvTest ()
                {
                        Application.Init ();
                        
                        TestHelper dvHelper = new TestHelper ("../../media/Sailing Boat.dv", "video/x-dv",
                                                              720, 576, 25.0, new Fraction (59, 54), 
                                                              48000, 16, 2,
                                                              Time.FromSeconds (10));
                        dvHelper.Start ();
                        dvHelper.Test ();
                }

                [Test]
                public void JpegTest ()
                {
                        Application.Init ();
                        
                        TestHelper jpegHelper = new TestHelper ("../../media/Julia i marysia.jpg", "image/jpeg",
                                                                720, 576, 1.0, new Fraction (1, 1), Time.Empty);
                        jpegHelper.Start ();
                        jpegHelper.Test ();
                }

                [Test]
                public void PngTest ()
                {
                        Application.Init ();
                        
                        TestHelper pngHelper = new TestHelper ("../../media/Martini.png", "image/png",
                                                                720, 576, 1.0, new Fraction (1, 1), Time.Empty);
                        pngHelper.Start ();
                        pngHelper.Test ();
                }
                
                [Test]
                public void WavTest ()
                {
                        Application.Init ();
                        
                        TestHelper wavHelper = new TestHelper ("../../media/La citte.wav", "audio/x-wav",
                                                               48000, 16, 2, Time.FromSeconds (30));
                        wavHelper.Start ();
                        wavHelper.Test ();
                }
                
        }

}