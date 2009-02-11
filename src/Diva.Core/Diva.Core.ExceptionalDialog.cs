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

namespace Diva.Core {

        using System;
        using Gtk;
        using Mono.Unix;
        using Util;
        using System.IO;

        public class ExceptionalDialog : Gtk.Dialog {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("Fatal error");
                
                readonly static string messageHeaderSS = Catalog.GetString
                        ("Whoops...");

                readonly static string messageBodySS = Catalog.GetString
                        ("An unexpected, fatal error occured. " + 
                         "Details about the failure were outputed to the error console. " + 
                         "You might optionally save the crash log to a file and attach it to a bug report. " + 
                         "\n\n" + 
                         "The application is now most likely in an unstable state. " +
                         "You should save your work as soon as possible");

                readonly static string detailsSS = Catalog.GetString
                        ("Error details");
                
                readonly static string saveLogSS = Catalog.GetString
                        ("Save log");

                // Fields //////////////////////////////////////////////////////

                Exception exception;                 // The exception that triggered the error
                static Gdk.Pixbuf grinPixbuf = null; // The pixbuf we display as a consolation

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public ExceptionalDialog (Exception exception, Gtk.Window parent) : base (titleSS, parent, DialogFlags.Modal)
                {
                        this.exception = exception;
                        
                        BuildGUI ();
                        
                        try { 
                                WriteLog (Console.Error);
                        } catch {
                        }
                }

                // Static methods //////////////////////////////////////////////

                // FIXME: This should be kind of conditional to some compile-time define
                public static void Grab (Exception exception, Widget source)
                {
                        Gtk.Window parent = GtkFu.GetParentForWidget (source);
                                                
                        ExceptionalDialog dialog = new ExceptionalDialog (exception, parent);
                        dialog.Run ();
                        dialog.Destroy ();
                }
                
                // Private methods ////////////////////////////////////////////
                
                public void OnSaveLogClicked (object o, EventArgs args)
                {
                        FileChooserDialog dialog = new FileChooserDialog 
                                (saveLogSS, this, FileChooserAction.Save, 
                                 Stock.Cancel, ResponseType.Cancel, 
                                 Stock.Save, ResponseType.Accept);
                        
                        dialog.SelectMultiple = false;
                        dialog.CurrentName = "crash.log";
                        dialog.DefaultResponse = ResponseType.Accept;
                        
                        if (dialog.Run () == (int) ResponseType.Accept) {
                                // Save it
                                StreamWriter writer = null; 
                                try {
                                        writer = new StreamWriter (dialog.Filename, false);
                                        WriteLog (writer);
                                } catch {
                                } finally {
                                        if (writer != null)
                                                writer.Close ();
                                }
                        }
                        
                        dialog.Destroy ();
                }
                
                void BuildGUI ()
                {
                        HasSeparator = false;
                        Resizable = false;
                        
                        // Containers
                        HBox mainHBox = new HBox (false, 12);
                        VBox rightVBox = new VBox (false, 6);
                        VBox leftVBox = new VBox (false, 6);

                        // Devil pic
                        if (grinPixbuf == null)
                                grinPixbuf = new Gdk.Pixbuf (null, "bug.png");
                        Image grinImage = new Image (grinPixbuf);
                        
                        // Label
                        Label messageLabel = new Label (String.Format ("<big><b>{0}</b></big>\n\n{1}",
                                                                       messageHeaderSS, messageBodySS));
                                                        
                        messageLabel.UseMarkup = true;
                        messageLabel.Wrap = true;
                        messageLabel.LineWrap = true;
                        messageLabel.Selectable = true;
                        
                        // Error expander
                        Expander errorExpander = new Expander (String.Format ("<b>{0}</b>", detailsSS));
                        errorExpander.UseMarkup = true;
                        Label errorLabel = new Label (GenerateExcpString ());
                        errorLabel.Xalign = 0.0f;
                        errorLabel.Wrap = true;
                        errorLabel.LineWrap = true;
                        errorExpander.Child = errorLabel;

                        // Buttons
                        Button saveLogButton = new Button (saveLogSS);
                        saveLogButton.Clicked += OnSaveLogClicked;
                        ActionArea.Add (saveLogButton);
                        AddButton (Stock.Ok, ResponseType.Ok);
                                                                        
                        // Pack it all
                        leftVBox.PackStart (grinImage, false, false, 0);
                        rightVBox.PackStart (messageLabel, true, true, 0);
                        rightVBox.PackEnd (errorExpander, false, false, 0);
                        mainHBox.PackStart (leftVBox, false, false, 0);
                        mainHBox.PackEnd (rightVBox, true, true, 0);
                        VBox.PackStart (mainHBox, false, false, 6);
                        VBox.Spacing = 12;
                        VBox.ShowAll ();
                }
                
                /* Output a nicely formatted message log to the given writer.
                 * We try to be as safe as possible here */
                void WriteLog (TextWriter writer)
                {
                        writer.WriteLine (StringFu.CenteredLine ("CRASH LOG", '=', 78));
                        
                        // Application version
                        string version = String.Empty;
                        try { 
                                version = VersionFu.GetCallingVersion ();
                        } catch {
                                version = "<exception>";
                        } finally {
                                writer.WriteLine (StringFu.ReportLine ("Diva:", version, '.', 78));
                        }
                        
                        // Runtime version 
                        string runtimeVersion = String.Empty;
                        try {
                                runtimeVersion = Environment.Version.ToString ();
                        } catch {
                                runtimeVersion = "<exception>";
                        } finally {
                                writer.WriteLine (StringFu.ReportLine ("Runtime:", runtimeVersion, '.', 78));
                        }

                        // GStreamer version
                        string gstVersion = String.Empty;
                        try {
                                gstVersion = Gdv.Application.GStreamerVersion;
                        } catch {
                                gstVersion = "<exception>";
                        } finally {
                                writer.WriteLine (StringFu.ReportLine ("GStreamer:", gstVersion, '.', 78));
                        }
                        
                        // OS
                        string osVersion = String.Empty;
                        try {
                                osVersion = Environment.OSVersion.ToString ();
                        } catch {
                                osVersion = "<exception>";
                        } finally {
                                writer.WriteLine (StringFu.ReportLine ("OS:", osVersion, '.', 78));
                        }
                        
                        try {
                                Exception excp = exception;
                                while (excp != null) {
                                        string header = String.Format ("\n  [{0} {1}]", excp.GetType (), excp.Message);
                                        writer.WriteLine (StringFu.Wrap (header, 78, 78));
                                        
                                        string[] stack = excp.StackTrace.Split ('\n');
                                        foreach (string stackString in stack)
                                                writer.WriteLine ("* {0}", StringFu.Wrap (stackString, 76, 78));
                                        
                                        excp = excp.InnerException;
                                }
                        } catch {
                        }
                        
                        // Final line
                        writer.WriteLine (StringFu.CenteredLine ("END", '=', 78));
                }
                
                string GenerateExcpString ()
                {
                        string created = String.Empty;
                        
                        Exception excp = exception;
                        while (excp != null) {
                                created = created + String.Format ("{0} ({1})", excp.GetType (), excp.Message);
                                excp = excp.InnerException;
                        }
                        
                        return created;
                }

        }

}
        
