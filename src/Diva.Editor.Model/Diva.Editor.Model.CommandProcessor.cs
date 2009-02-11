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
        using Widgets;
        using System.Collections.Generic;
        using System.Collections;
        using Mono.Unix;
        
        public sealed class CommandProcessor : IModelPart {

                // Translatable ///////////////////////////////////////////////
                
                readonly static string undoneSS = Catalog.GetString
                        ("Undone: {0}");

                readonly static string redoneSS = Catalog.GetString
                        ("Redone: {0}");
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;
                Core.ICommand stackedCommand = null;
                
                // Events //////////////////////////////////////////////////////
                
                public event UndoRedoHandler UndoRedoChange;
                                
                // Properties /////////////////////////////////////////////////

                public bool HasStackedCommand {
                        get { if (stackedCommand != null)
                                        return true;
                                else
                                        return false;
                        }
                }
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public List <string> UndoStackMessages {
                        get {
                                IEnumerator enumerator = modelRoot.Project.Commander.GetUndoableEnumerator ();
                                List <string> lst = new List <string> ();
                                while (enumerator.MoveNext ()) {
                                        Core.ICommand cmd = enumerator.Current as Core.ICommand;
                                        lst.Add (cmd.Message);
                                }
                                
                                return lst;
                        }
                }
                
                public List <string> RedoStackMessages {
                        get {
                                IEnumerator enumerator = modelRoot.Project.Commander.GetRedoableEnumerator ();
                                List <string> lst = new List <string> ();
                                while (enumerator.MoveNext ()) {
                                        Core.ICommand cmd = enumerator.Current as Core.ICommand;
                                        lst.Add (cmd.Message);
                                }
                                
                                return lst;
                        }
                }
                
                public bool CanUndo {
                        get { return modelRoot.Project.Commander.CanUndo; }
                }
                
                public bool CanRedo {
                        get { return modelRoot.Project.Commander.CanRedo; }
                }
                
                public string UndoMessage {
                        get { return modelRoot.Project.Commander.UndoMessage; }
                }
                
                public string RedoMessage {
                        get { return modelRoot.Project.Commander.RedoMessage; }
                }
                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public CommandProcessor (Root root)
                {
                        modelRoot = root;
                        root.Project.Commander.StackChange += OnUndoRedoStackChange;
                }

                public void StackCommand (Core.ICommand cmd)
                {
                        if (stackedCommand != null)
                                throw new Exception ("Already has stacked command!");

                        stackedCommand = cmd;
                }

                public void PushStackedCommand ()
                {
                        PushCommand (stackedCommand);
                        stackedCommand = null;
                }
                                
                public void PushCommand (Core.ICommand cmd)
                {
                        modelRoot.Project.NeedsSave = true;
                        
                        try {
                                if (cmd is Core.IPreparableCommand)
                                        (cmd as Core.IPreparableCommand).Prepare (modelRoot.Project);
                                
                                // Execute imidiatelly
                                if (! (cmd is Core.ITaskPreparableCommand)) {
                                        modelRoot.Project.Commander.Run (cmd);
                                } else {
                                        Task t = (cmd as Core.ITaskPreparableCommand).Prepare (modelRoot.Project);
                                        t.CustomData = cmd;
                                        t.CustomFinish += OnCommandTaskFinish;
                                        modelRoot.TaskProcessor.PushTask (t);
                                        return;
                                }
                                
                        } catch (Exception excp) {
                                modelRoot.Window.PushException (excp);
                        }

                        if (cmd is Core.IMessagingCommand)
                                modelRoot.Window.PushInstantMessage ((cmd as Core.IMessagingCommand).InstantMessage);
                }
                
                public void Undo ()
                {
                        Core.IUndoableCommand cmd = modelRoot.Project.Commander.Undo ();
                        modelRoot.Window.PushInstantMessage (String.Format (undoneSS, cmd.Message));

                        modelRoot.Project.NeedsSave = true;
                }
                
                public void Redo ()
                {
                        Core.IUndoableCommand cmd = modelRoot.Project.Commander.Redo ();
                        modelRoot.Window.PushInstantMessage (String.Format (redoneSS, cmd.Message));

                        modelRoot.Project.NeedsSave = true;
                }
                
                public void UndoMany (int i)
                {
                        Core.IUndoableCommand cmd = modelRoot.Project.Commander.UndoMany (i);
                        modelRoot.Window.PushInstantMessage (String.Format (undoneSS, cmd.Message));

                        modelRoot.Project.NeedsSave = true;
                }
                
                public void RedoMany (int i)
                {
                        Core.IUndoableCommand cmd = modelRoot.Project.Commander.RedoMany (i);
                        modelRoot.Window.PushInstantMessage (String.Format (redoneSS, cmd.Message));

                        modelRoot.Project.NeedsSave = true;
                }

                public void WipeHistory ()
                {
                        modelRoot.Project.Commander.FlushAll ();
                        modelRoot.Project.NeedsSave = true;
                }

                // Private methods ////////////////////////////////////////////
                
                public void OnCommandTaskFinish (object o, EventArgs args)
                {
                        Task task = o as Task;
                        Core.ICommand cmd = (Core.ICommand) task.CustomData;
                                                
                        if (task.Status == TaskStatus.Done) {
                                // Execute the post-prepare func
                                (cmd as Core.ITaskPreparableCommand).PostPrepare (modelRoot.Project);
                                
                                modelRoot.Project.Commander.Run (cmd);
                                if (cmd is Core.IMessagingCommand)
                                        modelRoot.Window.PushInstantMessage ((cmd as Core.IMessagingCommand).InstantMessage);
                        }
                }
                
                public void OnUndoRedoStackChange (object o, EventArgs eventArgs)
                {
                        if (UndoRedoChange == null)
                                return;
                        
                        UndoRedoArgs args = new UndoRedoArgs ();
                        
                        if (modelRoot.Project.Commander.CanUndo) {
                                args.CanUndo = true;
                                args.UndoMessage = modelRoot.Project.Commander.UndoMessage;
                        } else {
                                args.CanUndo = false;
                                args.UndoMessage = String.Empty;
                        }
                        
                        if (modelRoot.Project.Commander.CanRedo) {
                                args.CanRedo = true;
                                args.RedoMessage = modelRoot.Project.Commander.RedoMessage;
                        } else {
                                args.CanRedo = false;
                                args.RedoMessage = String.Empty;
                        }
                        
                        // Emission
                        UndoRedoChange (modelRoot, args);
                }
                
        }
        
}
 
