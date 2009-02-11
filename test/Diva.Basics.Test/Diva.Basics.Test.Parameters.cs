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
        
        public static class Parameters {
                
                public static void String ()
                {
                        Console.WriteLine ("String parameter test");
                        
                        ObjectContainer container = new ObjectContainer ("object",
                                                                         typeof (object),
                                                                         100);

                        // Create some string params
                        StringParameter string1 = new StringParameter ("name", "bigred");
                        StringParameter string2 = new StringParameter ("brand", "ford");
                        StringParameter string3 = new StringParameter ("kind", "scorpio");

                        // Add 
                        container.Add (string1);
                        container.Add (string2);
                        container.Add (string3);

                        // Find the added params and check
                        if (container.FindString ("name").Value != "bigred")
                                throw new Exception ();

                        if (container.FindString ("brand").Value != "ford")
                                throw new Exception ();

                        if (container.FindString ("kind").Value != "scorpio")
                                throw new Exception ();

                        // Remove
                        container.Remove (string1);
                        container.Remove (string2);
                        container.Remove (string3);
                }

                public static void Time ()
                {
                        Console.WriteLine ("Time parameter test");
                        
                        ObjectContainer container = new ObjectContainer ("object",
                                                                         typeof (object),
                                                                         101);

                        // Create some string params
                        TimeParameter time1 = new TimeParameter ("second", Gdv.Time.FromSeconds (1));
                        TimeParameter time2 = new TimeParameter ("two", Gdv.Time.FromSeconds (2));
                        TimeParameter time3 = new TimeParameter ("ten", Gdv.Time.FromSeconds (10));

                        // Add 
                        container.Add (time1);
                        container.Add (time2);
                        container.Add (time3);

                        // Find the added params and check
                        if (container.FindTime ("second").Value != Gdv.Time.FromSeconds (1))
                                throw new Exception ();

                        if (container.FindTime ("two").Value != Gdv.Time.FromSeconds (2))
                                throw new Exception ();

                        if (container.FindTime ("ten").Value != Gdv.Time.FromSeconds (10))
                                throw new Exception ();

                        // Remove
                        container.Remove (time1);
                        container.Remove (time2);
                        container.Remove (time3);
                }
        
        }

}

