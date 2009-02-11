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

namespace Diva.Exe {

        using System;
        using Mono.Unix;
        using MainMenu;
        
        public static class Diva {
                
                // Fields //////////////////////////////////////////////////////
                
                static int exitCode = 0;
                
                // Public methods //////////////////////////////////////////////

                /* The main routine. Dead simple */
                public static int Main (string[] args)
                {
                        // Banner if version request
                        if (HasVersionRequest (args)) {
                                Console.WriteLine ("Diva v{0} [{1}]", Util.VersionFu.GetCallingVersion (),
                                                   Util.VersionFu.GetCallingSlugline ());
                                Console.WriteLine ("Copyright (C) 2005 Michael Dominic K. " +
                                                   "and others <michaldominik@gmail.com>");
                                Console.WriteLine ("This free software is licensed under MIT X11 license");
                                Console.WriteLine ();
                                
                                return 0;
                        }
                                    
                        // Initialize all the locales
                        string localeDir = Environment.GetEnvironmentVariable ("DIVA_LOCALE");
                        if (localeDir == null)
                                localeDir = "./";
                        Catalog.Init("diva", localeDir);
                        
                        // Basic inits
                        Gdk.Threads.Init ();
                        Gtk.Application.Init ();
                        
                        new Gnome.Program ("Diva", Util.VersionFu.GetCallingVersion (),
                                           Gnome.Modules.UI, args);
                        Util.IconFu.InitDefaultWindowIcon ();
                        
                        Model model = new Model (args);
                        Window mainMenu = new Window (model);
                        
                        model.Quit += OnQuit;
                        
                        Gtk.Application.Run ();
                        
                        mainMenu.Unbind ();
                        mainMenu.Destroy ();
                                                                        
                        return exitCode;
                }
                
                // Private methods /////////////////////////////////////////////
                
                static void OnQuit (object o, QuitArgs args)
                {
                        exitCode = args.ExitCode;
                        Gtk.Application.Quit ();
                }

                /* Check if the parameter string contains a version request */
                static bool HasVersionRequest (string [] args)
                {
                        foreach (string s in args)
                                if (s == "--version" || s == "-v")
                                        return true;

                        return false;
                }
                
        }
                
}
