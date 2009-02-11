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
        using System.Collections;
        using Mono.Unix;
        using Gtk;
        using Widgets;

        public class Window : Gtk.Window, IUnbindable {
                
                // Translatable ////////////////////////////////////////////////
                                
                readonly static string divaSS = Catalog.GetString ("Diva");
                
                // Fields //////////////////////////////////////////////////////
                
                Widget currentWidget = null; // Widget currently displayed
                int width = -1;              // The width of the VBoxes we display (we take from first)
                int height = -1;             // The height of the VBoxes we display (we take from first)
                Model model = null;          // The main menu model
                
                // Properties //////////////////////////////////////////////////
                
                // Public methods //////////////////////////////////////////////
 
                public Window (Model model) : base (divaSS)
                {
                        this.model = model;
                        
                        // Set some window params
                        WindowPosition = Gtk.WindowPosition.Center;
                        Resizable = false;
                        
                        PackComponent (model.CurrentComponent);
                        
                        // Model bind
                        model.VisibilityChange += OnVisibilityChange;
                        model.ComponentChange += OnComponentChange;

                        AllowShrink = false;
                        AllowGrow = true;
                        BorderWidth = 12;
                }
                
                public void Unbind ()
                {
                        model.VisibilityChange -= OnVisibilityChange;
                        model.ComponentChange -= OnComponentChange;
                }
                
                // Private methods ////////////////////////////////////////////
                
                void PackComponent (MenuComponent component)
                {
                        VBox box = null;
                        switch (component) {
                                
                                case MenuComponent.Splash:
                                        box = new SplashVBox (model);
                                        break;
                                
                                case MenuComponent.Welcome:
                                        box = new WelcomeVBox (model);
                                        break;
                                
                                case MenuComponent.NewProject:
                                        box = new NewProjectVBox (model);
                                        break;
                                
                                case MenuComponent.OpenProject:
                                        box = new OpenProjectVBox (model);
                                        break;
                                
                                default:
                                throw new Exception ("Component not supported");
                                
                        }
                      
                        if (currentWidget != null)
                                Remove (currentWidget);
                        
                        Add (box);
                        currentWidget = box;
                        
                        if (model.Visible) {
                                ShowAll ();
                                Present ();
                        }
                }
                
                void OnVisibilityChange (object o, VisibilityArgs args)
                {
                        if (args.Visible) {
                                ShowAll ();
                                Present ();
                        } else
                                HideAll ();
                }
                
                void OnComponentChange (object o, MenuComponentArgs args)
                {
                        PackComponent (args.MenuComponent);
                }
                
                protected override bool OnDeleteEvent (Gdk.Event evnt)
                {
                        model.QuitApplication (0);
                        return true;
                }
                
        }

}


