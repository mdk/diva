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
        using System.Collections.Generic;        
        using Util;
        using Widgets;
        using Mono.Unix;
        using Basics;

        public class SaverTask : MesurableTask, IBoilProvider {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string savingSS = Catalog.GetString
                        ("Saving project");
                
                // Enums ///////////////////////////////////////////////////////

                enum Step { StartXml, ProjectDetails, Deps, Boiling, Xmlize, Flush, Finished };
                
                // Fields //////////////////////////////////////////////////////
                
                static readonly string minVersion =
                        "0.0.1";                            // XML output backward compatibility
                
                string fileName;                            // Filename where we're saving
                XmlDocument xmlDocument;                    // Document we're creating
                XmlElement xmlRootElement;                  // Our root element
                XmlElement xmlProjectInfoElement;           // Our info element
                ObjectListContainer objectList;             // The container with the object list
                Project project;                            // The project we're saving
                int currentId;                              // Current ref id

                Dictionary <object, int> objectToId;        // Object -> id
                Dictionary <int, object> idToObject;        // id -> object
                Dictionary <object, string> objectToString; // Object -> canonical root name
                
                // Properties //////////////////////////////////////////////////
                
                public override string Message {
                        get { return savingSS; }
                }
                
                public override bool SuggestProgressBar {
                        get { return true; }
                }
                
                public override bool SuggestCursor {
                        get { return true; }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public SaverTask (Project project)
                {
                        this.fileName = project.FileName;
                        this.project = project;
                        maxSteps = 6;
                }

                public override void Reset ()
                {
                        xmlDocument = new XmlDocument ();
                        objectToId = new Dictionary <object, int> ();
                        idToObject = new Dictionary <int, object> ();
                        objectToString = new Dictionary <object, string> ();
                        currentId = 1;
                        
                        base.Reset ();
                }

                /* IBoilProvider */
                public int GetIdForObject (object o)
                {
                        return objectToId [o];
                }

                /* IBoilProvider */
                public object GetObjectForId (int id)
                {
                        return idToObject [id];
                }

                /* IBoilProvider */
                public bool Contains (int id)
                {
                        return idToObject.ContainsKey (id);
                }
                
                // Private methods /////////////////////////////////////////////

                protected override TaskStatus ExecuteStep (int s)
                {
                        switch ((Step) s) {

                                case Step.StartXml:
                                        CreateXmlDeclaration ();
                                        xmlRootElement = CreateRootElement ();
                                        xmlProjectInfoElement = CreateProjectInfoElement ();                                        
                                        break;
                                        
                                case Step.ProjectDetails:
                                        //xmlProjectInfoElement = CreateProjectInfoElement ();
                                        break;
                                        
                                case Step.Deps:
                                        DepDownObject (project.Tracks, "tracklist");
                                        DepDownObject (project.Tags, "taglist");
                                        DepDownObject (project.Commander, "commander");
                                        DepDownObject (project.Pipeline, "pipeline");
                                        DepDownObject (project.Stuff, "stufflist");
                                        DepDownObject (project.Clips, "cliplist");
                                        DepDownObject (project.MediaItems, "mediaitemlist");
                                        DepDownObject (project.Format, "projectformat");
                                        break;

                                case Step.Boiling:
                                        objectList = new ObjectListContainer ("objectlist");

                                        IEnumerator objectEnumerator;
                                        objectEnumerator = GetObjectsEnumerator ();
                                        while (objectEnumerator.MoveNext ()) {
                                                object currentObject = objectEnumerator.Current;
                                                int refId = GetIdForObject (currentObject);
                                                
                                                string currentName = (objectToString.ContainsKey (currentObject)) ?
                                                        objectToString [currentObject] :
                                                        String.Format ("object_{0}", refId);
                                                
                                                ObjectContainer container =
                                                        Basics.BoilFactory.Boil (currentName, currentObject, this);

                                                container.RefId = refId;
                                                objectList.Add (container);
                                                
                                        } break;
                                        
                                case Step.Xmlize:
                                        xmlRootElement.AppendChild (xmlProjectInfoElement);
                                        xmlRootElement.AppendChild (objectList.ToXmlElement (xmlDocument));
                                        break;
                                                                        
                                case Step.Flush:
                                        xmlDocument.Save (fileName);
                                break;

                                case Step.Finished:
                                        return TaskStatus.Done;

                                default:
                                break;
                        }

                        return TaskStatus.Running;
                }

                void CreateXmlDeclaration ()
                {
                        XmlNode xmlNode = xmlDocument.CreateNode (XmlNodeType.XmlDeclaration, "", "");
                        xmlDocument.AppendChild (xmlNode);
                }
                
                XmlElement CreateRootElement ()
                {
                        XmlElement element = xmlDocument.CreateElement ("divaproject");

                        // Minimal version needed to open the file
                        element.SetAttribute ("minversion", minVersion);
                        
                        // Software version
                        element.SetAttribute ("version", VersionFu.GetCallingVersion ());
                        
                        // Time this project was saved
                        element.SetAttribute ("timestamp", DateTime.Now.ToFileTime ().ToString ());
                        
                        xmlDocument.AppendChild (element);
                        return element;
                }

                XmlElement CreateProjectInfoElement ()
                {
                        // Main
                        XmlElement projectInfoElement = xmlDocument.CreateElement ("projectinfo");
                        
                        // Name
                        XmlElement nameElement = xmlDocument.CreateElement ("name");
                        XmlNode nameNode = xmlDocument.CreateTextNode (project.Name);
                        nameElement.AppendChild (nameNode);

                        // Directory
                        XmlElement directoryElement = xmlDocument.CreateElement ("directory");
                        XmlNode directoryNode = xmlDocument.CreateTextNode (project.Directory);
                        directoryElement.AppendChild (directoryNode);

                        // Length
                        XmlElement lengthElement = xmlDocument.CreateElement ("length");
                        XmlNode lengthNode = xmlDocument.CreateTextNode
                                (TimeFu.ToShortString (project.Clips.GetMaximumOutPoint ()));
                        lengthElement.AppendChild (lengthNode);
                        
                        projectInfoElement.AppendChild (nameElement);
                        projectInfoElement.AppendChild (directoryElement);
                        projectInfoElement.AppendChild (lengthElement);
                        
                        return projectInfoElement;
                }

                /*
                XmlElement CreateRootObjectsElement ()
                {
                        XmlElement element  = xmlDocument.CreateElement ("root");
                        xmlObjectsElement.AppendChild (element);
                        
                        element.AppendChild (XmlFu.CreateElementRef (xmlDocument, "taglist", 
                                                                     boilerStore.GetIdForObject (project.Tags)));
                        element.AppendChild (XmlFu.CreateElementRef (xmlDocument, "stufflist",
                                                                     boilerStore.GetIdForObject (project.Stuff)));
                        element.AppendChild (XmlFu.CreateElementRef (xmlDocument, "commander",
                                                                     boilerStore.GetIdForObject (project.Commander)));
                        element.AppendChild (XmlFu.CreateElementRef (xmlDocument, "pipeline",
                                                                     boilerStore.GetIdForObject (project.Pipeline)));
                        element.AppendChild (XmlFu.CreateElementRef (xmlDocument, "tracklist",
                                                                     boilerStore.GetIdForObject (project.Tracks)));
                        element.AppendChild (XmlFu.CreateElementRef (xmlDocument, "mediaitemlist",
                                                                     boilerStore.GetIdForObject (project.MediaItems)));
                        return element;
                }*/
                
                /* Get the dependent objects. Recursive */
                void DepDownObject (object obj)
                {
                        // Put ourselves here
                        if (! PutInTable (obj))
                                return;
                        
                        // Put references
                        List <object> list = Basics.BoilFactory.GetDepObjects (obj);
                        
                        foreach (object o in list)
                                DepDownObject (o);
                }

                /* As above, but with a name */
                void DepDownObject (object obj, string objectName)
                {
                        DepDownObject (obj);
                        objectToString [obj] = objectName;
                }

                IEnumerator GetObjectsEnumerator ()
                {
                        return idToObject.Values.GetEnumerator ();
                }
                
                bool PutInTable (object o)
                {
                        // Do nothing if we already have this object
                        if (objectToId.ContainsKey (o))
                                return false;
                        
                        // Assign it an id
                        objectToId [o] = currentId;
                        idToObject [currentId] = o;
                        currentId++;
                        return true;
                }

        }

}
