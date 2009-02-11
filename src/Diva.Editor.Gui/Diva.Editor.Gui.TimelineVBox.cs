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
        using Mono.Unix;
        using Widgets;
        using Timeline;
        using DrawingArea = Timeline.DrawingArea;

        public sealed class TimelineVBox : VBox {
                
                // Translatable ////////////////////////////////////////////////
                
                // Fields /////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                DrawingArea timelineArea = null;
                TimelineHScrollbar timelineScroller = null;
                StatusHBox statusHBox = null;
                Frame timelineFrame = null;
                Table toolTable = null;
                HBox timelineHBox = null;
                VBox toolStubVBox = null;
                VBox rightVBox = null;
                
                ToolToggleButton panTool = null;
                ToolToggleButton zoomTool = null;
                ToolToggleButton cutTool = null;
                ToolToggleButton opacityTool = null;
                ToolToggleButton editTool = null;
                ToolToggleButton editChainTool = null;

                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TimelineVBox (Model.Root modelRoot) : base (false, 6)
                {
                        this.modelRoot = modelRoot;
                        
                        // Timline drawing area 
                        timelineArea = new DrawingArea (modelRoot);
                        
                        // Frame
                        timelineFrame = new Frame ();
                        timelineFrame.Shadow = ShadowType.In;
                        timelineFrame.Add (timelineArea);
                        
                        // Scroller
                        timelineScroller = new TimelineHScrollbar (modelRoot);
                                                
                        // Right vbox
                        rightVBox = new VBox (false, 6);
                        rightVBox.PackStart (timelineFrame, true, true, 0);
                        rightVBox.PackEnd (timelineScroller, false, false, 0);
                                                                        
                        // Tools
                        editTool = new ToolToggleButton (modelRoot, "stock_draw-selection", true, 
                                                         Model.ToolSetTool.Edit);
                        editChainTool = new ToolToggleButton (modelRoot, "stock_draw-line-with-arrows", true,
                                                              Model.ToolSetTool.EditChain);
                        editChainTool.Sensitive = true;
                        
                        panTool = new ToolToggleButton (modelRoot, "stock_zoom-shift", true, 
                                                        Model.ToolSetTool.Pan);
                        zoomTool = new ToolToggleButton (modelRoot, "stock_zoom", true,
                                                         Model.ToolSetTool.Zoom);

                        cutTool = new ToolToggleButton (modelRoot, "stock_cut", true,
                                                        Model.ToolSetTool.Cut);
                        opacityTool = new ToolToggleButton (modelRoot, "stock_brightness", true,
                                                            Model.ToolSetTool.Opacity);
                        opacityTool.Sensitive = true;
                        
                        // Tool table
                        toolTable = new Table (4, 2, false);
                        toolTable.ColumnSpacing = 3;
                        toolTable.RowSpacing = 3;
                        toolTable.Attach (editTool, 0, 1, 0, 1);
                        toolTable.Attach (editChainTool, 1, 2, 0, 1);
                        toolTable.Attach (panTool, 0, 1, 1, 2);
                        toolTable.Attach (zoomTool, 1, 2, 1, 2);
                        toolTable.Attach (cutTool, 0, 1, 2, 3);
                        toolTable.Attach (opacityTool, 1, 2, 2, 3);
                        
                        // Tool table stubby VBox
                        toolStubVBox = new VBox (false, 0);
                        toolStubVBox.PackStart (toolTable, false, false, 0);

                        // HBox 
                        timelineHBox = new HBox (false, 6);
                        timelineHBox.PackStart (toolStubVBox, false, false, 0);
                        timelineHBox.PackStart (rightVBox, true, true, 0);
                        
                        // Status
                        statusHBox = new StatusHBox (modelRoot);
                                                
                        // Main pack
                        PackStart (timelineHBox, true, true, 0);
                        PackEnd (statusHBox, false, false, 0);
                }
                
        }
        
}
       
