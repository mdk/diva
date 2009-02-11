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

using GConf;

namespace Diva.Config {

        using System;
        
        public static class GConf {

                // Fields //////////////////////////////////////////////////////

                static Client client;
                static readonly string basePath = "/apps/diva/";
                
                // Properties //////////////////////////////////////////////////

                public static Client Client {
                        get { return client; }
                }

                // Public methods //////////////////////////////////////////////

                /* Get a client */
                public static void Initialize ()
                {
                        client = new Client ();
                }

                /* Get a string list or 0-length list */
                public static string[] GetStringList (string path)
                {
                        string[] ret;
                        try {
                                ret = Client.Get (basePath + path) as string[];
                                if (ret == null)
                                        ret = new string [0];
                        } catch (Exception e) {
                                ret = new string [0];
                        }
                        
                        return ret;
                }

                /* Set a string list */
                public static void SetStringList (string path, string[] list)
                {
                        try {
                                if (list == null)
                                        list = new string[0];
                                
                                Client.Set (basePath + path, list);
                        } catch (Exception e) {
                        }
                }

                /* Get a string  */
                public static string GetString (string path, string def)
                {
                        string ret;
                        try {
                                ret = Client.Get (basePath + path) as string;
                                if (ret == null)
                                        ret = def;
                        } catch (Exception e) {
                                ret = def;
                        }
                        
                        return ret;
                }

                /* Set a string  */
                public static void SetString (string path, string val)
                {
                        try {
                                if (val == null)
                                        val = String.Empty;
                                
                                Client.Set (basePath + path, val);
                        } catch (Exception e) {
                        }
                }
                
                
        }
        
}
