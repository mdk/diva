
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

/* A small collection of utilility functions to create/read/manage various data
 * storage types (the Gdv ones) and XML nodes */

namespace Diva.Util {

        using System;
        using System.Xml;

        public static class XmlFu {

                // Static methods //////////////////////////////////////////////
                
                public static bool HasNodeChild (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes)
                                if (child.Name == name)
                                        return true;
                        
                        return false;
                }
                
                
                public static int GetIdFromElementRef (XmlNode node)
                {
                        if (node.Name != "refid")
                                throw new Exception ("FIXME: ");
                        
                        return Convert.ToInt32 (node.Attributes ["value"].Value);
                }
                
                public static float GetFloatFromNodeChildren (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes)
                                if (child.Name == name)
                                        return (float) Convert.ToDouble (child.FirstChild.Value);
                        
                        throw new Exception ("FIXME: element not found");
                        
                }
                
                public static int GetIdFromNodeChildren (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes)
                                if (child.Name == name)
                                        return GetIdFromElementRef (child.FirstChild);
                        
                        throw new Exception ("FIXME: element not found");

                }
                
                public static bool GetBoolFromNodeChildren (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes)
                                if (child.Name == name)
                                        return (bool) Convert.ToBoolean (child.FirstChild.Value);
                        
                        throw new Exception ("FIXME: element not found");

                }
                
                public static string GetStringFromNodeChildren (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes) {
                                if (child.Name == name)
                                        return child.FirstChild.Value;
                        }
                        
                        throw new Exception ("FIXME: element not found");
                }
                
                public static Gdv.Color GetColorFromNodeChildren (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes) {
                                if (child.Name == name) {
                                        float r = GetFloatFromNodeChildren ("red", child);
                                        float g = GetFloatFromNodeChildren ("green", child);
                                        float b = GetFloatFromNodeChildren ("blue", child);
                                        float a = GetFloatFromNodeChildren ("alpha", child);
                                        return new Gdv.Color (r, g, b, a);
                                }
                        }
                        
                        throw new Exception ("FIXME: element not found");
                }
                
                public static Gdv.Time GetTimeFromNodeChildren (string name, XmlNode node)
                {
                        foreach (XmlNode child in node.ChildNodes) {
                                if (child.Name == name) 
                                        return Gdv.Time.FromNSeconds (Convert.ToUInt64 (child.FirstChild.Value));
                        }
                        
                        throw new Exception ("FIXME: element not found");
                }
                
                /* Create an element with a refid. 
                 * ie. <videoformat><refid value=323 /></videoformat> */
                public static XmlElement CreateElementRef (XmlDocument document, string name, int id)
                {
                        XmlElement element = document.CreateElement (name);
                        XmlElement refElement = document.CreateElement ("refid");

                        refElement.SetAttribute ("value", id.ToString ());

                        element.AppendChild (refElement);
                        return element;
                }

                /* Create an element with a simple string
                 * ie. <name>This is a name</name> */
                public static XmlElement CreateElement (XmlDocument document, string name, string content)
                {
                        XmlElement stringElement = document.CreateElement (name);
                        XmlNode node = document.CreateTextNode (content);
                        stringElement.AppendChild (node);
                        return stringElement;
                }

                /* Create an element with a Gdv.Color 
                 * ie. <color>#ffffff</color> */
                public static XmlElement CreateElement (XmlDocument document, string name, Gdv.Color color)
                {
                        XmlElement element = document.CreateElement (name);
                        
                        element.AppendChild (CreateElement (document, "red", color.Red));
                        element.AppendChild (CreateElement (document, "green", color.Green));
                        element.AppendChild (CreateElement (document, "blue", color.Blue));
                        element.AppendChild (CreateElement (document, "alpha", color.Alpha));
                
                        return element;
                }

                /* Create an element with a float value 
                 * ie. <aspect>0.4343434</aspect> */
                public static XmlElement CreateElement (XmlDocument document, string name, float val)
                {
                        return CreateElement (document, name, val.ToString ());
                }

                /* Create an element containing a time in nano seconds
                 * ie. <length>54354353453</length> */
                public static XmlElement CreateElement (XmlDocument document, string name, Gdv.Time time)
                {
                        // FIXME: In future we'll add some human-readable stuff as attributes
                        // ie. the SMPTE format
                        return CreateElement (document, name, time.NSeconds.ToString ());
                }
                
                /* Create an element containing a bool value
                 * ie. <length>1</length> */
                public static XmlElement CreateElement (XmlDocument document, string name, bool val)
                {
                        return CreateElement (document, name, val.ToString ());
                }

        }

}


