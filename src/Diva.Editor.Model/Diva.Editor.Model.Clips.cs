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
        
        public sealed class Clips : IModelPart, IEnumerable {
                
                // Fields //////////////////////////////////////////////////////
                
                Root modelRoot = null;
                
                // Events //////////////////////////////////////////////////////
                
                public event Core.ClipHandler Add;

                public event Core.ClipHandler Remove;
                
                // Properties //////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public int Count {
                        get { return modelRoot.Project.Clips.Count; }
                }

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Clips (Root root)
                {
                        modelRoot = root;
                        
                        // Bind
                        modelRoot.Project.Clips.ClipAdd += OnClipAdded;
                        modelRoot.Project.Clips.ClipRemove += OnClipRemoved;
                }
                
                /* IEnumerable */
                public IEnumerator GetEnumerator () 
                {
                        return modelRoot.Project.Clips.GetEnumerator ();
                }

                /* Get end of the most rightish clip. In other words -- get the
                 * duration of the project */
                public Time GetMaximumOutPoint ()
                {
                        return modelRoot.Project.Clips.GetMaximumOutPoint ();
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnClipAdded (object o, Core.ClipArgs args)
                {
                        if (Add != null)
                                Add (modelRoot, args);
                }

                void OnClipRemoved (object o, Core.ClipArgs args)
                {
                        if (Remove != null)
                                Remove (modelRoot, args);
                }
                                                
        }
        
}
