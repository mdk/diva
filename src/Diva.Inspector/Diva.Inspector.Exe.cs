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

namespace Diva.Inspector {

        using Gdv;
        using System;
        
        public static class Exe {

                // Public methods //////////////////////////////////////////////
        
                /* This is the place where we came in... */
                public static int Main (string[] args)
                {
                        // Banner
                        PrintBanner ();
                
                        // No params
                        if (args.Length == 0) {
                                PrintUsage ();
                                return 128;
                        }
                
                        // --help param
                        if (args[0] == "--help") {
                                Console.WriteLine ("This tool can be used to analyze/inspect media files. \n" +
                                                   "Files are loaded using GStreamer and Gdv interface\n");
                                PrintUsage ();
                                return 128;
                        }
                        
                        // Let's start working!
                        Application.Init ();
                        
                        foreach (string url in args) {

                                try {
                                        Helper helper = new Helper (url);
                                        helper.Start ();
                                        helper.WaitFor ();
                                        helper.Print ();
                                } catch (UserException excp) {
                                        Console.WriteLine ("   Error: {0}", excp.Message);                                                   
                                } finally {
                                        // One new line
                                        Console.WriteLine ("\n");
                                }
                        }
                        
                        return 0;
                }

                // Private methods /////////////////////////////////////////////

                /* Disply usage information */
                static void PrintUsage ()
                {
                        Console.WriteLine ("Usage: diva-inspector <file1> [file2] [...]\n" +
                                           "       diva-inspector --help\n");
                }

                /* Display the initial runtime banner */
                static void PrintBanner ()
                {
                        // Banner
                        Console.WriteLine ("DivaInspector v{0} [{1}]",
                                           Util.VersionFu.GetCallingVersion (), 
                                           Util.VersionFu.GetCallingSlugline ());
                        
                        Console.WriteLine ("Copyright (C) 2005 Michael Dominic K. " +
                                           "and others <michaldominik@gmail.com>");
                        
                        Console.WriteLine ("This free software is licensed under MIT X11 license\n");
                }

        }
}
