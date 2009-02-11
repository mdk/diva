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

namespace Diva.Plugins.Wav {

        using System;
        using Mono.Unix;
        using Gtk;

        public sealed class ExportPlugin : PluginLib.ExportPlugin {

                // Translatable ////////////////////////////////////////////////

                readonly static string menuItemNameSS = Catalog.GetString 
                        ("Export uncompressed sound...");

                readonly static string descriptionSS = Catalog.GetString 
                        ("Allows to export the sound of the composition to a WAV file. " +
                         "The resulting file is a raw, uncompressed file");

                readonly static string nameSS = Catalog.GetString 
                        ("Wav export");

                // Properties //////////////////////////////////////////////////

                public override string MenuItemName {
                        get { return menuItemNameSS; }
                }
                
                public override string Authors {
                        get { return "Michael Dominic K. <michaldominik@gmail.com>"; }
                }

                public override string Description {
                        get { return descriptionSS; }
                }

                public override string Name {
                        get { return nameSS; }
                }

                // Public methods //////////////////////////////////////////////

                public override void Init ()
                {
                }

                public override bool Availible ()
                {
                        return true;
                }

                public override void Register ()
                {
                }

                public override Core.ExportSettings GetExportSettings (Gtk.Window parent,
                                                                       Gdv.ProjectFormat format)
                {
                        ExportSettings settings = null;
                        Dialog dialog = new Dialog (parent, format);

                        if (dialog.Run () == (int) ResponseType.Ok)
                                settings = new ExportSettings (dialog.FileName, dialog.AudioFormat);
                        
                        dialog.Destroy ();

                        return settings;
                }

        }

}
        
        