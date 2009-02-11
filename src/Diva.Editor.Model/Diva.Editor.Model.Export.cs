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

namespace Diva.Editor.Model {
        
        using System;
        using System.Collections;
        using Commands;
        using Mono.Unix;
        using Gdv;
        using PluginLib;
        
        public sealed class Export : IModelPart {
                
                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                Time exportMax;
                Time savedPos;
                
                // Properties //////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }

                public Time ExportMax {
                        get { return exportMax; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Export (Root root)
                {
                        modelRoot = root;
                }

                public void Start (Core.ExportSettings settings)
                {
                        exportMax = modelRoot.Clips.GetMaximumOutPoint ();
                        savedPos = modelRoot.Pipeline.CurrentTicker;

                        // Model bind
                        modelRoot.Pipeline.Ticker += OnTicker;
                        
                        modelRoot.Window.Lock ();
                        modelRoot.Window.ShowExportWindow ();
                        
                        // FIXME:
                        modelRoot.Pipeline.StartExporting (settings.GetExporter ());
                        modelRoot.Pipeline.PlayPause ();
                }

                public void Abort ()
                {
                        // Unbind
                        modelRoot.Pipeline.Ticker -= OnTicker;

                        // FIXME:
                        modelRoot.Pipeline.PlayPause ();
                        modelRoot.Pipeline.StopExporting ();
                        modelRoot.Pipeline.Seek (savedPos);

                        modelRoot.Window.UnLock ();
                        modelRoot.Window.HideExportWindow ();
                }
                
                // Private methods /////////////////////////////////////////////

                void OnTicker (object o, Model.PipelineTickerArgs args)
                {
                        if (args.Time >= exportMax)
                                Abort ();
                }
                                                
        }
        
}
