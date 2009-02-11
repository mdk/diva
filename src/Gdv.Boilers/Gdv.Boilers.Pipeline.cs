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
        public static class Pipeline {

                [BoilFuncAttribute (typeof (Gdv.Pipeline))]
                public static void Boil (object o, ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.Pipeline pipe = (o as Gdv.Pipeline);
                        container.Add (new RefParameter ("projectformat", pipe.ProjectFormat, provider));
                        foreach (Gdv.Track track in pipe.Tracks)
                                container.Add (new RefParameter ("track", track, provider));
                }

                [GetDepObjectsFuncAttribute (typeof (Gdv.Pipeline))]
                public static List <object> GetDepObjects (object o)
                {
                        Gdv.Pipeline pipe = (o as Gdv.Pipeline);
                        List <object> lst = BoilFactory.GetList (pipe.ProjectFormat);

                        foreach (Gdv.Track track in pipe.Tracks)
                                lst.Add (track);

                        return lst;
                }

                [UnBoilFuncAttribute (typeof (Gdv.Pipeline))]
                public static object UnBoil (ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.ProjectFormat format = (Gdv.ProjectFormat)
                                container.FindRef ("projectformat").ToObject (provider);
                        
                        Gdv.Pipeline pipe = new Gdv.Pipeline (format);

                        foreach (RefParameter reff in container.FindAllRefs ("track")) {
                                Gdv.Track track = (Gdv.Track) reff.ToObject (provider);
                                pipe.AddTrack (track);
                        }

                        return pipe;
                }

        }

}