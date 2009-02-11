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

namespace Diva.Basics.Test {

        using System;
        using Gdv;
        using System.Collections.Generic;

        [BoilersAttribute]
        public static class ShipBoiler {

                [BoilFuncAttribute (typeof (Ship))]
                public static void ShipBoil (object o, ObjectContainer container, IBoilProvider provider)
                {
                        Ship ship = o as Ship;
                        container.Add (new StringParameter ("model", ship.Model));
                        container.Add (new StringParameter ("port", ship.Port));
                        container.Add (new TimeParameter ("cruisetime", ship.CruiseTime));
                }

                [GetDepObjectsFuncAttribute (typeof (Ship))]
                public static List <object> ShipGetDepObjects (object o)
                {
                        return null;
                }

                [UnBoilFuncAttribute (typeof (Ship))]
                public static object ShipUnBoil (ObjectContainer container, IBoilProvider provider)
                {
                        string model = container.FindString ("model").Value;
                        string port = container.FindString ("port").Value;
                        Time cruise = container.FindTime ("cruisetime").Value;
                        return new Ship (model, port, cruise);
                }

        }

}