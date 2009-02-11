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

namespace Diva.Basics {

        using System;
        
        public class LookupException : Exception {
                
                public static LookupException NewNotFound (string needle, string stack)
                {
                        return new LookupException (String.Format ("'{0}' was not found in '{1}'", needle, stack));
                }

                public static LookupException NewIncomplete (string needle, string stack)
                {
                        return new LookupException (String.Format ("'{0}' specifiaction incomplete in '{1}'", needle, stack));
                }
                        
                public static LookupException NewMismatch (string name, Type expected, Type was)
                {
                        return new LookupException (String.Format ("While searching for '{0}' type {1} was expected but got {2}",
                                                                   expected, was));
                }

                public static LookupException UnsupportedData (string data)
                {
                        return new LookupException (String.Format ("Data of type '{0}' is not supported."));
                }
                
                /* CONSTRUCTOR */
                protected LookupException (string text) : base (text)
                {
                }

        }

}
        

