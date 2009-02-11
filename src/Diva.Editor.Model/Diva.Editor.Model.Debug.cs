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
        
        public sealed class Debug : IModelPart {
                
                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                bool hasCombSeek = false;
                uint combTimeout = 0;
                bool combForward;
                Time currentPosition = Time.Zero;
                                
                // Properties //////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Debug (Root root)
                {
                        modelRoot = root;
                }

                public void StartCombSeek (uint milisecs)
                {
                        if (hasCombSeek)
                                StopCombSeek ();

                        combTimeout = GLib.Timeout.Add (milisecs, OnCombSeekTimeout);
                        hasCombSeek = true;
                        combForward = true;
                        currentPosition = Time.Zero;

                        OnCombSeekTimeout ();
                }

                public void StopCombSeek ()
                {
                        if (hasCombSeek && combTimeout != 0)
                                GLib.Source.Remove (combTimeout);

                        hasCombSeek = false;
                        combTimeout = 0;
                        currentPosition = Time.Zero;
                }

                // Private methods /////////////////////////////////////////////

                bool OnCombSeekTimeout ()
                {
                        modelRoot.Pipeline.Seek (currentPosition);

                        if (combForward)
                                currentPosition = currentPosition + Time.FromSeconds (0.5);
                        else
                                currentPosition = currentPosition - Time.FromSeconds (0.5);
                        
                        if (currentPosition == Time.Zero)
                                combForward = true;

                        if (currentPosition >= Time.FromSeconds (10.0))
                                combForward = false;

                        return true;
                }
                
        }
        
}