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

namespace Diva.Editor.Gui {
        
        using System;
        using Mono.Unix;
        using Gtk;
        using Core;
        using Widgets;
        using Commands;
        using Util;

        public class Window : Gtk.Window {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string divaSS = Catalog.GetString
                        ("Diva - {0}");
                                
                // Fields /////////////////////////////////////////////////////
                
                VBox totalVBox = null;              // The core container
                VBox mainVBox = null;               // Second-to-total container
                MenuBar menuBar = null;             // The menu bar
                VideoFrame videoFrame = null;       // Video output controller
                StuffVBox stuffVBox = null;         // Stuff container
                TimelineVBox timelineVBox = null;   // Timeline container
                HBox upperHBox = null;              // Horizontal box
                Model.Root modelRoot = null;        // The root of the model
                
                HistoryWindow historyWindow = null; // Window with undo/redo history
                TagsWindow tagsWindow = null;       // Window to edit tags
                ExportWindow exportWindow = null;   // Window showing the export progress

                bool hadPipelineError = false;      // If we had a pipeline error
                                
                // Public methods /////////////////////////////////////////////

                /* CONSTRUCTOR */
                public Window (Model.Root root) : base (String.Format (divaSS, root.ProjectDetails.Name))
                {
                        modelRoot = root;
                        
                        // VideoFrame
                        videoFrame = new VideoFrame (root);
                        
                        // Stuff
                        stuffVBox = new StuffVBox (root);
                        
                        // Top HBox
                        upperHBox = new HBox (false, 6);
                        upperHBox.PackStart (videoFrame, true, true, 0);
                        upperHBox.PackStart (stuffVBox, false, false, 0);
                                                
                        // Timeline
                        timelineVBox = new TimelineVBox (modelRoot);
                        
                        // Main VBox
                        mainVBox = new VBox (false, 12);
                        mainVBox.PackStart (upperHBox, true, true, 0);
                        mainVBox.PackStart (timelineVBox, false, false, 0);
                        mainVBox.BorderWidth = 6;
                        
                        // Menu
                        menuBar = new MenuBar (root);
                                                
                        // Total VBox
                        totalVBox = new VBox (false, 6);
                        totalVBox.PackStart (menuBar, false, false, 0);
                        totalVBox.PackStart (mainVBox, true, true, 0);
                        
                        // Model bind
                        modelRoot.Window.WaitCursorRequest += OnWaitCursorRequest;
                        modelRoot.Window.HistoryWindowRequest += OnHistoryWindowRequest;
                        modelRoot.Window.TagsWindowRequest += OnTagsWindowRequest;
                        modelRoot.Window.AboutWindowRequest += OnAboutWindowRequest;
                        modelRoot.Window.FullscreenRequest += OnFullscreenRequest;
                        modelRoot.Window.LockRequest += OnLockRequest;
                        modelRoot.Window.ExportWindowRequest += OnExportWindowRequest;
                        modelRoot.Pipeline.Error += OnPipelineError;
                        modelRoot.SaveNag += OnSaveNag;
                        
                        // Add it all
                        Add (totalVBox);

                        // Geometry
                        if (Config.Ui.EditorWindowGeometry != "-1")
                                GtkFu.ParseGeometry (this, Config.Ui.EditorWindowGeometry);
                        else
                                Maximize ();
                        
                        ShowAll ();
                }

                public void SaveGeometry ()
                {
                        Config.Ui.EditorWindowGeometry = GtkFu.GetGeometry (this);
                }
                
                // Private methods ////////////////////////////////////////////
                
                /* The model requests the cursor to change shape to/from */
                void OnWaitCursorRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition) 
                                GdkWindow.Cursor = new Gdk.Cursor (Gdk.CursorType.Watch);
                        else
                                GdkWindow.Cursor = null;
                }
                
                /* History window */
                void OnHistoryWindowRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true) 
                                if (historyWindow == null)
                                        historyWindow = new HistoryWindow (modelRoot, this);
                        else
                                historyWindow.Present ();
                        else 
                                if (historyWindow != null) {
                                        historyWindow.SaveGeometry ();
                                        historyWindow.Unbind ();
                                        historyWindow.Destroy ();
                                        historyWindow = null;
                                }
                }
                
                /* Tags window */
                void OnTagsWindowRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true) 
                                if (tagsWindow == null)
                                        tagsWindow = new TagsWindow (modelRoot, this);
                                else
                                        tagsWindow.Present ();
                        else 
                                if (tagsWindow != null) {
                                        tagsWindow.SaveGeometry ();
                                        tagsWindow.Unbind ();
                                        tagsWindow.Destroy ();
                                        tagsWindow = null;
                                }
                }
                
                /* About window */
                void OnAboutWindowRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true) {
                                Dialog dialog = new AboutDialog ();
                                dialog.Run ();
                                dialog.Destroy ();
                        }
                }

                void OnLockRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true) {
                                stuffVBox.Sensitive = false;
                                timelineVBox.Sensitive = false;
                                menuBar.Sensitive = false;
                                videoFrame.Sensitive = false;
                        }

                        if (args.Requisition == false) {
                                stuffVBox.Sensitive = true;
                                timelineVBox.Sensitive = true;
                                menuBar.Sensitive = true;
                                videoFrame.Sensitive = true;
                        }
                }

                void OnExportWindowRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true && exportWindow == null) {
                                exportWindow = new ExportWindow (modelRoot, this);
                                exportWindow.ShowAll ();
                        }

                        if (args.Requisition == false && exportWindow != null) {
                                exportWindow.UnBind ();
                                exportWindow.Destroy ();
                                exportWindow = null;
                        }
                }
                
                void OnFullscreenRequest (object o, Model.RequisitionArgs args)
                {
                        if (args.Requisition == true &&
                            videoFrame.Fullscreen == false) {
                                mainVBox.BorderWidth = 0;
                                
                                stuffVBox.Hide ();
                                timelineVBox.Hide ();
                                menuBar.Hide ();
                                videoFrame.Fullscreen = true;
                                                                
                                Fullscreen ();
                                return;

                                // FIXME: Idle to force re-exposition
                        }

                        if (args.Requisition == false &&
                            videoFrame.Fullscreen == true) {
                                mainVBox.BorderWidth = 6;
                                
                                stuffVBox.ShowAll ();
                                timelineVBox.ShowAll ();
                                menuBar.ShowAll ();
                                videoFrame.Fullscreen = false;
                                
                                Unfullscreen ();
                                return;

                                // FIXME: Idle to force re-exposition
                        }
                }
                
                void OnPipelineError (object o, Model.PipelineErrorArgs args)
                {
                        // We display only first error, not to create a situation where
                        // user is blocked due to message dialog spam
                        
                        if (hadPipelineError)
                                return;

                        hadPipelineError = true;
                        PipelineErrorDialog dialog = new PipelineErrorDialog (this, args.Error);
                        dialog.Run ();
                        dialog.Destroy ();
                }

                protected override bool OnDeleteEvent (Gdk.Event evnt)
                {
                        modelRoot.QuitModel (Model.QuitMode.Complete);
                        return true;
                }

                protected override bool OnKeyPressEvent (Gdk.EventKey evnt)
                {
                        if (evnt.Key == Gdk.Key.Escape &&
                            videoFrame.Fullscreen == true) {
                                modelRoot.Window.StopFullscreen ();
                                return true;
                        } else 
                                return base.OnKeyPressEvent (evnt);
                }

                void OnSaveNag (object o, Model.SaveNagActionArgs args)
                {
                        SaveNagDialog dialog = new SaveNagDialog (this, modelRoot.ProjectDetails.Name);

                        int response = dialog.Run ();
                        args.Action = dialog.ParseResponse (response);
                        dialog.Destroy ();
                }


                
        }
        
}
