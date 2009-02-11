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

namespace Diva.Editor.Gui {
        
        using System;
        using Gtk;
        using Widgets;
        
        public sealed class TogglesHBox : HBox {
                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                SmallishToggleButton framesToggle = null;
                SmallishToggleButton snapToggle = null;
                SmallishToggleButton markersToggle = null;
                VolumeButton volumeButton = null;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TogglesHBox (Model.Root root) : base (false, 3)
                {
                        modelRoot = root;
                        
                        framesToggle = new SmallishToggleButton 
                                ("stock_insert-video-plugin", IconSize.Menu, false);
                        snapToggle = new SmallishToggleButton
                                ("stock_snap-grid", IconSize.Menu, false);
                        markersToggle = new SmallishToggleButton
                                ("stock_interaction", IconSize.Menu, false);
                        volumeButton = new VolumeButton
                                (root);

                        PackStart (framesToggle, true, true, 0);
                        PackStart (snapToggle, true, true, 0);
                        PackStart (markersToggle, true, true, 0);
                        PackStart (volumeButton, true, true, 0);
                }
                
        }
        
}
       
