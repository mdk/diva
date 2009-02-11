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

namespace Diva.Commands {
        
        using System;
        using Widgets;
        using System.Collections.Generic;
        using Core;
        using Gdv;
        using Mono.Unix;

        public class LoaderTask : MesurableTask {

                // Translatable ///////////////////////////////////////////////

                readonly static string loadingSS = Catalog.GetString
                        ("Loading {0}");
                
                // Fields //////////////////////////////////////////////////////
                
                string[] fileNames;             // The filenames we'll load
                List<MediaItem> mediaItemList;  // Items loaded from filenames
                Gdv.ProjectFormat format;       // Our project format to load into
                
                // Properties /////////////////////////////////////////////////
                
                public override bool SuggestCursor {
                        get { return true; }
                }
                
                public override bool SuggestProgressBar {
                        get { return (fileNames.Length > 1) ? true : false ; }
                }
                
                public override string Message {
                        get { return String.Format (loadingSS, 
                                                    System.IO.Path.GetFileName (fileNames [step])); }
                }
                
                public List<MediaItem> MediaItems {
                        get { return mediaItemList; }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public LoaderTask (Gdv.ProjectFormat format, string[] fileNames)
                {
                        this.fileNames = fileNames;
                        this.format = format;
                        maxSteps = (fileNames.Length > 1) ? fileNames.Length - 1 : 1;
                        mediaItemList = new List <MediaItem> ();
                        
                }
                
                // Private methods ////////////////////////////////////////////
                                
                protected override TaskStatus ExecuteStep (int step)
                {
                        if (step >= fileNames.Length)
                                return TaskStatus.Done;
                        
                        LoaderHelper helper = new LoaderHelper (fileNames [step], 
                                                                (fileNames.Length > 1) ? true : false);

                        helper.Done += OnLoaderHelperDone;
                        helper.Run ();
                        return TaskStatus.Blocked;
                }
                
                void OnLoaderHelperDone (object o, LoaderResponseArgs args)
                {
                        LoaderHelper helper = o as LoaderHelper;
                        
                        switch (args.Response) {
                                
                                case LoaderResponse.FailAbort:
                                        Abort ();
                                        break;
                                
                                case LoaderResponse.FailSkip:
                                        break;
                                        
                                default:
                                        MediaItem item = ItemGenerator.FromInspector (format, helper.Inspector);

                                        // Check the frame rate
                                        if (item.HasVideo &&
                                            item.VideoFormat.Fps != format.VideoFormat.Fps) {
                                                bool multi = (fileNames.Length > 1) ? true : false;
                                                WrongFpsDialog dialog = new WrongFpsDialog (multi,
                                                                                            null, helper.FileName,
                                                                                            item.VideoFormat.Fps, format.VideoFormat.Fps);
                                                
                                                int resp = dialog.Run ();
                                                dialog.Destroy ();
                                                
                                                if (multi == true && (LoaderResponse) resp != LoaderResponse.FailSkip)
                                                        Abort ();

                                                if (multi == false)
                                                        Abort ();
                                        } else 
                                                mediaItemList.Add (item);
                                        break;
                        }
                        
                        Ressurect ();
                }
                
        }
        
}
                
                
