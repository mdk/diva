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

namespace Diva.MainMenu {
        
        using System;

        // MenuComponent args //////////////////////////////////////////////////

        public class MenuComponentArgs : EventArgs {
                
                public MenuComponent MenuComponent;
                
                /* CONSTRUCTOR */
                public MenuComponentArgs (MenuComponent component)
                {
                        MenuComponent = component;
                }

        }
        
        // Visibility args /////////////////////////////////////////////////////
        public class VisibilityArgs : EventArgs {
                
                public bool Visible;
                
                /* CONSTRUCTOR */
                public VisibilityArgs (bool visible)
                {
                        Visible = visible;
                }

        }
        
        // Quit args ///////////////////////////////////////////////////////////

        public class QuitArgs : EventArgs {
                
                public int ExitCode;
                
                /* CONSTRUCTOR */
                public QuitArgs (int exitCode)
                {
                        ExitCode = exitCode;
                }

        }
        
}
