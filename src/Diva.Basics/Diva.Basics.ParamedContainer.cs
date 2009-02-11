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

        public abstract class ParamedContainer : DataContainer {

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ParamedContainer (string name) :
                        base (name)
                {
                }
                
                /* CONSTRUCTOR */
                public ParamedContainer (XmlElement element) :
                        base (element)
                {
                }

                public StringParameter FindString (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is StringParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (StringParameter),
                                                                   element.GetType ());
                        else
                                return element as StringParameter;
                }

                public BoolParameter FindBool (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is BoolParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (BoolParameter),
                                                                   element.GetType ());
                        else
                                return element as BoolParameter;
                }

                public IntParameter FindInt (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is IntParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (IntParameter),
                                                                   element.GetType ());
                        else
                                return element as IntParameter;
                }

                public DoubleParameter FindDouble (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is DoubleParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (DoubleParameter),
                                                                   element.GetType ());
                        else
                                return element as DoubleParameter;
                }

                public TimeParameter FindTime (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is TimeParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (TimeParameter),
                                                                   element.GetType ());
                        else
                                return element as TimeParameter;
                }

                public TimeSpanParameter FindTimeSpan (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is TimeSpanParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (TimeSpanParameter),
                                                                   element.GetType ());
                        else
                                return element as TimeSpanParameter;
                }

                public List <RefParameter> FindAllRefs (string name)
                {
                        List <RefParameter> lst = new List <RefParameter> ();
                        foreach (DataElement element in FindAllByName (name)) {
                                if (! (element is RefParameter))
                                        throw LookupException.NewMismatch (name,
                                                                           typeof (RefParameter),
                                                                           element.GetType ());
                                else
                                        lst.Add (element as RefParameter);
                        }

                        return lst;
                }

                public List <ObjectContainer> FindAllObjects ()
                {
                        List <ObjectContainer> lst = new List <ObjectContainer> ();
                        foreach (DataElement element in FindAllByType (typeof (ObjectContainer))) {
                                lst.Add (element as ObjectContainer);
                        }

                        return lst;
                }

                public RefParameter FindRef (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is RefParameter))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (RefParameter),
                                                                   element.GetType ());
                        else
                                return element as RefParameter;
                }

                public ObjectContainer FindObjectContainer (string name)
                {
                        DataElement element = FindOneByName (name);
                        if (! (element is ObjectContainer))
                                throw LookupException.NewMismatch (name,
                                                                   typeof (ObjectContainer),
                                                                   element.GetType ());
                        else
                                return element as ObjectContainer;
                }
                
        }

}