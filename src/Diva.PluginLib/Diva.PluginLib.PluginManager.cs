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
        using Mono.Unix;
        using System.Collections.Generic;
        using System.Reflection;
        using System.IO;
        using System.Diagnostics;
        using System.Collections;

        public static class PluginManager {

                // Translatable ////////////////////////////////////////////////
                
                static readonly string noPluginDirSS = Catalog.GetString
                        ("Plugin directory not specified");
                
                static readonly string errorInAssembly = Catalog.GetString
                        ("Error while loading plugin assembly {0}");

                // Fields //////////////////////////////////////////////////////
                
                static List <Plugin> plugins;                // Succesfully loaded plugins
                static Dictionary <object, Plugin> privates; // Private hash
                
                // Properties //////////////////////////////////////////////////
 
                public static IEnumerator AllPlugins { 
                        get { return plugins.GetEnumerator (); }
                }

                public static IEnumerator ExportPlugins { 
                        get { return new TypeEnumerator (plugins, typeof (ExportPlugin)); }
                }
                
                // Public methods //////////////////////////////////////////////
                
                public static void Init ()
                {
                        plugins = new List <Plugin> ();
                        privates = new Dictionary <object, Plugin> ();
                                                
                        string pluginDir = Environment.GetEnvironmentVariable ("DIVA_PLUGINS");
                        // FIXME: A different kind of exception
                        if (pluginDir == null || pluginDir == String.Empty)
                                throw new Exception (noPluginDirSS);

                        DirectoryInfo dirInfo = new DirectoryInfo (pluginDir);
                        if (!dirInfo.Exists)
                                return;
                        
                        // Scan each assembly
                        foreach (FileInfo fileInfo in dirInfo.GetFiles ()) {
                                
                                if (fileInfo.Extension != ".dll")
                                        continue;
                                try {
                                        Assembly a = Assembly.LoadFrom (fileInfo.FullName);
                                        ScanAssemblyForPlugins (a);
                                } catch (Exception e) {
                                        throw new Exception (String.Format (errorInAssembly,
                                                                            fileInfo.FullName));
                                }
                        }
                                    
                }

                /* Allows to add a 'private' key object that's related to a given
                 * plugin. Later a plugin can be retrieved using a key. This is used
                 * in the app to assosciate plugins ie. with widgets */
                public static void AddPrivate (Plugin plugin, object o)
                {
                        if (privates.ContainsKey (o))
                                throw new Exception ();
                        else
                                privates [o] = plugin;
                }

                /* Get a plugin for a given private key */
                public static Plugin GetPluginByPrivate (object o)
                {
                        if (! privates.ContainsKey (o))
                                throw new Exception ();
                        else
                                return privates [o];
                }
                
                // Private methods /////////////////////////////////////////////

                private static void ScanAssemblyForPlugins (Assembly assembly)
                {                        
                        foreach (Type t in assembly.GetTypes ()) 
                                if ((t.IsSubclassOf (typeof (Plugin)) && !t.IsAbstract)) {
                                        Plugin plugin = null;
                                        bool res = false;
                                        
                                        try {
                                                plugin = (Plugin) Activator.CreateInstance (t);
                                                plugin.Init ();
                                                res = plugin.Availible ();
                                                if (res)
                                                        plugin.Register ();
                                                
                                        } catch (Exception excp) {
                                                // FIXME: For change
                                                throw (excp);
                                                res = false;
                                        } finally {
                                                if (res == true)
                                                        plugins.Add (plugin);
                                                // else 
                                                // failedPlugins.Add (new FailedPlugin (plugin.Name, failureReason));
                                        }
                                        
                                }
                }
                
        }
        
}
                
                
