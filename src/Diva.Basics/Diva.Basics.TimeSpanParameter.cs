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
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;

        [TypeAttribute ("timespan")]
        public sealed class TimeSpanParameter : DataParameter {

                // Fields //////////////////////////////////////////////////////

                TimeSpan val;

                // Properties //////////////////////////////////////////////////

                public TimeSpan Value {
                        get { return val; }
                        set { val = value; }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public TimeSpanParameter (string name, TimeSpan val) :
                        base (name)
                {
                        this.val = val;
                }

                /* CONSTRUCTOR */
                public TimeSpanParameter (XmlElement element) :
                        base (element)
                {
                        Gdv.Time start = Gdv.Time.Zero;
                        Gdv.Time end = Gdv.Time.Zero;
                        
                        foreach (XmlNode node in element.ChildNodes) {
                                TimeParameter param = new TimeParameter ((XmlElement) node);
                                
                                if (param.Name == "start")
                                        start = param.Value;
                                
                                if (param.Name == "end")
                                        end = param.Value;
                        }

                        this.val = new TimeSpan (start, end);
                }

                // Private methods /////////////////////////////////////////////

                protected override void FillXmlElement (XmlDocument document,
                                                        XmlElement element)
                {
                        TimeParameter start = new TimeParameter ("start", val.Start);
                        TimeParameter end = new TimeParameter ("end", val.End);                        
                        
                        element.AppendChild (start.ToXmlElement (document));
                        element.AppendChild (end.ToXmlElement (document));                        
                }
                
        }

}