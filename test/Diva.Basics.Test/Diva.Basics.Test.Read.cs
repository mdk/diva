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
        using System.IO;
        
        public static class Read {

                static XmlDocument xmlDocument = null;

                public static void InspectDocument ()
                {
                        Console.WriteLine ("Inspecting document");

                        ObjectListContainer root = (ObjectListContainer)
                                DataFactory.MakeDataElement (xmlDocument.DocumentElement);

                        ObjectContainer container1 = root.FindObjectContainer ("object1");
                        ObjectContainer container2 = root.FindObjectContainer ("object2");

                        if (container1.RefId != 1)
                                throw new Exception ();

                        if (container2.RefId != 2)
                                throw new Exception ();

                        if (container1.SystemType != (typeof (object)).ToString ())
                                throw new Exception ();

                        if (container2.SystemType != (typeof (object)).ToString ())
                                throw new Exception ();

                        // Check the object containers

                        StringParameter string1 = container1.FindString ("brand");
                        if (string1.Value != "ford")
                                throw new Exception ();

                        StringParameter string2 = container2.FindString ("brand");
                        if (string2.Value != "mercedes")
                                throw new Exception ();

                        TimeParameter warranty1 = container1.FindTime ("warranty");
                        if (warranty1.Value.Seconds != 3600)
                                throw new Exception ();

                        TimeParameter warranty2 = container2.FindTime ("warranty");
                        if (warranty2.Value.Seconds != 7200)
                                throw new Exception ();
                }

                public static void ReadDocument ()
                {
                        Console.WriteLine ("Loading document");
                        xmlDocument = new XmlDocument ();
                        xmlDocument.Load ("basics.xml");
                }

                public static void RemoveDocument ()
                {
                        Console.WriteLine ("Removing document");
                        File.Delete ("basics.xml");
                }
        
        }

}