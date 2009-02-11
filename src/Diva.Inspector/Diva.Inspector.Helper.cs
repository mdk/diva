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

        public sealed class Helper {

                // Enums ///////////////////////////////////////////////////////
                
                enum Result { Success, Exception, Error };
                
                // Fields //////////////////////////////////////////////////////

                string url;
                Inspector inspector;
                Result result;
                Exception exception;
                bool loopRun;
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public Helper (string url)
                {
                        this.url = url;
                        this.inspector = null;
                        exception = null;
                        result = Result.Success;
                        loopRun = false;
                }

                /* Start the inspecting process */
                public void Start ()
                {
                        try {
                                inspector = new Inspector (url);
                                inspector.SetDoneFunc (DoneFunc);
                                inspector.SetErrorFunc (ErrorFunc);
                                inspector.Start ();
                        } catch (Exception e) {
                                exception = e;
                                result = Result.Exception;
                        }
                                                
                 
                }

                /* Wait for the inspecting process to end */
                public void WaitFor ()
                {
                        if (result != Result.Success)
                                return;
                        else {
                                loopRun = true;
                                Gtk.Application.Run ();
                        }
                }

                public void Print ()
                {
                        Console.WriteLine ("{0}", url);

                        // Exception
                        if (result == Result.Exception) {
                                Console.WriteLine ("Exception occurred! ({0})", exception);
                                Console.WriteLine ("{0}", exception.StackTrace);
                        }

                        // Plain error
                        if (result == Result.Error) {
                                Console.WriteLine ("   Error : {0}", inspector.Error);
                        }

                        // Else ok...
                        Console.WriteLine ("   Mime  : {0}",
                                           inspector.Mime);
                        
                        if (inspector.Length != Time.Zero) 
                                Console.WriteLine ("   Length: {0:f2}s ({1}ns)",
                                                   inspector.Length.Seconds,
                                                   inspector.Length.NSeconds);
                        else
                                Console.WriteLine ("   Length: N/A");
                        
                        if (inspector.HasVideo)
                                Console.WriteLine ("   Video : {0}",
                                                   inspector.VideoFormat);
                        else 
                                Console.WriteLine ("   Video : N/A");
                        
                        if (inspector.HasAudio) 
                                Console.WriteLine ("   Audio : {0}",
                                                   inspector.AudioFormat);
                        else 
                                Console.WriteLine ("   Audio : N/A");
                }

                // Private methods /////////////////////////////////////////////

                bool DoneFunc ()
                {
                        result = Result.Success;

                        if (loopRun)
                                Gtk.Application.Quit ();
                        
                        return false;
                }

                bool ErrorFunc ()
                {
                        result = Result.Error;
                        
                        if (loopRun)
                                Gtk.Application.Quit ();

                        return false;
                }

        }

}