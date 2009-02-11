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

namespace Gdv.Boilers {

        using System;
        using Gdv;
        using System.Collections.Generic;
        using Diva.Basics;

        [BoilersAttribute]
        public static class Track {

                [BoilFuncAttribute (typeof (Gdv.Track))]
                public static void Boil (object o, ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.Track track = o as Gdv.Track;
                        
                        container.Add (new RefParameter ("projectformat", track.ProjectFormat, provider));
                        container.Add (new IntParameter ("layer", track.Layer));
                        container.Add (new DoubleParameter ("opacity", track.Opacity));

                        foreach (Gdv.Clip clip in track.Clips)
                                container.Add (new RefParameter ("clip", clip, provider));
                }

                [GetDepObjectsFuncAttribute (typeof (Gdv.Track))]
                public static List <object> GetDepObjects (object o)
                {
                        Gdv.Track track = o as Gdv.Track;
                        List <object> lst = BoilFactory.GetList (track.ProjectFormat);
                        foreach (Gdv.Clip clip in track.Clips)
                                lst.Add (clip);

                        return lst;
                }

                [UnBoilFuncAttribute (typeof (Gdv.Track))]
                public static object UnBoil (ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.ProjectFormat format = (Gdv.ProjectFormat)
                                container.FindRef ("projectformat").ToObject (provider);

                        int layer = container.FindInt ("layer").Value;
                        double opacity = container.FindDouble ("opacity").Value;

                        Gdv.Track track = new Gdv.Track (format, layer);
                        track.Opacity = opacity;

                        foreach (RefParameter reff in container.FindAllRefs ("clip")) {
                                Gdv.Clip clip = (Gdv.Clip) reff.ToObject (provider);
                                track.AddClip (clip);
                        }

                        return track;
                }

        }

}