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

        public sealed class MediaItems : IModelPart, IEnumerable {
                
                // Translatable ////////////////////////////////////////////////

                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                
                // Events //////////////////////////////////////////////////////
                
                // Properties //////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public MediaItems (Root root)
                {
                        modelRoot = root;
                }
                
                /* IEnumerable */
                public IEnumerator GetEnumerator () 
                {
                        return modelRoot.Project.MediaItems.GetEnumerator ();
                }
                
                /* Check if we imported the file to the project */
                public bool HasFileName (string file)
                {
                        foreach (MediaItem item in modelRoot.Project.MediaItems) {
                                if (item.BaseSource is IFileBased)
                                        if ((item.BaseSource as IFileBased).BaseFile == file)
                                                return true;
                                if (item is IFileBased)
                                        if ((item as IFileBased).BaseFile == file)
                                                return true;
                        }
                        
                        return false;
                }

                /* Get the number of times the item was used on the timeline */
                public int GetUsageCount (MediaItem item)
                {
                        int c = 0;

                        foreach (Clip clip in modelRoot.Clips)
                                if (clip.ParentItem == item)
                                        c++;

                        return c;
                }
                                                          
                
                // Private methods /////////////////////////////////////////////
                
                                
        }
        
}
