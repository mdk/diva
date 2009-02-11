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
        using Gtk;
        using Util;
        using Mono.Unix;

        public class LoadFailureDialog : Dialog {
                
                // Translatable ///////////////////////////////////////////////

                readonly static string titleSS = Catalog.GetString
                        ("Loading failed");

                readonly static string headerSS = Catalog.GetString
                        ("Failed to load '{0}'");
                
                readonly static string messageSS = Catalog.GetString
                        ("The media file could not be loaded. " + 
                         "It might be the file is broken or you're missing the plugins required to handle it.");
                
                readonly static string detailsSS = Catalog.GetString
                        ("Error details");
                
                readonly static string abortSS = Catalog.GetString
                        ("Abort all");
                
                readonly static string skipSS = Catalog.GetString
                        ("Skip file");
                
                // Fields /////////////////////////////////////////////////////
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public LoadFailureDialog (bool multi, Widget source, string file, string error) :
                base  (titleSS, GtkFu.GetParentForWidget (source), DialogFlags.Modal)
                {
                        HasSeparator = false;
                        Resizable = false;
                        
                        HBox hBox = new HBox (false, 12);
                        VBox leftVBox = new VBox (false, 6);
                        VBox rightVBox = new VBox (false, 12);

                        Image iconImage = new Image (Stock.DialogError, IconSize.Dialog);
                        Label messageLabel = new Label ("");
                        messageLabel.Wrap = true;
                        messageLabel.LineWrap = true;
                        messageLabel.Markup = String.Format ("<big><b>{0}</b></big>\n\n{1}",
                                                             String.Format (headerSS,
                                                                            System.IO.Path.GetFileName (file)),
                                                             messageSS);
                        
                        Expander errorExpander = new Expander (String.Format ("<b>{0}</b>", detailsSS));
                        errorExpander.UseMarkup = true;
                        Label errorLabel = new Label (error);
                        errorLabel.Xalign = 0.0f;
                        errorLabel.Wrap = true;
                        errorLabel.LineWrap = true;
                        errorExpander.Child = errorLabel;
                          
                        leftVBox.PackStart (iconImage, false, false, 0);
                        rightVBox.PackStart (messageLabel, false, false, 0);
                        rightVBox.PackStart (errorExpander, false, false, 0);
                        hBox.PackStart (leftVBox, false, false, 0);
                        hBox.PackStart (rightVBox, true, true, 0);
                        VBox.PackStart (hBox, false, false, 12);
                        
                        if (multi) {
                                AddButton (abortSS, (int) LoaderResponse.FailAbort);
                                AddButton (skipSS, (int) LoaderResponse.FailSkip);
                        } else 
                                AddButton (Stock.Ok, ResponseType.Ok);
                        
                        ShowAll ();
                }
                
        }
        
}
                
                
