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

namespace Diva.Editor.Timeline {

        using System;
        using Gtk;
        using Mono.Unix;
        using Gdv;
        
        public class OpacityMenu : Menu {
                
                // Translatable /////////////////////////////////////////////
                
                readonly static string technoSS = Catalog.GetString 
                        ("Techno mode");
                
                // Fields ///////////////////////////////////////////////////
                
                Track track;          // Track we were spawned for
                Model.Root modelRoot; // App model

                MenuItem twoFive;
                MenuItem fiveZero;
                MenuItem sevenFive;
                MenuItem oneZeroZero;
                
                // Public methods ///////////////////////////////////////////
                
                /* Constructor */
                public OpacityMenu (Model.Root root, Track track)
                {
                        modelRoot = root;
                        this.track = track;

                        twoFive = new MenuItem ("25%");
                        twoFive.Activated += OnPercentActivated;
                        Append (twoFive);

                        fiveZero = new MenuItem ("50%");
                        fiveZero.Activated += OnPercentActivated;
                        Append (fiveZero);

                        sevenFive = new MenuItem ("75%");
                        sevenFive.Activated += OnPercentActivated;
                        Append (sevenFive);

                        oneZeroZero = new MenuItem ("100%");
                        oneZeroZero.Activated += OnPercentActivated;
                        Append (oneZeroZero);

                        // Separator
                        Append (new SeparatorMenuItem ());

                        // Techno mode
                        MenuItem technoModeItem = new MenuItem ("Techno mode");
                        technoModeItem.Activated += OnTechnoModeActivated;
                        Append (technoModeItem);
                }
                        
                
                // Private methods ////////////////////////////////////////////

                public void OnPercentActivated (object sender, EventArgs args)
                {
                        double opacity;
                        MenuItem item = sender as MenuItem;
                        
                        if (item == twoFive)
                                opacity = 0.25;
                        else if (item == fiveZero)
                                opacity = 0.50;
                        else if (item == sevenFive)
                                opacity = 0.75;
                        else
                                opacity = 1.0;

                        Core.ICommand cmd = new Commands.ChangeOpacity (track, opacity);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }

                public void OnTechnoModeActivated (object sender, EventArgs args)
                {
                        Core.ICommand cmd = new Commands.ChangeOpacity (track, -1.0);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }

        }
        
}