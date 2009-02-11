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
        using System.Runtime.InteropServices;
        using GLib;

        public class Exporter : Gdv.Exporter {
                
                // Imports /////////////////////////////////////////////////////
                
                [DllImport ("wavexporter")]
                internal static extern IntPtr diva_plugins_wav_exporter_new
                        (IntPtr filePtr);

                [DllImport ("wavexporter")]
                internal static extern IntPtr diva_plugins_wav_exporter_set_format
                        (IntPtr filePtr, IntPtr formatHandle);
                
                // Properties //////////////////////////////////////////////////
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Exporter (IntPtr ptr) : base (ptr)
                {
                }
                
                /* CONSTRUCTOR */
                public Exporter (string fileName) : base (IntPtr.Zero)
                {
                        IntPtr fileNamePtr = Marshaller.StringToPtrGStrdup (fileName);
                        IntPtr raw = diva_plugins_wav_exporter_new (fileNamePtr);
                        Marshaller.Free (fileNamePtr);
                        
                        if (raw == IntPtr.Zero)
                                throw new Exception ();
                        else
                                this.Raw = raw;
                }

                public void SetFormat (Gdv.AudioFormat format)
                {
                        diva_plugins_wav_exporter_set_format (Handle, format.Handle);
                }

        }
        
}