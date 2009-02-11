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
        
        // TagApplied args /////////////////////////////////////////////////////
        
        public class AppliedTagArgs : EventArgs {
                
                public Tag Tag;
                public bool Applied;
                // FIXME: Add stuff
                
                /* CONSTRUCTOR */
                public AppliedTagArgs (Tag tag, bool applied)
                {
                        Tag = tag;
                        Applied = applied;
                }
                
        }
                
        // StuffStatus args ////////////////////////////////////////////////////

        public class StuffStatusArgs : EventArgs {
                
                public Stuff Stuff;
                public bool Present;
                
                /* CONSTRUCTOR */
                public StuffStatusArgs (Stuff stuff, bool present)
                {
                        Stuff = stuff;
                        Present = present;
                }
                
        }
        
        // Stuff args //////////////////////////////////////////////////////////

        public class StuffArgs : EventArgs {
                
                public Stuff Stuff;
                
                /* CONSTRUCTOR */
                public StuffArgs (Stuff stuff)
                {
                        Stuff = stuff;
                }
                
        }

        // Tag args ////////////////////////////////////////////////////////////
        
        public class TagArgs : EventArgs {
                
                public Tag Tag;
                
                /* CONSTRUCTOR */
                public TagArgs (Tag tag)
                {
                        Tag = tag;
                }
                
        }
        
        // Tag status args /////////////////////////////////////////////////////
        
        public class TagStatusArgs : EventArgs {
                
                public Tag Tag;
                public bool Present;
                
                /* CONSTRUCTOR */
                public TagStatusArgs (Tag tag, bool present)
                {
                        Present = present;
                        Tag = tag;
                }

        }
        
        // ProjectSpoofer args /////////////////////////////////////////////////
        
        public class ProjectSpooferArgs : EventArgs {
                
                public ProjectSpoofer ProjectSpoofer;
                
                /* CONSTRUCTOR */
                public ProjectSpooferArgs (ProjectSpoofer spoofer)
                {
                        ProjectSpoofer = spoofer;
                }
                
        }
        
        // Clip args ///////////////////////////////////////////////////////////
        
        public class ClipArgs : EventArgs {
                
                public Gdv.Clip Clip;
                
                /* CONSTRUCTOR */
                public ClipArgs (Gdv.Clip clip)
                {
                        Clip = clip;
                }
                
        }
                
}
       
