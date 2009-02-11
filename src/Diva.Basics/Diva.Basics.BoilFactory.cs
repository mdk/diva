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
        using System.Collections.Generic;
        using System.Reflection;
        
        public static class BoilFactory {

                class ExtBoilInfo {
                        
                        public MethodInfo BoilInfo;
                        public MethodInfo UnBoilInfo;
                        public MethodInfo GetDepObjectsInfo;

                        /* CONSTRUCTOR */
                        public ExtBoilInfo ()
                        {
                                BoilInfo = null;
                                UnBoilInfo = null;
                                GetDepObjectsInfo = null;
                        }

                }
                
                // Fields //////////////////////////////////////////////////////

                static readonly Type[] constructorTypes = new Type[2] { typeof (ObjectContainer),
                                                                        typeof (IBoilProvider) };

                static Dictionary <string, ExtBoilInfo> boilDict = null;
                static Dictionary <string, Type> typesDict = null;
                
                // Public methods //////////////////////////////////////////////

                static public void Initialize ()
                {
                        boilDict = new Dictionary <string, ExtBoilInfo> ();
                        typesDict = new Dictionary <string, Type> ();
                }

                public static List <object> GetList (params object[] list)
                {
                        List <object> outLst = new List <object> ();
                        foreach (object o in list)
                                outLst.Add (o);

                        return outLst;
                }

                public static List <object> GetDepObjects (object o)
                {
                        List <object> lst;
                        
                        if (o is IBoilable) 
                                lst = (o as IBoilable).GetDepObjects ();
                        else {
                                ExtBoilInfo ext = GetExtBoilInfo (o.GetType ().ToString ());
                                if (ext == null)
                                        throw LookupException.NewNotFound (o.GetType ().ToString (),
                                                                           "external boilers");

                                if (ext.GetDepObjectsInfo == null)
                                        throw LookupException.NewIncomplete (o.GetType ().ToString (),
                                                                             "external boilers");

                                Object[] parameters = new Object[1];
                                parameters [0] = o;
                                lst = (List <object>) ext.GetDepObjectsInfo.Invoke (null, parameters);
                        }

                        if (lst == null)
                                lst = new List <object> ();

                        return lst;
                }

                public static ObjectContainer Boil (string name, object o, IBoilProvider provider)
                {
                        ObjectContainer container = new ObjectContainer (name, o.GetType ());
                        // Let's fill in the container deps if we can
                        if (provider != null)
                                container.FillDeps (GetDepObjects (o), provider);
                        
                        if (o is IBoilable) {
                                (o as IBoilable).Boil (container, provider);
                                return container;
                        } else {
                                // Lookup in dictionary
                                ExtBoilInfo ext = GetExtBoilInfo (o.GetType ().ToString ());
                                if (ext == null)
                                        throw LookupException.NewNotFound (o.GetType ().ToString (),
                                                                           "external boilers");

                                if (ext.BoilInfo == null)
                                        throw LookupException.NewIncomplete (container.SystemType,
                                                                             "external boilers");
                                
                                Object[] parameters = new Object[3];
                                parameters [0] = o;
                                parameters [1] = container;
                                parameters [2] = provider;
                                ext.BoilInfo.Invoke (null, parameters);
                                return container;
                        }
                }

                public static object UnBoil (ObjectContainer container, IBoilProvider provider)
                {
                        Type type = GetType (container.SystemType);
                        
                        if (type.GetInterface ("IBoilable") != null) {
                                // Direct constructing
                                // FIXME: Check for null value. Fairly common error
                                object o = type.GetConstructor (constructorTypes).Invoke
                                        (new object [2] {container, provider });
                                return o;
                        } else {
                                // Lookup in dictionary
                                ExtBoilInfo ext = GetExtBoilInfo (container.SystemType);
                                if (ext == null)
                                        throw LookupException.NewNotFound (container.SystemType,
                                                                           "external boilers");

                                if (ext.UnBoilInfo == null)
                                        throw LookupException.NewIncomplete (container.SystemType,
                                                                             "external boilers");

                                Object[] parameters = new Object[2];
                                parameters [0] = container;
                                parameters [1] = provider;
                                return ext.UnBoilInfo.Invoke (null, parameters);
                        }
                }

                /* Scan the passed assembly for boiler functions and automatically register them */
                // FIXME: A bit ugly. Split into few methods
                public static void ScanAssemblyForBoilers (Assembly ass)
                {
                        // For each type...
                        foreach (Type type in ass.GetTypes ()) {
                                // If defines the attribute...
                                if (type.IsDefined (typeof (BoilersAttribute), false)) {
                                        // Search for all methods...
                                        foreach (MethodInfo info in type.GetMethods (BindingFlags.Static | BindingFlags.Public)) {
                                                // If it's a boilfunc attributed method...
                                                if (info.IsDefined (typeof (BoilFuncAttribute), false)) {
                                                        object[] attr = info.GetCustomAttributes (typeof (BoilFuncAttribute), false);
                                                        BoilFuncAttribute ourattr = attr [0] as BoilFuncAttribute;

                                                        // Register it
                                                        ExtBoilInfo ext = GetExtBoilInfoCreate (ourattr.TypeName);
                                                        ext.BoilInfo = info;
                                                }

                                                // If it's a unboilfunc attributed method...
                                                if (info.IsDefined (typeof (UnBoilFuncAttribute), false)) {
                                                        object[] attr = info.GetCustomAttributes (typeof (UnBoilFuncAttribute), false);
                                                        UnBoilFuncAttribute ourattr = attr [0] as UnBoilFuncAttribute;

                                                        // Register it
                                                        ExtBoilInfo ext = GetExtBoilInfoCreate (ourattr.TypeName);
                                                        ext.UnBoilInfo = info;
                                                }

                                                // If it's a boilfunc attributed method...
                                                if (info.IsDefined (typeof (GetDepObjectsFuncAttribute), false)) {
                                                        object[] attr = info.GetCustomAttributes (typeof (GetDepObjectsFuncAttribute), false);
                                                        GetDepObjectsFuncAttribute ourattr = attr [0] as GetDepObjectsFuncAttribute;

                                                        // Register it
                                                        ExtBoilInfo ext = GetExtBoilInfoCreate (ourattr.TypeName);
                                                        ext.GetDepObjectsInfo = info;
                                                }
                                        }
                                }
                        }
                }

                // Private methods /////////////////////////////////////////////

                static ExtBoilInfo GetExtBoilInfo (string type)
                {
                        if (boilDict.ContainsKey (type))
                                return boilDict [type];
                        else
                                return null;
                }

                static ExtBoilInfo GetExtBoilInfoCreate (string type)
                {
                        ExtBoilInfo ext = GetExtBoilInfo (type);
                        if (ext != null)
                                return ext;
                        else {
                                ext = new ExtBoilInfo ();
                                boilDict [type] = ext;
                                return ext;
                        }
                }

                static Type GetType (string type)
                {
                        if (typesDict.ContainsKey (type))
                                return typesDict [type];

                        foreach (Assembly ass in AppDomain.CurrentDomain.GetAssemblies ()) {
                                Type t = ass.GetType (type);
                                if (t != null) {
                                        typesDict [type] = t;
                                        return t;
                                }
                        }

                        throw LookupException.NewNotFound (type,
                                                           "loaded modules");
                }

        }

}