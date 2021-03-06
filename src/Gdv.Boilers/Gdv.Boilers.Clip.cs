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
        public static class Clip {

                [BoilFuncAttribute (typeof (Gdv.Clip))]
                public static void Boil (object o, ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new RefParameter ("source", (o as Gdv.Clip).Source, provider));
                        container.Add (new RefParameter ("parentitem", (o as Gdv.Clip).ParentItem, provider));
                        container.Add (new TimeSpanParameter ("timelinespan", (o as Gdv.Clip).TimelineSpan));
                        container.Add (new TimeSpanParameter ("sourcespan", (o as Gdv.Clip).SourceSpan));
                }

                [GetDepObjectsFuncAttribute (typeof (Gdv.Clip))]
                public static List <object> GetDepObjects (object o)
                {
                        return BoilFactory.GetList ((o as Gdv.Clip).Source,
                                                    (o as Gdv.Clip).ParentItem);
                }

                [UnBoilFuncAttribute (typeof (Gdv.Clip))]
                public static object UnBoil (ObjectContainer container, IBoilProvider provider)
                {
                        Gdv.Source source = (Gdv.Source) container.FindRef ("source").ToObject (provider);
                        Gdv.MediaItem item = (Gdv.MediaItem) container.FindRef ("parentitem").ToObject (provider);
                        Gdv.TimeSpan timelineSpan = container.FindTimeSpan ("timelinespan").Value;
                        Gdv.TimeSpan sourceSpan = container.FindTimeSpan ("sourcespan").Value;

                        Gdv.Clip clip = new Gdv.Clip (source, item);
                        clip.TimelineSpan = timelineSpan;
                        clip.SourceSpan = sourceSpan;

                        return clip;
                }

        }

}