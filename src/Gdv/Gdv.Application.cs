///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//      Senko Rasic <senko@senko.net>                                        //
//                                                                           //
// Permission is hereby granted, free of charge, to any person obtaining a   //
// copy of this software and associated documentation files (the             //
// "Software"), to deal in the Software without restriction, including       //
// without limitation the rights to use, copy, modify, merge, publish,       //
// distribute, sublicense, and/or sell copies of the Software, and to permit //
// persons to whom the Software is furnished to do so, subject to the        //
// following conditions:                                                     //
//                                                                           //
// The above copyright notice and this permission notice shall be included   //
// in all copies or substantial portions of the Software.                    //
//                                                                           //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

using System;
using System.Runtime.InteropServices;
using GLib;

namespace Gdv {

        public sealed class Application {

                // Imports //////////////////////////////////////////////////

                [DllImport ("gdv")]
                private static extern bool gdv_main_gst_init (ref int argc, ref IntPtr argv, 
                                                              out IntPtr error);

                [DllImport ("gdv")]
                private static extern bool gdv_main_has_mp3 ();
                
                [DllImport ("gdv")]
                private static extern bool gdv_main_has_wav ();

                [DllImport ("gdv")]
                private static extern bool gdv_main_has_dv ();
                
                [DllImport ("gdv")]
                private static extern bool gdv_main_has_ogg ();
                
                [DllImport ("gdv")]
                private static extern bool gdv_main_has_dvcapture ();
                
                [DllImport ("gdv")]
                private static extern bool gdv_main_has_images ();
                
                [DllImport ("gdv")]
                private static extern bool gdv_main_gstreamer_version (out UInt32 major, out UInt32 minor,
                                                                       out UInt32 micro, out UInt32 nano);

                // Properties  //////////////////////////////////////////////
                
                public static bool HasMp3 { get { return gdv_main_has_mp3 (); }}

                public static bool HasWav { get { return gdv_main_has_wav (); }}
                
                public static bool HasDv { get { return gdv_main_has_dv (); }}
                
                public static bool HasOgg { get { return gdv_main_has_ogg (); }}
                
                public static bool HasDvCapture { get { return gdv_main_has_dvcapture (); }}

                public static bool HasImages { get { return gdv_main_has_images (); }}
                
                public static string GStreamerVersion { 
                        get {
                                uint maj, min, mic, nan;
                                GetGStreamerVersion (out maj, out min, out mic, out nan);
                                return String.Format ("{0}.{1}.{2}.{3}", maj, min, mic, nan);
                        }
                }

                // Public methods ///////////////////////////////////////////
                
                public static void Init ()
                {
                        IntPtr errorPtr = IntPtr.Zero;
                        int argc = 0;
                        IntPtr dumbPtr = IntPtr.Zero;
                        gdv_main_gst_init (ref argc, ref dumbPtr, out errorPtr);
                        
                        if (errorPtr != IntPtr.Zero)
                                throw new GException (errorPtr);
                                
                }
                
                public static string[] Init (string appName, string[] args)
                {
                        string[] argsCp = new string[args.Length + 1];
                        argsCp[0] = appName;
                        Array.Copy (args, 0, argsCp, 1, args.Length);
                                                
                        IntPtr errorPtr = IntPtr.Zero;
                        IntPtr argvPtr = Marshaller.ArgvToArrayPtr (argsCp);
                        int argc = argsCp.Length;
                        
                        gdv_main_gst_init (ref argc, ref argvPtr, out errorPtr);
                                                
                        if (errorPtr != IntPtr.Zero)
                                throw new GException (errorPtr);
                                                
                        string[] outArgs = Marshaller.ArrayPtrToArgv (argvPtr, argc);
                        string[] outArgsCp = new string [outArgs.Length - 1];
                        Array.Copy (outArgs, 1, outArgsCp, 0, outArgs.Length - 1);
                        
                        RegisterTypes ();

                        return outArgsCp;
                }
                
                public static void GetGStreamerVersion (out uint major, out uint minor, out uint micro, out uint nano)
                {
                        gdv_main_gstreamer_version (out major, out minor, out micro, out nano);
                }

                // Private methods /////////////////////////////////////////////

                /* Register our custom types */
                static void RegisterTypes ()
                {
                        GType.Register (Color.GType, typeof (Color));
                }

                
                /*

                public static void InitGST (string[] args)
                {
                        IntPtr error = IntPtr.Zero;
                       
                        // FIXME It's already handled in GdvMain.c
                        if (args.Length > 0) {
                                GLib.Argv argv = new GLib.Argv (args, false);
                                if (! gdv_main_gst_init (args.Length, argv.Handle, out error))
                                        throw new GLib.GException (error);
                        } else {
                               if (! gdv_main_gst_init (0, IntPtr.Zero, out error))
                                        throw new GLib.GException (error);
                        }
                         
                        
                }
                
                public static void Init (string[] args)
                {
                        InitGST (args);
                        InitGDV (args);
                }
                
                public static void InitGDV (string[] args)
                {
                        IntPtr error = IntPtr.Zero;
                       
                        // FIXME It's already handled in GdvMain.c
                        if (args.Length > 0) {
                                GLib.Argv argv = new GLib.Argv (args, false);
                                if (! gdv_main_gdv_init (args.Length, argv.Handle, out error))
                                        throw new GLib.GException (error);
                        } else {
                               if (! gdv_main_gdv_init (0, IntPtr.Zero, out error))
                                        throw new GLib.GException (error);
                        }
                         
                        
                }
                
                public static void CheckElements ()
                {
                        IntPtr error = IntPtr.Zero;
                       
                        if (! gdv_main_check_elements (out error))
                                throw new GLib.GException (error);
                }

                public static void InitOil ()
                {
                        gdv_main_oil_init ();
                }
                
                public static CpuInfo GetCpuInfo ()
                {
                        return new CpuInfo (gdv_main_get_cpu_flags ());
                }
                 */

        }

}


