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
        using System.Collections.Generic;

        public abstract class DataContainer : DataElement {

                // Fields //////////////////////////////////////////////////////

                List <DataElement> children = null;

                // Properties //////////////////////////////////////////////////

                public int Count {
                        get { return children.Count; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public DataContainer (string name) :
                        base (name)
                {
                        children = new List <DataElement> ();
                }

                /* CONSTRUCTOR */
                public DataContainer (XmlElement element) :
                        base (element)
                {
                        children = new List <DataElement> ();

                        foreach (XmlNode node in element.ChildNodes) 
                                if (node is XmlElement) {
                                        DataElement data = DataFactory.MakeDataElement (node as XmlElement);
                                        Add (data);
                                }
                }

                /* Find all elements with a given name */
                public List <DataElement> FindAllByName (string name)
                {
                        List <DataElement> list = new List <DataElement> ();

                        foreach (DataElement element in children)
                                if (element.Name == name)
                                        list.Add (element);

                        return list;
                }

                public List <DataElement> FindAllByType (Type type)
                {
                        List <DataElement> list = new List <DataElement> ();

                        foreach (DataElement element in children)
                                if (element.GetType () == type)
                                        list.Add (element);

                        return list;
                }

                /* Find one with a given name. The first one. Throws exception
                   if not found */
                public DataElement FindOneByName (string name)
                {
                        foreach (DataElement element in children)
                                if (element.Name == name)
                                        return element;

                        throw LookupException.NewNotFound (name, this.Name);
                }

                /* Check if we have a given data element */
                public bool Has (string name)
                {
                        foreach (DataElement element in children)
                                if (element.Name == name)
                                        return true;

                        return false;
                }

                /* Add the given data element */
                public void Add (DataElement element)
                {
                        children.Add (element);
                }

                /* Remove the given data element */
                public void Remove (DataElement element)
                {
                        children.Remove (element);
                }

                /* A default implementation that should work in most cases. Fill-in creating
                 * sub-nodes for each children */
                protected override void FillXmlElement (XmlDocument document,
                                                        XmlElement element)
                {
                        foreach (DataElement child in children) {
                                XmlElement xmlEle = child.ToXmlElement (document);
                                element.AppendChild (xmlEle);
                        }
                }
                  
        }

}
