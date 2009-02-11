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

        [TypeAttribute ("object")]
        public sealed class ObjectContainer : ParamedContainer {

                // Fields //////////////////////////////////////////////////////

                int refid;
                string systemtype;
                List <int> depends;

                // Properties //////////////////////////////////////////////////

                public int RefId {
                        get { return refid; }
                        set { refid = value ; }
                }

                public string SystemType {
                        get { return systemtype; }
                }

                public List <int> Depends {
                        get { return depends; }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ObjectContainer (string name, Type systemtype, int refid) :
                        base (name)
                {
                        this.systemtype = systemtype.ToString ();
                        this.refid = refid;
                        this.depends = new List <int> ();
                }

                /* CONSTRUCTOR */
                public ObjectContainer (string name, Type systemtype) :
                        base (name)
                {
                        this.systemtype = systemtype.ToString ();
                        this.refid = -1;
                        this.depends = new List <int> ();
                }

                /* CONSTRUCTOR */
                public ObjectContainer (XmlElement element) :
                        base (element)
                {
                        this.systemtype = element.GetAttribute ("systemtype");
                        this.refid = Convert.ToInt32 (element.GetAttribute ("refid"));
                        this.depends = StringToDeps (element.GetAttribute ("depends"));
                }

                public void AddDep (int dep)
                {
                        if (depends.Contains (dep))
                                return;
                        else
                                depends.Add (dep);
                }

                public void RemoveDep (int dep)
                {
                        if (depends.Contains (dep))
                                depends.Remove (dep);
                }

                public bool HasDep (int dep)
                {
                        return depends.Contains (dep);
                }

                public void FillDeps (List <object> list, IBoilProvider provider)
                {
                        foreach (object o in list)
                                AddDep (provider.GetIdForObject (o));
                }

                // Private methods /////////////////////////////////////////////

                protected override void FillXmlElement (XmlDocument document,
                                                        XmlElement element)
                {
                        base.FillXmlElement (document, element);
                        element.SetAttribute ("refid", refid.ToString ());
                        element.SetAttribute ("systemtype", systemtype);
                        element.SetAttribute ("depends", DepsToString (depends));                           
                }

                string DepsToString (List <int> list)
                {
                        string p = String.Empty;
                        
                        foreach (int dep in list)
                                p = p + dep.ToString () + ",";
                        
                        return p.TrimEnd (',');
                }

                List <int> StringToDeps (string str)
                {
                        List <int> lst = new List <int> ();

                        if (str == String.Empty)
                                return lst;

                        foreach (string dep in str.Split (','))
                                lst.Add (Convert.ToInt32 (dep));

                        return lst;
                }

        }

}