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
        using Mono.Unix;
        using Core;
        using Widgets;

        public class OpenProjectVBox : VBox {

                // Translatable ////////////////////////////////////////////////

                readonly static string recentSS = Catalog.GetString
                        ("Recently edited");

                readonly static string fromDiskSS = Catalog.GetString
                        ("From disk");
                
                readonly static string openProjectSS = Catalog.GetString
                        ("Open project");

                readonly static string divaProjectsSS = Catalog.GetString
                        ("Diva projects");
                
                // Fields //////////////////////////////////////////////////////

                RecentProjectsTreeView treeView; // Here we're storing our recent projects
                SwitchableContainer container;   // Our switchable container
                Model model = null;              // App model
                Button loadButton = null;
                Button backButton = null;
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public OpenProjectVBox (Model model) : base (false, 12)
                {
                        this.model = model;
                        
                        // Label
                        Label recentLabel = new Label (String.Format ("<b>{0}</b>", recentSS));
                        recentLabel.UseMarkup = true;
                        
                        // TreeView
                        treeView = new RecentProjectsTreeView ();
                        treeView.RowActivated += OnRowActivated;
                        
                        // Scrolled window
                        ScrolledWindow scrolledWindow = new ScrolledWindow ();
                        scrolledWindow.ShadowType = ShadowType.In;
                        scrolledWindow.Add (treeView);
                        scrolledWindow.VscrollbarPolicy = PolicyType.Automatic;
                        scrolledWindow.HscrollbarPolicy = PolicyType.Never;

                        // Frame
                        Frame frame = new Frame ();
                        frame.LabelWidget = recentLabel;
                        frame.ShadowType = ShadowType.None;
                        frame.Add (scrolledWindow);
                        
                        // Buttons
                        backButton = new Button (Stock.GoBack);
                        backButton.Clicked += OnBackButtonClicked;
                        loadButton = new Button (fromDiskSS);
                        loadButton.Image = new Image (Stock.Open, IconSize.Button);
                        loadButton.Clicked += OnLoadButtonClicked;
                                                
                        // Button box
                        HButtonBox buttonBox = new HButtonBox ();
                        buttonBox.Add (backButton);
                        buttonBox.Add (loadButton);
                        
                        // Container
                        container = new SwitchableContainer (buttonBox);
                        
                        PackStart (frame, true, true, 0);
                        PackEnd (container, false, false, 0);
                                                
                        StartSpoofer ();
                }

                // Private methods /////////////////////////////////////////////

                void StartSpoofer ()
                {
                        ProjectsSpooferTask task = new ProjectsSpooferTask ();
                        task.ProjectSpoofed += OnProjectSpoofed;
                        task.Finished += OnSpooferTaskFinished;
                        
                        task.Start ();
                }
                
                // Private methods ////////////////////////////////////////////
                
                void OnSpooferTaskFinished (object o, EventArgs args)
                {
                        (o as ProjectsSpooferTask).ProjectSpoofed -= OnProjectSpoofed;
                        (o as ProjectsSpooferTask).Finished -= OnSpooferTaskFinished;
                }
                
                void OnProjectSpoofed (object o, ProjectSpooferArgs args)
                {
                        treeView.AddSpoofer (args.ProjectSpoofer);
                }
                
                public void OnBackButtonClicked (object o, EventArgs args)
                {
                        model.SwitchToComponent (MenuComponent.Welcome);
                }
                
                public void OnLoadButtonClicked (object o, EventArgs args)
                {
                        FileChooserDialog dialog = new FileChooserDialog (openProjectSS, null, FileChooserAction.Open,
                                                                          Stock.Cancel, ResponseType.Cancel,
                                                                          Stock.Ok, ResponseType.Accept);

                        dialog.SelectMultiple = false;

                        // File filter
                        Gtk.FileFilter divFilter = new FileFilter ();
                        divFilter.AddPattern ("*.div"); 
                        divFilter.Name = divaProjectsSS;
                        dialog.AddFilter (divFilter);
                                                                                                
                        int resp = dialog.Run ();

                        if ((ResponseType) resp == ResponseType.Accept) {
                                string fileName = dialog.Filename;
                                dialog.Destroy ();

                                LoadProject (fileName);
                        } else
                                dialog.Destroy ();
                }
                                
                void LoadProject (string fileName)
                {
                        Core.OpenerTask task = new OpenerTask (fileName);
                        container.SwitchTo ();
                                
                        task.Running += OnOpenerTaskRunning;
                        task.Finished += OnOpenerTaskDone;
                        task.Start ();
                }
                
                void OnOpenerTaskRunning (object o, EventArgs args)
                {
                        container.Pulse ();
                }
                
                void OnOpenerTaskDone (object o, EventArgs args)
                {
                        try {
                                OpenerTask task = (o as OpenerTask);
                                task.Exceptionize ();
                                
                                if (task.Status == TaskStatus.Done) {
                                        Core.Project prj = Core.Project.FromOpener (task);
                                        model.StartEditor (prj);
                                }
                                                                        
                        } catch (Exception excp) {
                                ExceptionalDialog.Grab (excp, null);
                        } finally {
                                container.SwitchBack ();
                        }
                }
                
                void OnRowActivated (object o, RowActivatedArgs args)
                {
                        TreeIter iter;
                        (treeView.Model as ListStore).GetIter (out iter, args.Path);
                        string fileName = (string) (treeView.Model as ListStore).GetValue (iter, 3);
                        
                        LoadProject (fileName);
                }
                
        }

}

