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
        public static class DecodeBin {

                [BoilFuncAttribute (typeof (Gdv.DecodeBin))]
                public static void Boil (object o, ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.DecodeBin bin = (o as Gdv.DecodeBin);
                        Common.BoilFileBased (o, container, provider);
                        if (bin.HasVideo == true)
                                container.Add (new RefParameter ("videoformat", bin.VideoFormat, provider));

                        if (bin.HasAudio == true)
                                container.Add (new RefParameter ("audioformat", bin.AudioFormat, provider));

                        container.Add (new TimeParameter ("length", (o as Gdv.DecodeBin).Length));
                }

                [GetDepObjectsFuncAttribute (typeof (Gdv.DecodeBin))]
                public static List <object> GetDepObjects (object o)
                {
                        List <object> lst = new List <object> ();
                        Gdv.DecodeBin bin = (o as Gdv.DecodeBin);
                        
                        if (bin.HasVideo == true)
                                lst.Add (bin.VideoFormat);
                         if (bin.HasAudio == true)
                                lst.Add (bin.AudioFormat);
                        
                        return lst;
                }

                [UnBoilFuncAttribute (typeof (Gdv.DecodeBin))]
                public static object UnBoil (ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.VideoFormat vFormat = null;
                        Gdv.AudioFormat aFormat = null;
                        
                        if (container.Has ("videoformat"))
                                vFormat = (Gdv.VideoFormat) container.FindRef ("videoformat").ToObject (provider);
                        
                        if (container.Has ("audioformat"))
                                aFormat = (Gdv.AudioFormat) container.FindRef ("audioformat").ToObject (provider);
                        
                        Gdv.Time time =  container.FindTime ("length").Value;
                        string file = container.FindString ("basefile").Value;

                        return new Gdv.DecodeBin (file, vFormat, aFormat, time);
                }

        }

}
