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
        
        [TypeAttribute ("int")]
        public sealed class IntParameter : DataParameter {

                // Fields //////////////////////////////////////////////////////

                int val;
                
                // Properties //////////////////////////////////////////////////

                public int Value {
                        get { return val; }
                        set { val = value; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public IntParameter (string name, int val) :
                        base (name)
                {
                        this.val = val;
                }

                /* CONSTRUCTOR */
                public IntParameter (XmlElement element) :
                        base (element)
                {
                        this.val = Convert.ToInt32 (element.FirstChild.Value);
                }

                // Private methods /////////////////////////////////////////////

                protected override void FillXmlElement (XmlDocument document,
                                                        XmlElement element)
                {
                        XmlNode textNode = document.CreateTextNode (val.ToString ());
                        element.AppendChild (textNode);
                }
                
        }

}