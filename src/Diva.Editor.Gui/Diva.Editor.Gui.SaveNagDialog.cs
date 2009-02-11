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
        using Gtk;
        using Mono.Unix;
        using Util;

        public sealed class SaveNagDialog : Dialog {

                // Enums ///////////////////////////////////////////////////////

                enum Response { Ignore = 1001, Abort = 1002, Save = 1003 }

                // Translatable ////////////////////////////////////////////////
                
                readonly static string messageSS = Catalog.GetString
                        ("The project has been modified since your last save. " +
                         "You'll lose all your modifications if you proceed.");
                
                readonly static string headerSS = Catalog.GetString
                        ("Save '{0}' before quitting?");

                readonly static string closeNoSaveSS = Catalog.GetString
                        ("Close without saving");

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public SaveNagDialog (Widget source, string name) :
                base  (String.Empty, GtkFu.GetParentForWidget (source), DialogFlags.Modal)
                {
                        HasSeparator = false;
                        Resizable = false;
                        
                        HBox hBox = new HBox (false, 12);
                        VBox leftVBox = new VBox (false, 6);
                        VBox rightVBox = new VBox (false, 12);

                        Image iconImage = new Image (Stock.DialogWarning, IconSize.Dialog);
                        Label messageLabel = new Label ("");
                        messageLabel.Wrap = true;
                        messageLabel.LineWrap = true;
                        messageLabel.Markup = String.Format ("<big><b>{0}</b></big>\n\n{1}",
                                                             String.Format (headerSS, name),
                                                             messageSS);
                        
                        leftVBox.PackStart (iconImage, false, false, 0);
                        rightVBox.PackStart (messageLabel, false, false, 0);
                        hBox.PackStart (leftVBox, false, false, 0);
                        hBox.PackStart (rightVBox, true, true, 0);
                        VBox.PackStart (hBox, false, false, 12);
                        
                        AddButton (closeNoSaveSS, (int) Response.Ignore);
                        AddButton (Stock.Cancel, (int) ResponseType.Cancel);
                        AddButton (Stock.Save, (int) Response.Save);
                        
                        ShowAll ();
                }

                public Model.SaveNagAction ParseResponse (int response)
                {
                        switch ((Response) response) {
                                
                                case Response.Save:
                                        return Model.SaveNagAction.Save;

                                case Response.Ignore:
                                        return Model.SaveNagAction.Ignore;

                                default:
                                        return Model.SaveNagAction.Abort;

                        }
                }

        }

}