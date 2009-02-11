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
        using System.Xml;
        using Gdv;
        using System.IO;
        
        public static class UnBoiling {

                static XmlDocument xmlDocument = null;

                public static void InspectDocument ()
                {
                        Console.WriteLine ("Inspecting boilable document");

                        ObjectListContainer root = (ObjectListContainer)
                                DataFactory.MakeDataElement (xmlDocument.DocumentElement);

                        ObjectContainer container1 = root.FindObjectContainer ("object1");
                        ObjectContainer container2 = root.FindObjectContainer ("object2");
                        
                        Plane plane = (Plane) Basics.BoilFactory.UnBoil (container1, null);
                        Ship ship = (Ship) Basics.BoilFactory.UnBoil (container2, null);

                        if (plane.Brand != "boeing")
                                throw new Exception ();

                        if (plane.Model != "707")
                                throw new Exception ();

                        if (plane.CruiseTime != Time.FromSeconds (2600))
                                throw new Exception ();

                        if (ship.Model != "tanker")
                                throw new Exception ();

                        if (ship.Port != "danzig")
                                throw new Exception ();

                        if (ship.CruiseTime != Time.FromSeconds (3200))
                                throw new Exception ();
                }

                public static void ReadDocument ()
                {
                        Console.WriteLine ("Loading boiled document");
                        xmlDocument = new XmlDocument ();
                        xmlDocument.Load ("boiled.xml");
                }

                public static void RemoveDocument ()
                {
                        Console.WriteLine ("Removing boiled document");
                        File.Delete ("boiled.xml");
                }
        
        }

}