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

namespace Diva.Core {

        using System;
        using System.Xml;
        using System.Collections;

        public class ProjectSpoofer {

                // Fields //////////////////////////////////////////////////////

                string length;      // Length of the project
                string name;        // The name of the project
                string fileName;    // The filename
                string directory;   // The base directory
                DateTime lastSaved; // Last save time

                // Properties //////////////////////////////////////////////////

                public string Name {
                        get { return name; }
                }

                public string Directory {
                        get { return directory; }
                }
                
                public string FileName {
                        get { return fileName; }
                }
                
                public string Length {
                        get { return length; }
                }

                public DateTime LastSaved {
                        get { return lastSaved; }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public ProjectSpoofer (string fileName)
                {
                        // Reset our params
                        name = String.Empty;
                        directory = String.Empty;
                        length = String.Empty;
                        lastSaved = DateTime.Now;
                        this.fileName = fileName;
                        
                        XmlTextReader xmlReader = new XmlTextReader (fileName);
                        xmlReader.MoveToContent ();

                        // FIXME: Here we can detect borkage early on
                        
                        xmlReader.MoveToAttribute ("timestamp");
                        lastSaved = DateTime.FromFileTime (Convert.ToInt64 (xmlReader.Value));

                        // Main body read
                        while (xmlReader.Read ()) {
                                
                                if (xmlReader.Name == "projectinfo") {
                                        XmlDocument xmlDocument = new XmlDocument ();
                                        XmlNode xmlNode = xmlDocument.ReadNode (xmlReader);
                                        ResolveNode (xmlNode);
                                }
                        }
                        
                        xmlReader.Close ();
                        
                        // FIXME: Exception if we don't have enough data!
                        
                }
                
                // Private methods ////////////////////////////////////////////
                
                void ResolveNode (XmlNode node)
                {
                        foreach (XmlNode childNode in node.ChildNodes) {
                                
                                switch (childNode.Name) {
                                        
                                        case "name":
                                        name = childNode.FirstChild.Value;
                                        break;
                                        
                                        case "directory":
                                        directory = childNode.FirstChild.Value;
                                        break;

                                        case "length":
                                        length = childNode.FirstChild.Value;
                                        break;
                                }
                        }
                }

        }

}
                
