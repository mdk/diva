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
        
        public struct CursorStackEntry {
                
                public int Layer;
                public Gdk.Cursor Cursor;
                
        }
        
        public sealed class CursorStack {
                
                // Events //////////////////////////////////////////////////////
                
                public event CursorHandler CursorChange;
                                
                // Fields //////////////////////////////////////////////////////
                
                Dictionary <long, CursorStackEntry> tokenToEntry = null;
                long currentToken = 0;
                
                long currentTopToken = -1;
                int currentTopLayer = -1;
                Gdk.Cursor currentCursor = null;
                
                // Properties //////////////////////////////////////////////////
                
                public Gdk.Cursor Cursor {
                        get { return currentCursor; }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CursorStack ()
                {
                        currentToken = 0;
                        tokenToEntry = new Dictionary <long, CursorStackEntry> ();
                }
                
                /* Add one cursor to the stack, at the specified layer. It's not
                 * guaranteed that the cursor will be immidiately displayed */
                public long PushCursor (Gdk.Cursor cursor, int layer)
                {
                        currentToken++;
                        
                        CursorStackEntry entry;
                        entry.Layer = layer;
                        entry.Cursor = cursor;
                        
                        tokenToEntry [currentToken] = entry;
                        
                        // Check if higher
                        if (layer > currentTopLayer) {
                                currentTopLayer = layer;
                                currentTopToken = currentToken;
                                currentCursor = cursor;
                                
                                // Launch notification event
                                if (CursorChange != null)
                                        CursorChange (this, new CursorArgs (currentCursor));
                        }
                        
                        return currentToken;
                }
                
                /* Remove a cursor from the cursor stack by token -- previously 
                 * obtained at PushCursor */
                public void RemoveCursor (long token)
                {
                        tokenToEntry.Remove (token);
                        
                        if (token == currentTopToken) {
                                // Iterate over all to fund a new top token
                                currentTopToken = -1;
                                currentTopLayer = -1;
                                currentCursor = null;
                                foreach (long ttoken in tokenToEntry.Keys) {
                                        CursorStackEntry entry = tokenToEntry [ttoken];
                                        if (entry.Layer > currentTopLayer) {
                                                currentTopLayer = entry.Layer;
                                                currentTopToken = ttoken;
                                                currentCursor = entry.Cursor;
                                        }
                                }
                                
                                // Launch notification event
                                if (CursorChange != null)
                                        CursorChange (this, new CursorArgs (currentCursor));
                        }
                        
                }
                
        }
        
}
                
                
        
        
                
