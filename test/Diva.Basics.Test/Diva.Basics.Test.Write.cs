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
        
        public static class Write {

                static XmlDocument xmlDocument = null;

                public static void CreateDocument ()
                {
                        Console.WriteLine ("Creating document");
                        
                        ObjectContainer container1 = new ObjectContainer ("object1",
                                                                          typeof (object),
                                                                          1);

                        ObjectContainer container2 = new ObjectContainer ("object2",
                                                                          typeof (object),
                                                                          2);
                        // Create some parameters
                        StringParameter string11 = new StringParameter ("brand", "ford");
                        StringParameter string12 = new StringParameter ("brand", "mercedes");

                        TimeParameter time11 = new TimeParameter ("warranty", Gdv.Time.FromSeconds (3600));
                        TimeParameter time12 = new TimeParameter ("warranty", Gdv.Time.FromSeconds (7200));
                        
                        // Add all the parameters
                        container1.Add (string11);
                        container2.Add (string12);
                        container1.Add (time11);
                        container2.Add (time12);

                        // Xml document stuff
                        xmlDocument = new XmlDocument ();
                        XmlNode xmlNode = xmlDocument.CreateNode (XmlNodeType.XmlDeclaration, "", "");
                        xmlDocument.AppendChild (xmlNode);

                        // The object list
                        ObjectListContainer root = new ObjectListContainer ("root");
                        root.Add (container1);
                        root.Add (container2);
                        
                        xmlDocument.AppendChild (root.ToXmlElement (xmlDocument));
                }

                public static void WriteDocument ()
                {
                        Console.WriteLine ("Writing document");
                        xmlDocument.Save ("basics.xml");
                }
        
        }

}