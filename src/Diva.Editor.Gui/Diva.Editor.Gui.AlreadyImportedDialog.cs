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

namespace Diva.Editor.Gui {
        
        using Gtk;
        using System;
        using Mono.Unix;
        
        public sealed class AlreadyImportedDialog : MessageDialog {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string importSS = Catalog.GetString
                        ("Import");
                
                readonly static string skipSS = Catalog.GetString
                        ("Skip");
                
                readonly static string headerSS = Catalog.GetString
                        ("'{0}' is already in the project");
                
                readonly static string messageSS = Catalog.GetString
                        ("You have already imported '{0}' into this project. " + 
                         "Are you sure that you want to import another instance?");
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public AlreadyImportedDialog (Window parent, string file) :
                base (parent, DialogFlags.Modal, MessageType.Question, ButtonsType.None, true,
                      "<big><b>{0}</b></big>\n\n{1}", String.Format (headerSS, file), 
                      String.Format (messageSS, file))
                {
                        AddButton (Stock.Cancel, ResponseType.Cancel);
                        AddButton (skipSS, 100);
                        AddButton (importSS, 200);
                }
                
        }
        
}
