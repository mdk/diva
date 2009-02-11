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

using System;
using System.Runtime.InteropServices;
using GLib;

namespace Gdv {
        
        public enum ItemType { None, AV, Audio, Photo };
        
        public enum SourceType { None, DecodeBin, DvSource, WavSource, SheepSource };
        
        public static class ItemGenerator {
                
                [DllImport ("gdv")]
                internal static extern IntPtr gdv_itemgenerator_from_inspector
                        (IntPtr format, IntPtr inspector);
                
                [DllImport ("gdv")]
                internal static extern int gdv_itemgenerator_get_item_type 
                        (IntPtr ptr);
                
                [DllImport ("gdv")]
                internal static extern int gdv_itemgenerator_get_source_type
                        (IntPtr ptr);

                // Public methods /////////////////////////////////////////////
                
                public static MediaItem FromInspector (ProjectFormat format, Inspector inspector)
                {
                        // FIXME: Error message via GError
                        IntPtr ptr = gdv_itemgenerator_from_inspector (format.Handle, inspector.Handle);
                        if (ptr == IntPtr.Zero)
                                throw new Exception ();
                        else 
                                return AutowrapItem (ptr);
                }
                
                public static MediaItem AutowrapItem (IntPtr item)
                {
                        ItemType type = GetItemType (item);
                        
                        switch (type) {
                                
                                case ItemType.AV:
                                return new AVItem (item);
                                                                
                                case ItemType.Audio:
                                return new AudioItem (item);
                                                               
                                case ItemType.Photo:
                                return new PhotoItem (item);
                                                              
                                default:
                                throw new Exception ();
                                // FIXME: Other exception!
                        }
                }
                
                public static Source AutowrapSource (IntPtr source)
                {
                        SourceType type = GetSourceType (source);

                        switch (type) {
                                
                                case SourceType.DecodeBin:
                                return new DecodeBin (source);
                                
                                case SourceType.SheepSource:
                                return new SheepSource (source);
                                                            
                                default:
                                throw new Exception ();
                                // FIXME: Other exception!
                        }
                }
                
                // Private methods ////////////////////////////////////////////
                
                static ItemType GetItemType (IntPtr item)
                {
                        return (ItemType) gdv_itemgenerator_get_item_type (item);
                }

                static SourceType GetSourceType (IntPtr source)
                {
                        return (SourceType) gdv_itemgenerator_get_source_type (source);
                }
                
        }
        
}
                
