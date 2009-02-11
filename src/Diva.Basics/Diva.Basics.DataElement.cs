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

        public abstract class DataElement {
                
                // Fields //////////////////////////////////////////////////////

                protected string name; // Name of the element
                
                // Properties //////////////////////////////////////////////////

                public string Name {
                        get { return name; }
                }

                public string Type {
                        get {
                                // Let's try getting the attribute
                                if (! (this.GetType ().IsDefined (typeof (TypeAttribute),
                                                                  false)))
                                        throw new Exception ("DataElement does not implement the TypeAttribute");
                                
                                object[] attributes = this.GetType ().GetCustomAttributes (typeof (TypeAttribute),
                                                                                           false);
                                return (attributes [0] as TypeAttribute).TypeName;
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public DataElement (string name) 
                {
                        this.name = name;
                }

                /* CONSTRUCTOR */
                public DataElement (XmlElement element)
                {
                        this.name = element.GetAttribute ("name");
                }

                public virtual XmlElement ToXmlElement (XmlDocument document)
                {
                        XmlElement element = document.CreateElement (Type);
                        element.SetAttribute ("name", name);

                        FillXmlElement (document, element);

                        return element;
                }

                // Private /////////////////////////////////////////////////////

                protected abstract void FillXmlElement (XmlDocument document,
                                                        XmlElement element);

        }

}