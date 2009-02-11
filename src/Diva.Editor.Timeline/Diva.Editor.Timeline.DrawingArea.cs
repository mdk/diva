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

namespace Diva.Editor.Timeline {

        using System.Collections.Generic;
        using System;
        using Gtk;
        using Gdk;
        using Gdv;
        using Util;
        using Cairo;
        using TimeSpan = Gdv.TimeSpan;
        
        public class DrawingArea : Gtk.DrawingArea {

                // Constant /////////////////////////////////////////////////

                static readonly Gdv.Fraction zoomInRatio = new Fraction (5, 6);
                
                // Fields ///////////////////////////////////////////////////

                ElementList viewElementsList;       // The list with our view elements
                List <Element> toolSpawnedList;     // View elements spawned by the tool
                Graphics currentGraphics;           // The current Cairo graphics
                Model.Root modelRoot = null;        // Model we're bound to
                
                Gdk.Point currentCursorPos;         // The last known position of the cursor
                Tool activeTool = null;             // Currently active tool
                
                bool hasDragIn = false;             // If we're currently handling a drag-in op
                ViewElement dragInElement = null;   // Element we're dragging in

                // Properties ///////////////////////////////////////////////
                
                /* FIXME: TO BE DELETED! */
                public TimeSpan TimeSpan {
                        get { return modelRoot.Timeline.DisplayedTimeSpan; }
                }

                /* No of layers/tracks */
                public int Tracks {
                        get { return 3; }
                }
                
                public Graphics Graphics {
                        get { return currentGraphics; }
                }

                // Public methods ///////////////////////////////////////////
                                
                /* CONSTRUCTOR */
                public DrawingArea (Model.Root root) : base ()
                {
                        modelRoot = root;
                        currentGraphics = null;
                        currentCursorPos = Gdk.Point.Zero;
                        
                        // Fill the element list with default elements
                        SpawnViewElements ();
                        
                        // Start the default/current tool
                        activeTool = null;
                        toolSpawnedList = null;
                        ActivateTool (modelRoot.ToolSet.SelectedTool);

                        // D&D
                        SetupDragAndDrop ();
                                                
                        // Model bind
                        modelRoot.Timeline.DisplayedTimeSpanChange += OnDisplayedTimeSpanChanged;
                        modelRoot.Timeline.ViewportRectangleRequest += OnViewportRectangleRequest;
                        modelRoot.Timeline.InvalidationRequest += OnInvalidationRequest;
                        modelRoot.Timeline.CursorChange += OnCursorChange;
                        modelRoot.ToolSet.SelectedToolChange += OnSelectedToolChanged;
                        modelRoot.Clips.Add += OnClipAdded;
                        modelRoot.Clips.Remove += OnClipRemoved;
                        modelRoot.Window.LockRequest += OnLockRequest;

                        // Events we want to capture
                        AddEvents ((int) Gdk.EventMask.ButtonPressMask);
                        AddEvents ((int) Gdk.EventMask.ButtonReleaseMask);
                        AddEvents ((int) Gdk.EventMask.PointerMotionMask);
                        AddEvents ((int) Gdk.EventMask.LeaveNotifyMask);
                        AddEvents ((int) Gdk.EventMask.EnterNotifyMask);
                }
                
                // Private ////////////////////////////////////////////////////
                
                /* Setup this widget as a D&D target. Part of the construction
                 * process */
                void SetupDragAndDrop ()
                {
                        TargetEntry t = new TargetEntry ("timeline", TargetFlags.App, 0);
                        TargetEntry[] targets = new TargetEntry[1];
                        targets[0] = t;

                        Gtk.Drag.DestSet (this, DestDefaults.All, targets, Gdk.DragAction.Link);
                }
                
                /* Setup all the default elements for this timeline (ie. TrackLines element) */
                void SpawnViewElements ()
                {
                        viewElementsList = new ElementList ();
                        viewElementsList.Add (new TrackLinesElement (modelRoot));
                        viewElementsList.Add (new TimeStripesElement (modelRoot));
                        viewElementsList.Add (new RulerElement (modelRoot));
                        viewElementsList.Add (new CursorElement (modelRoot));
                        viewElementsList.Add (new CursorLineElement (modelRoot));
                        viewElementsList.Add (new LockElement (modelRoot));

                        foreach (Clip clip in modelRoot.Clips)
                                viewElementsList.Add (new ClipTrueElement (modelRoot, clip));

                        foreach (Track track in modelRoot.Tracks)
                                viewElementsList.Add (new TrackElement (modelRoot, track));
                }

                // Protected ///////////////////////////////////////////////////
                
                /* We need to have at least proper height */
                protected override void OnSizeRequested (ref Requisition req)
                {
                        req.Height = 16 + modelRoot.Tracks.Count * 31 + 1;
                }
                
                /* Here we perform all the (re) drawing */
                protected override bool OnExposeEvent (Gdk.EventExpose evnt)
                {
                        currentGraphics = new Graphics (GdkWindow, Style, PangoContext);
                        Rectangle currentRect = evnt.Region.Clipbox;
                        
                        foreach (ViewElement element in viewElementsList) {
                                if (element.DrawingRect != Rectangle.Zero &&
                                    GtkFu.RectanglesIntersect (element.DrawingRect, currentRect)) {
                                        Gdk.Rectangle intersection = GtkFu.GetIntersection (element.DrawingRect,
                                                                                            currentRect);
                                        
                                        element.Draw (currentGraphics, intersection);
                                }
                        }
                        
                        // Free memory
                        currentGraphics.Dispose ();
                        currentGraphics = null;
                                
                        return true;
                }
                
                /* User pressed a button */
                protected override bool OnButtonPressEvent (Gdk.EventButton evnt)
                {
                        if (evnt.Button == 1)
                                activeTool.LeftPress ((int) evnt.X, (int) evnt.Y);
                        if (evnt.Button == 3)
                                activeTool.RightPress ((int) evnt.X, (int) evnt.Y);
                        
                        return false;
                }
                
                /* User pressed a button */
                protected override bool OnButtonReleaseEvent (Gdk.EventButton evnt)
                {
                        if (evnt.Button == 1)
                                activeTool.LeftRelease ((int) evnt.X, (int) evnt.Y);
                        if (evnt.Button == 3)
                                activeTool.RightRelease ((int) evnt.X, (int) evnt.Y);

                        if (modelRoot.CommandProcessor.HasStackedCommand)
                                modelRoot.CommandProcessor.PushStackedCommand ();
                        
                        return false;
                }
                
                protected override bool OnDragMotion (Gdk.DragContext context, int x, int y, uint time)
                {
                        ParsePointer (x, y);
                        
                        // Leave if we're already handling that or this is not a compatible
                        // drag
                        if (! hasDragIn && modelRoot.Drag.Status != Model.DragStatus.Idle) {
                                
                                if (! (modelRoot.Drag.DraggedObject is Core.MediaItemStuff))
                                        goto done;
                                
                                MediaItem item = (modelRoot.Drag.Claim () as Core.MediaItemStuff).MediaItem;
                                
                                Time startTime = modelRoot.Timeline.MouseOverTime;
                                int track = Helper.GetTrackNoForY (modelRoot, y);
                                dragInElement = ClipPretenderElement.NewFromMediaItem (modelRoot, startTime, track, item);
                                viewElementsList.Add (dragInElement);

                                // Let's force the tool to edit
                                if (modelRoot.ToolSet.SelectedTool != Model.ToolSetTool.Edit)
                                        modelRoot.ToolSet.PushTool (Model.ToolSetTool.Edit);
                                
                                hasDragIn = true;
                                (activeTool as EditTool).ForceDragMove (dragInElement, x, y);
                                
                                QueueDraw ();
                        }
                        
                        done:
                        activeTool.Mouse (x, y);
                        return false;
                }
                
                /* This is called when the cursor leaves the timeline area (during drag) AND
                 * when the drag is actually finished */
                protected override void OnDragLeave (Gdk.DragContext context, uint time)
                {
                        if (hasDragIn) {
                                viewElementsList.Remove (dragInElement);
                                                                
                                hasDragIn = false;
                                modelRoot.Drag.UnClaim ();
                                
                                dragInElement = null;
                                (activeTool as EditTool).UnForceDrag ();

                                Redraw ();
                        }
                }
                
                /* It's totally fucking absurdal, that the DragDrop event is emitted AFTER
                 * the DragLeave signal */
                protected override bool OnDragDrop (Gdk.DragContext context, int x, int y, uint time)
                {
                        // FIXME: It's the responsability of the tool to make sure the 
                        // operation is valid
                        
                        activeTool.LeftRelease (x, y);
                        modelRoot.ToolSet.PopTool ();
                                                
                        return false;
                }

                /* User moved a mouse wheel */
                protected override bool OnScrollEvent (Gdk.EventScroll evnt)
                {
                        if (evnt.Direction == ScrollDirection.Up)
                                modelRoot.Timeline.ZoomShifty ((int) evnt.X, zoomInRatio);
                        else if (evnt.Direction == ScrollDirection.Down)
                                modelRoot.Timeline.ZoomShifty ((int) evnt.X, zoomInRatio.Invert ());
                        
                        modelRoot.Timeline.MouseOverTime = modelRoot.Timeline.PixelToTime (currentCursorPos.X);
                                               
                        return true;
                }
                
                /* Mouse is being moved */
                protected override bool OnMotionNotifyEvent (Gdk.EventMotion evnt)
                {
                        ParsePointer ((int) evnt.X, (int) evnt.Y);
                        activeTool.Mouse (currentCursorPos.X, currentCursorPos.Y);
                        
                        return true;
                }
                
                /* Update the last cursor position, send the position to model */
                void ParsePointer (int x, int y)
                {
                        modelRoot.Timeline.MouseOverTime = modelRoot.Timeline.PixelToTime (x);
                        
                        currentCursorPos.X = x;
                        currentCursorPos.Y = y;
                }
                
                /* Mouse left the area */
                protected override bool OnLeaveNotifyEvent (Gdk.EventCrossing evnt)
                {
                        modelRoot.Timeline.MouseOverTime = Time.Empty;
                        currentCursorPos = Gdk.Point.Zero;

                        activeTool.Mouse (-1, -1);
                                                                
                        return false;
                }
                
                /* Mouse entered the area */
                protected override bool OnEnterNotifyEvent (Gdk.EventCrossing evnt)
                {
                        return false;
                }

                protected override bool OnConfigureEvent (Gdk.EventConfigure conf)
                {
                        modelRoot.Timeline.ForceDisplayedTimespanChange ();
                        return true;
                }

                /* Schedlue a complete redraw of the widget */
                void Redraw ()
                {
                        // FIXME: Iterate over all and reset
                        QueueDraw ();
                }
                
                void OnDisplayedTimeSpanChanged (object o, Model.TimeSpanArgs args)
                {
                        Redraw ();
                }
                
                void OnViewportRectangleRequest (object o, Model.RectangleArgs args)
                {
                        if (GdkWindow == null)
                                return;
                        else {
                                int w,h;
                                GdkWindow.GetSize (out w, out h);
                                args.Rectangle = new Rectangle (0, 0, w, h);
                        }
                }
                
                void OnInvalidationRequest (object o, Model.RectangleArgs args)
                {
                        if (GdkWindow != null)
                                GdkWindow.InvalidateRect (args.Rectangle, true);
                }
                
                void OnCursorChange (object o, Model.CursorArgs args)
                {
                        if (GdkWindow != null)
                                GdkWindow.Cursor = args.Cursor;
                }
                
                void OnClipAdded (object o, Core.ClipArgs args)
                {
                        ViewElement newElement = new ClipTrueElement (modelRoot, args.Clip);
                        viewElementsList.Add (newElement);
                        
                        activeTool.NewViewElement (newElement);
                        
                        Redraw ();
                }

                void OnClipRemoved (object o, Core.ClipArgs args)
                {
                        // Find the element
                        ViewElement clipElement = null;

                        foreach (ViewElement element in viewElementsList) {
                                if (element is ClipTrueElement) {
                                        ClipTrueElement el = element as ClipTrueElement;
                                        if (el.Clip == args.Clip) {
                                                clipElement = element;
                                                break;
                                        }
                                }
                        }

                        if (clipElement == null)
                                return;

                        viewElementsList.Remove (clipElement);
                        activeTool.RemovedViewElement (clipElement);
                        Redraw ();
                }

                void OnSelectedToolChanged (object o, Model.ToolSetToolArgs args)
                {
                        ActivateTool (args.Tool);
                }

                void ActivateTool (Model.ToolSetTool tool)
                {
                        // Remove the per-tool view elements
                        if (activeTool != null && toolSpawnedList != null)
                                foreach (Element element in toolSpawnedList)
                                        viewElementsList.Remove (element);
                                         
                        modelRoot.Timeline.ResetCursors ();
                        
                        switch (tool) {
                                
                                case Model.ToolSetTool.Edit:
                                        activeTool = new EditTool (modelRoot);
                                        break;

                                case Model.ToolSetTool.EditChain:
                                        activeTool = new EditChainTool (modelRoot);
                                        break;

                                case Model.ToolSetTool.Cut:
                                        activeTool = new CutTool (modelRoot);
                                        break;

                                case Model.ToolSetTool.Pan:
                                        activeTool = new PanTool (modelRoot);
                                        break;

                                case Model.ToolSetTool.Zoom:
                                        activeTool = new ZoomTool (modelRoot);
                                        break;

                                case Model.ToolSetTool.Opacity:
                                        activeTool = new OpacityTool (modelRoot);
                                        break;
                                        
                                default:
                                        throw new Exception ("Unsupported tool");

                        }
                        
                        toolSpawnedList = activeTool.SpawnCustomElements (viewElementsList);
                        if (toolSpawnedList != null)
                                foreach (Element element in toolSpawnedList)
                                        viewElementsList.Add (element);

                        foreach (ViewElement viewElement in viewElementsList)
                                activeTool.NewViewElement (viewElement);
                        
                        Redraw ();
                }
                
                void OnLockRequest (object o, Model.RequisitionArgs args)
                {
                        Redraw ();
                }
                
        }
        
}
                                            
                
                
                
                
                
