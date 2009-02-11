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
        using System.Xml;
        using System.Reflection;
        using System.Collections.Generic;

        public static class DataFactory {

                // Fields //////////////////////////////////////////////////////

                static Dictionary <string, Type> stringToType = null;

                static readonly Type[] constructorTypes = new Type[1] { typeof (XmlElement) };

                // Public methods //////////////////////////////////////////////

                static public void Initialize ()
                {
                        stringToType = new Dictionary <string, Type> ();
                        ScanSelf ();
                }

                /* Call the constructor for the right type */
                static public DataElement MakeDataElement (XmlElement element)
                {
                        if (! stringToType.ContainsKey (element.Name))
                                throw LookupException.UnsupportedData (element.Name);

                        Type type = stringToType [element.Name];
                        
                        object o = type.GetConstructor (constructorTypes).Invoke (new object [1] {element});

                        return o as DataElement;
                }

                // Private methods /////////////////////////////////////////////

                /* Scans the given assembly to create a dictionatry of the
                   respective canonical type names ('string') mapped to true
                   System.Type's */
                static void ScanAssemblyToDictionary (Assembly ass)
                {
                        foreach (Type type in ass.GetTypes ()) 
                                if (type.IsDefined (typeof (TypeAttribute), false)) {
                                        // This is a potential type
                                        object[] attrs = type.GetCustomAttributes (typeof (TypeAttribute), false);
                                        stringToType [(attrs [0] as TypeAttribute).TypeName] = type;
                                }
                }
                        
                /* Scan self assembly */
                static void ScanSelf ()
                {
                        Assembly current = Assembly.GetAssembly (typeof (DataFactory));
                        ScanAssemblyToDictionary (current);
                }
                
        }

}
                