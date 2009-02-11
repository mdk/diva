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

/* A tag (a label -- in gmail style) */

namespace Diva.Core {

        using System.Collections.Generic;
        using System;
        using Gdk;
        using Util;
        using Basics;

        public class Tag : IBoilable {
                
                // Events //////////////////////////////////////////////////////
                
                public event TagHandler Change;
                
                // Fields /////////////////////////////////////////////////////

                string name; // A name. Unique in the project
                
                // Properties /////////////////////////////////////////////////
                
                public string Name { 
                        get { return name; }
                        set { 
                                name = value;
                                if (Change != null)
                                        Change (this, new TagArgs (this));
                        }
                }
                
                // Public methods /////////////////////////////////////////////
                                
                /* CONSTRUCTOR */
                public Tag (string name)
                {
                        this.name = name;
                }

                /* CONSTRUCTOR */
                public Tag (ObjectContainer container, IBoilProvider provider)
                {
                        this.name = container.FindString ("name").Value;
                }

                public override string ToString ()
                {
                        return String.Format ("tag:{0}", Name);
                }

                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        container.Add (new StringParameter ("name", name));
                }

                public List <object> GetDepObjects ()
                {
                        return null;
                }

                // Private methods ////////////////////////////////////////////

        }
        
}
                
                
