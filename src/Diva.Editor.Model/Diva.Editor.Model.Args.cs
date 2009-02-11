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
        using Core;
        
        // StuffArgs //////////////////////////////////////////////////////////

        public class StuffArgs : EventArgs {
                
                public Stuff Stuff;
                
                /* CONSTRUCTOR */
                public StuffArgs (Stuff stuff)
                {
                        Stuff = stuff;
                }
                
        }
        
        // TagArgs ////////////////////////////////////////////////////////////
        
        public class TagArgs : EventArgs {
                
                public Tag Tag;
                
                /* CONSTRUCTOR */
                public TagArgs (Tag tag)
                {
                        Tag = tag;
                }
                
        }
        
        // ExceptionArgs //////////////////////////////////////////////////////
        
        public class ExceptionArgs : EventArgs {
                
                public Exception Exception;
                
                /* CONSTRUCTOR */
                public ExceptionArgs (Exception exception)
                {
                        Exception = exception;
                }
                
        }
        
        // Requisition args ///////////////////////////////////////////////////

        public class RequisitionArgs : EventArgs {
                
                public bool Requisition;
                
                /* CONSTRUCTOR */
                public RequisitionArgs (bool req)
                {
                        Requisition = req;
                }
                
        }
        
        // OrganizeByMethod args //////////////////////////////////////////////
        
        public class OrganizeByMethodArgs : EventArgs {
                
                public OrganizeByMethod Method;
                
                /* CONSTRUCTOR */
                public OrganizeByMethodArgs (OrganizeByMethod method)
                {
                        Method = method;
                }
                
        }
        
        // Model args /////////////////////////////////////////////////////////
        
        public class GtkModelArgs : EventArgs {
                
                public Gtk.TreeModel Model;

                /* CONSTRUCTOR */
                public GtkModelArgs (Gtk.TreeModel model)
                {
                        Model = model;
                }
                
        }
        
        // Time args ///////////////////////////////////////////////////////////
        
        public class TimeArgs : EventArgs {
                
                public Gdv.Time Time;
                
                /* CONSTRUCTOR */
                public TimeArgs (Gdv.Time time)
                {
                        Time = time;
                }

        }
        
        // TimeSpan args ///////////////////////////////////////////////////////
        
        public class TimeSpanArgs : EventArgs {
                
                public Gdv.TimeSpan TimeSpan;
                
                /* CONSTRUCTOR */
                public TimeSpanArgs (Gdv.TimeSpan timeSpan)
                {
                        TimeSpan = timeSpan;
                }

        }
        
        // ToolSetTool args ////////////////////////////////////////////////////
        
        public class ToolSetToolArgs : EventArgs {
                
                public ToolSetTool Tool;
                
                /* CONSTRUCTOR */
                public ToolSetToolArgs (ToolSetTool tool)
                {
                        Tool = tool;
                }
                
        }
        
        // Progress args ///////////////////////////////////////////////////////
        
        public class ProgressArgs : EventArgs {
                
                public double Progress;
                public string Message;
                public bool PulseOnly;
                
                /* CONSTRUCTOR */
                public ProgressArgs (double progress, string message, bool pulseOnly)
                {
                        Progress = progress;
                        Message = message;
                        PulseOnly = pulseOnly;
                }
                
                /* CONSTRUCTOR */
                public ProgressArgs () : this (0.0, String.Empty, true)
                {
                }
                
        }
        
        // UndoRedoArgs ////////////////////////////////////////////////////////
        
        public class UndoRedoArgs : EventArgs {
                
                public bool CanUndo;
                public bool CanRedo;
                public string UndoMessage;
                public string RedoMessage;
                
                /* CONSTRUCTOR */
                public UndoRedoArgs (bool canUndo, bool canRedo, 
                                     string undoMessage, string redoMessage)
                {
                        CanRedo = canRedo;
                        CanUndo = canUndo;
                        UndoMessage = undoMessage;
                        RedoMessage = redoMessage;
                }
                
                /* CONSTRUCTOR */
                public UndoRedoArgs ()
                {
                }
                 
        }
        
        // Node args ///////////////////////////////////////////////////////////
        
        public class NodeArgs : EventArgs {
                
                public int NodeId;
                
                /* CONSTRUCTOR */
                public NodeArgs (int id)
                {
                        NodeId = id;
                }
                
        }
        
        // Quit args ///////////////////////////////////////////////////////////
        
        public class QuitArgs : EventArgs {
                
                public QuitMode QuitMode;
                
                /* CONSTRUCTOR */
                public QuitArgs (QuitMode mode)
                {
                        QuitMode = mode;
                }

        }
        
        // PipelineState args //////////////////////////////////////////////////
        
        public class PipelineStateArgs : EventArgs {
                
                public bool Playing;
                
                /* CONSTRUCTOR */
                public PipelineStateArgs (bool playing)
                {
                        Playing = playing;
                }
                
        }
        
        // PipelineTicker args //////////////////////////////////////////////////
        
        public class PipelineTickerArgs : EventArgs {
                
                public Gdv.Time Time;
                
                /* CONSTRUCTOR */
                public PipelineTickerArgs (Gdv.Time time)
                {
                        Time = time;
                }
                
        }

        // PipelineTicker args //////////////////////////////////////////////////
        
        public class PipelineErrorArgs : EventArgs {
                
                public string Error;
                
                /* CONSTRUCTOR */
                public PipelineErrorArgs (string error)
                {
                        Error = error;
                }
                
        }
        
        // Rectangle args //////////////////////////////////////////////////////
        
        public class RectangleArgs : EventArgs {
                
                public Gdk.Rectangle Rectangle;
                
                /* CONSTRUCTOR */
                public RectangleArgs (Gdk.Rectangle rect)
                {
                        Rectangle = rect;
                }
                
        }
        
        // Cursor args /////////////////////////////////////////////////////////
        
        public class CursorArgs : EventArgs {
                
                public Gdk.Cursor Cursor;
                
                /* CONSTRUCTOR */
                public CursorArgs (Gdk.Cursor cursor)
                {
                        Cursor = cursor;
                }
                
        }

        // Message args ////////////////////////////////////////////////////////

        public class MessageArgs : EventArgs {

                public string Message;
                public bool Instant;
                
                /* CONSTRUCTOR */
                public MessageArgs (string message, bool instant)
                {
                        Message = message;
                        Instant = instant;
                }

        }

        // SaveNagActionArgs ///////////////////////////////////////////////////

        public class SaveNagActionArgs : EventArgs {

                public SaveNagAction Action;

                /* CONSTRUCTOR */
                public SaveNagActionArgs (SaveNagAction action)
                {
                        Action = action;
                }

        }

        // VolumeArgs //////////////////////////////////////////////////////////
        
        public class PipelineVolumeArgs : EventArgs {

                public double Volume;

                /* CONSTRUCTOR */
                public PipelineVolumeArgs (double volume)
                {
                        Volume = volume;
                }

        }
        
}
       
