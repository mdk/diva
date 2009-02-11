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

namespace Diva.Cairo.Test {
        
        using System;
        using Gtk;
        
        public sealed class Window : Gtk.Window {
                
                // Fields //////////////////////////////////////////////////////
                
                Notebook notebook = null;
                VBox vBox = null;
                HBox hBox = null;
                Label statusLabel = null;
                Button benchmarkButton = null;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Window () : base ("Cairo test")
                {
                        // Status label
                        statusLabel = new Label ("");
                        
                        // Benchmark button
                        benchmarkButton = new Button ("Benchmark");
                        benchmarkButton.Clicked += OnBechmarkClicked;
                        
                        // HBox
                        hBox = new HBox (false, 6);
                        hBox.PackStart (statusLabel, true, true, 0);
                        hBox.PackEnd (benchmarkButton, false, false, 0);
                        
                        // Areas
                        GradientsArea gradientsArea = new GradientsArea ();
                        gradientsArea.BenchmarkDone += OnBechmarkDone;
                        
                        SolidsArea solidsArea = new SolidsArea ();
                        solidsArea.BenchmarkDone += OnBechmarkDone;
                        
                        OutlinedSolidsArea outlinedSolidsArea = new OutlinedSolidsArea ();
                        outlinedSolidsArea.BenchmarkDone += OnBechmarkDone;
                        
                        ClipsSimpleArea clipsSimpleArea = new ClipsSimpleArea ();
                        clipsSimpleArea.BenchmarkDone += OnBechmarkDone;
                        
                        GimmicksArea gimmicksArea = new GimmicksArea ();
                        gimmicksArea.BenchmarkDone += OnBechmarkDone;
                                 
                        // Notebook
                        notebook = new Notebook ();
                        notebook.AppendPage (gradientsArea, new Label ("Gradients"));
                        notebook.AppendPage (solidsArea, new Label ("Solids"));
                        notebook.AppendPage (outlinedSolidsArea, new Label ("Outlined solids"));
                        notebook.AppendPage (clipsSimpleArea, new Label ("Simple clips"));
                        notebook.AppendPage (gimmicksArea, new Label ("Gimmicks"));
                        
                        // VBox
                        vBox = new VBox (false, 6);
                        vBox.PackStart (notebook, true, true, 0);
                        vBox.PackEnd (hBox, false, false, 0);
                        
                        // Window
                        Add (vBox);
                        BorderWidth = 6;
                        Resizable = false;
                }
                
                // Private methods /////////////////////////////////////////////
                
                protected override bool OnDeleteEvent (Gdk.Event evnt)
                {
                        Application.Quit ();
                        return true;
                }
                
                void OnBechmarkDone (object o, double result)
                {
                        benchmarkButton.Sensitive = true;
                        statusLabel.Text = String.Format ("{0:f2} redraws per sec", result);
                }
                
                void OnBechmarkClicked (object o, EventArgs args)
                {
                        statusLabel.Text = "Benchmarking...";
                        benchmarkButton.Sensitive = false;
                        (notebook.CurrentPageWidget as DrawingArea).StartBenchmark ();
                }
                
        }
        
}
