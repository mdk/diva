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
        using Widgets;
        using Core;
        using Mono.Unix;
        using Basics;
        using System.Reflection;

        public class InitializerTask : MesurableTask {

                enum Step { Zero, Gdv, Preload, Boilers, GConf, Plugins, Finished }
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string initializingSS =
                        Catalog.GetString ("Initializing application");

                readonly static string gdvSS =
                        Catalog.GetString ("Starting Gdv & GStreamer");
                
                readonly static string preloadSS =
                        Catalog.GetString ("Preloading components");

                readonly static string boilersSS =
                        Catalog.GetString ("Registering boilers");

                readonly static string initializingGConfSS =
                        Catalog.GetString ("Initializing GConf");

                readonly static string pluginsSS =
                        Catalog.GetString ("Loading plugins");

                readonly static string finishingSS =
                        Catalog.GetString ("Finishing");

                // Fields //////////////////////////////////////////////////////

                string[] args;       // Command line arguments we were launched with
                
                // Properties //////////////////////////////////////////////////
                
                public override string Message {
                        get {
                                switch ((Step) step) {
                                        
                                        case Step.Zero:
                                                return initializingSS;
                                                
                                        case Step.Gdv:
                                                return gdvSS;
                                                
                                        case Step.Preload:
                                                return preloadSS;
                                                
                                        case Step.Boilers:
                                                return boilersSS;
                                                
                                        case Step.GConf:
                                                return initializingGConfSS;
                                                
                                        case Step.Plugins:
                                                return pluginsSS;

                                        case Step.Finished:
                                                return finishingSS;

                                        default:
                                                return String.Empty;
                                                break;
                                }
                        }
                }

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public InitializerTask (string[] args)
                {
                        this.args = args;
                        maxSteps = 5;
                }
                
                // Private methods /////////////////////////////////////////////

                protected override TaskStatus ExecuteStep (int s)
                {
                        switch ((Step) s) {
                                        
                                case Step.Gdv:
                                        Gdv.Application.Init ("diva", args);
                                        break;
                                
                                case Step.Preload:
                                        Diva.Widgets.Stub.Init ();
                                        Diva.Util.Stub.Init ();
                                        Diva.Commands.Stub.Init ();
                                        Diva.Core.Stub.Init ();
                                        break;
                                
                                case Step.Boilers:
                                        DataFactory.Initialize ();
                                        BoilFactory.Initialize ();
                                        BoilFactory.ScanAssemblyForBoilers (Assembly.GetAssembly (typeof (Gdv.Boilers.AVItem)));
                                        break;
                                
                                case Step.GConf:
                                        Config.GConf.Initialize ();
                                        break;

                                case Step.Plugins:
                                        PluginLib.PluginManager.Init ();
                                        break;
                                
                                case Step.Finished:
                                        return TaskStatus.Done;

                                default:
                                break;
                        }
                        
                        return TaskStatus.Running;
                }
                
        }

}
