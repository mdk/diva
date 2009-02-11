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
        using System.Collections.Generic;
        
        public struct MessageStackEntry {
                
                public int Layer;
                public string Message;
                
        }
        
        public sealed class MessageStack {
                
                // Events //////////////////////////////////////////////////////

                // FIXME: HERE
                public event MessageHandler MessageChange;
                                
                // Fields //////////////////////////////////////////////////////
                
                readonly uint disappearTimeout = 3000; // Mili seconds
                
                Dictionary <long, MessageStackEntry> tokenToEntry = null;
                long currentToken = 0;
                
                long currentTopToken = -1;
                int currentTopLayer = -1;
                string currentMessage = String.Empty;
                
                uint instantTimeout = 0;
                string instantMessage = String.Empty;
                
                // Properties //////////////////////////////////////////////////
                
                public string Message {
                        get { return currentMessage; }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public MessageStack ()
                {
                        currentToken = 0;
                        tokenToEntry = new Dictionary <long, MessageStackEntry> ();
                }

                /* Push an "instant" message that is displayed on the top of other,
                 * and that disappears after some period of time */
                public void PushInstantMessage (string message)
                {
                        // Remove the old timeout, if we have one
                        if (instantTimeout != 0)
                                GLib.Source.Remove (instantTimeout);

                        // Add the disappear timeout for the new one
                        instantTimeout = GLib.Timeout.Add (disappearTimeout, OnInstantMessageTimeout);
                        instantMessage = message;

                        // Emit the message
                        if (MessageChange != null)
                                MessageChange (this, new MessageArgs (message, true));
                }
                
                /* Add one message to the stack, at the specified layer. */
                public long PushMessage (string message, int layer)
                {
                        currentToken++;
                        
                        MessageStackEntry entry;
                        entry.Layer = layer;
                        entry.Message = message;
                        
                        tokenToEntry [currentToken] = entry;
                        
                        // Check if higher
                        if (layer > currentTopLayer) {
                                currentTopLayer = layer;
                                currentTopToken = currentToken;
                                currentMessage = message;

                                // Launch notification event
                                if (MessageChange != null && instantTimeout == 0)
                                        MessageChange (this, new MessageArgs (currentMessage, false));
                        }
                        
                        return currentToken;
                }

                /* Remove a message from the stack */
                public void RemoveMessage (long token)
                {
                        tokenToEntry.Remove (token);
                        
                        if (token == currentTopToken) {
                                // Iterate over all to fund a new top token
                                currentTopToken = -1;
                                currentTopLayer = -1;
                                currentMessage = String.Empty;
                                foreach (long ttoken in tokenToEntry.Keys) {
                                        MessageStackEntry entry = tokenToEntry [ttoken];
                                        if (entry.Layer > currentTopLayer) {
                                                currentTopLayer = entry.Layer;
                                                currentTopToken = ttoken;
                                                currentMessage = entry.Message;
                                        }
                                }

                                // Launch notification event
                                if (MessageChange != null && instantTimeout == 0)
                                        MessageChange (this, new MessageArgs (currentMessage, false));
                        }
                        
                }

                // Private methods /////////////////////////////////////////////

                bool OnInstantMessageTimeout ()
                {
                        instantTimeout = 0;
                        instantMessage = String.Empty;

                        // A comeback to the top msg
                        if (MessageChange != null)
                                MessageChange (this, new MessageArgs (currentMessage, false));
                        
                        return false;
                }

        }
        
}