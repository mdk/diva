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

namespace Diva.PluginLib {

        using System;
        using System.Collections;
        using System.Collections.Generic;

        public class TypeEnumerator : IEnumerator
        {
                
                // Fields //////////////////////////////////////////////////////

                Type ourType = null;
                int savedCount;
                int currentItem;
                List <Plugin> list;

                // Properties //////////////////////////////////////////////////

                public object Current {
                        get {
                                if (savedCount != list.Count || currentItem == -1)
                                        throw new InvalidOperationException ();

                                return list [currentItem];
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public TypeEnumerator (List <Plugin> list, Type type)
                {
                        savedCount = list.Count;
                        ourType = type;
                        currentItem = -1;
                        this.list = list;
                }

                /* Move the enumerator to the next position. We try to find an
                 * element of our type */
                public bool MoveNext ()
                {
                        for (int i = (currentItem != -1) ? currentItem + 1 : 0 ; i < list.Count; i++) {
                                if (ourType.IsInstanceOfType (list [i])) {
                                        currentItem = i;
                                        return true;
                                }
                        }

                        // It was not found...
                        currentItem = -1;
                        return false;
                }

                public void Reset ()
                {
                        currentItem = -1;
                }

        }

}
                
                
        