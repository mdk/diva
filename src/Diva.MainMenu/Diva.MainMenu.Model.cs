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
        
        public sealed class Model {
                
                // Events //////////////////////////////////////////////////////

                public event MenuComponentHandler ComponentChange;
                
                public event VisibilityHandler VisibilityChange;

                public event QuitHandler Quit;
                
                // Fields //////////////////////////////////////////////////////
                
                MenuComponent currentComponent;
                bool visible;
                string[] args;
                Editor.Model.Root editorModel = null;
                Editor.Gui.Window editorWindow = null;
                                
                // Properties //////////////////////////////////////////////////
                
                public MenuComponent CurrentComponent {
                        get { return currentComponent; }
                }
                
                public bool Visible {
                        get { return visible; }
                }
                
                public string[] Args {
                        get { return args; }
                }
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Model (string[] args)
                {
                        currentComponent = MenuComponent.Splash;
                        visible = true;
                        this.args = (args != null) ? args : new string[0];
                }
                
                /* Request a switch to the given component */
                public void SwitchToComponent (MenuComponent component)
                {
                        currentComponent = component;
                        if (ComponentChange != null)
                                ComponentChange (this, new MenuComponentArgs (component));
                }
                
                /* Hide the menu */
                public void Hide ()
                {
                        if (VisibilityChange != null)
                                VisibilityChange (this, new VisibilityArgs (false));
                }
                
                /* Show the menu */
                public void Show ()
                {
                        if (VisibilityChange != null)
                                VisibilityChange (this, new VisibilityArgs (true));
                }
                
                /* Start the editor window */
                public void StartEditor (string name, string dir, Gdv.ProjectFormat format)
                {
                        Core.Project prj = Core.Project.StartNew (name, dir, format);
                        editorModel = new Editor.Model.Root (prj);
                        editorModel.Quit += OnEditorQuit;
                        editorWindow = new Editor.Gui.Window (editorModel);
                        Hide ();
                }
                
                /* Start the editor window */
                public void StartEditor (Core.Project prj)
                {
                        editorModel = new Editor.Model.Root (prj);
                        editorModel.Quit += OnEditorQuit;
                        editorWindow = new Editor.Gui.Window (editorModel);
                        Hide ();
                }
                
                /* Quit the application. Definitely */
                public void QuitApplication (int exitCode)
                {
                        if (Quit != null)
                                Quit (this, new QuitArgs (exitCode));
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnEditorQuit (object o, Editor.Model.QuitArgs args)
                {
                        editorWindow.SaveGeometry ();
                        editorWindow.Destroy ();
                        editorWindow = null;
                        editorModel = null;

                        // Complete quit
                        if (args.QuitMode == Editor.Model.QuitMode.Complete)
                                QuitApplication (0);

                        // Quit to "new project"
                        if (args.QuitMode == Editor.Model.QuitMode.ToNewProject) {
                                SwitchToComponent (MenuComponent.NewProject);
                                Show ();
                        }

                        // Quit to menu
                        if (args.QuitMode == Editor.Model.QuitMode.ToMenu) {
                                SwitchToComponent (MenuComponent.Welcome);
                                Show ();
                        }
                }

        }
        
}
                
        
