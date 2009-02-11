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
        
        public static class Boiling {

                static XmlDocument xmlDocument = null;

                public static void CreateDocument ()
                {
                        Console.WriteLine ("Creating boilable document");
                        
                        Plane plane = new Plane ("boeing", "707", Gdv.Time.FromSeconds (2600));
                        Ship ship = new Ship ("tanker", "danzig", Gdv.Time.FromSeconds (3200));                        

                        // Xml document stuff
                        xmlDocument = new XmlDocument ();
                        XmlNode xmlNode = xmlDocument.CreateNode (XmlNodeType.XmlDeclaration, "", "");
                        xmlDocument.AppendChild (xmlNode);

                        // The object list
                        ObjectListContainer root = new ObjectListContainer ("root");
                        root.Add (BoilFactory.Boil ("object1", plane, null));
                        root.Add (BoilFactory.Boil ("object2", ship, null));                        
                        
                        xmlDocument.AppendChild (root.ToXmlElement (xmlDocument));
                }

                public static void WriteDocument ()
                {
                        Console.WriteLine ("Writing boiled document");
                        xmlDocument.Save ("boiled.xml");
                }
        
        }

}