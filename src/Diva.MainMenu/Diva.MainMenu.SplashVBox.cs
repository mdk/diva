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

/* A VBox that contains the splash screen. Splash png is a compiled-in 
 * resource file
 */

namespace Diva.MainMenu {

        using System;
        using Mono.Unix;
        using Widgets;
        using Gtk;

        public class SplashVBox : VBox {

                // Delegates & events //////////////////////////////////////////

                public delegate void AllTasksDoneHandler (object sender, bool success);
                
                public event AllTasksDoneHandler AllTasksDone;
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string initializingSS = Catalog.GetString 
                        ("Initializing application");

                // Fields //////////////////////////////////////////////////////
                
                Gtk.ProgressBar progressBar; // A progress bar. Here we're displaying the message
                SplashDrawingArea areaArea;  // Area that draws the splash image
                InitializerTask task;        // Initializing task
                Model model;                 // App model
                
                // Public methods //////////////////////////////////////////////
                                
                /* CONSTRUCTOR */
                public SplashVBox (Model model) : base (false, 6)
                {
                        this.model = model;
                                                
                        // Load the splash file
                        Gdk.Pixbuf splashPixbuf = null;
                        splashPixbuf = new Gdk.Pixbuf (null, "Splash.png");
                        // FIXME: Resource exception

                        progressBar = new Gtk.ProgressBar ();
                        Gtk.Frame frameFrame = new Gtk.Frame ();
                        areaArea = new SplashDrawingArea (splashPixbuf);
                        
                        frameFrame.Add (areaArea);
                        PackStart (frameFrame);
                        PackEnd (progressBar);
                        
                        task = new InitializerTask (model.Args);
                        task.Running += OnTaskRunning;
                        task.Finished += OnTaskFinished;
                        task.Start ();
                }

                // Private methods /////////////////////////////////////////////

                void OnTaskRunning (object o, EventArgs args)
                {
                        progressBar.Fraction = task.Progress;
                        progressBar.Text = task.Message;
                }
                
                void OnTaskFinished (object o, EventArgs args)
                {
                        // FIXME: Handle exceptions in a nicer way
                        task.Exceptionize ();
                        
                        model.SwitchToComponent (MenuComponent.Welcome);
                }
                
        }

}
