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

namespace Diva.Commands {
        
        using System;
        using Gdv;
        using Gtk;
        using Mono.Unix;

        public class LoaderHelper {
                
                // Const //////////////////////////////////////////////////////
                
                static readonly uint timeoutVal = 3000; // Timeout in mili seconds
                
                // Translatable ///////////////////////////////////////////////

                readonly static string timeoutSS = Catalog.GetString
                        ("Operation has timed out");

                // Fields /////////////////////////////////////////////////////

                Inspector inspector;             // Inspector to get the file
                bool multi;                      // If this operations is a part
                                                 // of a multi-load operation
                string fileName;                 // File we're inspecting
                uint timeoutSource;              // The timeout counter
                LoaderResponse result;           // The result of the loading
                Exception exception;             // The cached exception

                // Properties /////////////////////////////////////////////////
                
                public Inspector Inspector {
                        get { return inspector; }
                }

                public string FileName {
                        get { return fileName; }
                }
                
                // Events /////////////////////////////////////////////////////
                
                public event LoaderResponseHandler Done;
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public LoaderHelper (string fileName, bool multi)
                {
                        this.fileName = fileName;
                        this.multi = multi;
                        this.exception = null;
                        timeoutSource = 0;
                        
                        inspector = new Inspector (fileName);
                        inspector.SetDoneFunc (OnInspectorDone);
                        inspector.SetErrorFunc (OnInspectorError);
                }
                
                public void Run ()
                {
                        result = LoaderResponse.FailAbort;
                        timeoutSource = GLib.Timeout.Add (timeoutVal, OnTimeout);
                        inspector.Start ();
                }
                
                // Private methods ////////////////////////////////////////////
                
                void HandleDialogResponse (int response)
                {
                        if (response != (int) LoaderResponse.FailAbort &&
                            response != (int) LoaderResponse.FailSkip &&
                            response != (int) LoaderResponse.Ok)
                                result = LoaderResponse.FailAbort;
                        else
                                result = (LoaderResponse) response;
                }
                                
                bool OnInspectorError ()
                {
                        Gdk.Threads.Enter ();
                        
                        LoadFailureDialog dialog = null;
                        
                        try { 
                                RemoveTimeout ();
                                dialog = new LoadFailureDialog (multi, null, fileName, inspector.Error);
                                
                                HandleDialogResponse (dialog.Run ());
                                if (Done != null)
                                        Done (this, new LoaderResponseArgs (result));
                                
                        } catch (Exception excp) {
                                exception = excp;
                        } finally {
                                if (dialog != null) 
                                        dialog.Destroy ();
                                Gdk.Threads.Leave ();
                        }
                        
                        return false;
                }
                        
                bool OnInspectorDone ()
                {
                        Gdk.Threads.Enter ();
                        RemoveTimeout ();
                        
                        result = LoaderResponse.Ok;
                        
                        if (Done != null)
                                Done (this, new LoaderResponseArgs (result));
                        
                        Gdk.Threads.Leave ();
                        return false;
                }
                
                bool OnTimeout ()
                {
                        Gdk.Threads.Enter ();
                        
                        LoadFailureDialog dialog = null;
                        
                        try {
                                inspector.Abort ();
                                dialog = new LoadFailureDialog (multi, null, fileName, timeoutSS);
                                
                                HandleDialogResponse (dialog.Run ());
                                
                                if (Done != null)
                                        Done (this, new LoaderResponseArgs (result));

                        } catch (Exception excp) {
                                exception = excp;
                        } finally {
                                if (dialog != null) 
                                        dialog.Destroy ();
                                
                                Gdk.Threads.Leave ();
                        }
                        
                        return false;
                }
                
                public void RemoveTimeout ()
                {
                        if (timeoutSource != 0) {
                                GLib.Source.Remove (timeoutSource);
                                timeoutSource = 0;
                        }
                }
                
        }
        
}
                
                
                
                
