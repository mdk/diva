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

namespace Diva.MainMenu {
        
        using System;
        using Gtk;
        using Widgets;
        
        public class SwitchableContainer : VBox, ISwitchableProgress {
                
                // Enums //////////////////////////////////////////////////////
                
                enum SwitchableContainerStatus { Child, ProgressBar };

                // Fields /////////////////////////////////////////////////////
                
                Widget child;                     // The child we're containing
                ProgressBar progressBar;          // The progress bar
                SwitchableContainerStatus status; // What we have currently switched on
                
                // Properties /////////////////////////////////////////////////
                
                public double Progress {
                        set { progressBar.Fraction = value; }
                }
                
                public string Message {
                        set { progressBar.Text = value; }
                }
                
                public Widget Child {
                        get { return child; }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public SwitchableContainer (Widget child)
                {
                        this.child = child;
                        progressBar = new ProgressBar (new Adjustment (0.0, 0.0, 1.0, 0.1, 0.2, 0.5));
                        
                        PackStart (child, true, true, 0);
                        status = SwitchableContainerStatus.Child;
                }
                
                public void SwitchTo ()
                {
                        if (status == SwitchableContainerStatus.ProgressBar)
                                return;
                        
                        Remove (child);
                        PackStart (progressBar, true, true, 0);
                        status = SwitchableContainerStatus.ProgressBar;
                        ShowAll ();
                }
                
                public void SwitchBack ()
                {
                        if (status == SwitchableContainerStatus.Child)
                                return;
                        
                        Remove (progressBar);
                        PackStart (child, true, true, 0);
                        status = SwitchableContainerStatus.Child;
                        ShowAll ();
                }
                
                public void Reset ()
                {
                        progressBar.Fraction = 0.0;
                }
                
                public void Pulse ()
                {
                        progressBar.Pulse ();
                }
                
        }
        
}

                        
                
