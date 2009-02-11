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
        using System.Collections.Generic;
        using Mono.Unix;
        using System.Collections;
        using Widgets;
        using Basics;
        
        public sealed class Commander : IBoilable {
                
                // Events //////////////////////////////////////////////////////
                
                public event EventHandler StackChange;
                
                // Fields //////////////////////////////////////////////////////

                Stack<IUndoableCommand> undoStack; // The list of commands we can undo
                Stack<IUndoableCommand> redoStack; // The list of commands we can redo
                
                // FIXME: We need to get rid of that (by design)
                Project parentProject;             // The parent we're part of
                
                // Properties //////////////////////////////////////////////////

                /* Can we undo something */
                public bool CanUndo {
                        get { return (undoStack.Count > 0) ? true : false ; }
                }

                /* Can we redo something */
                public bool CanRedo {
                        get { return (redoStack.Count > 0) ? true : false ; }
                }

                /* Operation that will be undone next */
                public string UndoMessage {
                        get { 
                                if (! CanUndo) 
                                        return String.Empty;
                                
                                return undoStack.Peek ().Message;
                        }
                }
                
                /* Project parent */
                public Project Parent {
                        get { return parentProject; }
                        set { parentProject = value; }
                }
                
                /* Operation that will be redone next */
                public string RedoMessage {
                        get { 
                                if (! CanRedo)
                                        return String.Empty;
                                
                                return redoStack.Peek ().Message;
                        }
                }
                
                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public Commander (Project parent)
                {
                        undoStack = new Stack<IUndoableCommand> ();
                        redoStack = new Stack<IUndoableCommand> ();
                        parentProject = parent;
                }

                /* CONSTRUCTOR */
                public Commander (ObjectContainer container, IBoilProvider provider)
                {
                        undoStack = new Stack<IUndoableCommand> ();
                        redoStack = new Stack<IUndoableCommand> ();
                        parentProject = null;

                        List <IUndoableCommand> undoList = new List <IUndoableCommand> ();
                        List <IUndoableCommand> redoList = new List <IUndoableCommand> ();
                        
                        foreach (RefParameter reff in container.FindAllRefs ("undo")) 
                                undoList.Add ((IUndoableCommand) reff.ToObject (provider));

                        foreach (RefParameter reff in container.FindAllRefs ("redo")) 
                                redoList.Add ((IUndoableCommand) reff.ToObject (provider));

                        undoList.Reverse ();
                        redoList.Reverse ();

                        foreach (IUndoableCommand cmd in undoList)
                                InjectUndoCommand (cmd);

                        foreach (IUndoableCommand cmd in redoList)
                                InjectRedoCommand (cmd);
                }
                        
                /* Execute a command immidiately */
                public void Run (ICommand cmd)
                {
                        ExecuteCommand (cmd);
                        
                        if (StackChange != null)
                                StackChange (this, EventArgs.Empty);
                }
                
                /* Undo one command */
                public IUndoableCommand Undo ()
                {
                        if (! CanUndo)
                                throw new CommanderException ("Nothing to undo!");

                        IUndoableCommand cmd = undoStack.Pop ();
                        UndoCommand (cmd);
                        
                        if (StackChange != null)
                                StackChange (this, EventArgs.Empty);

                        return cmd;
                }

                /* Undo many commands */
                public IUndoableCommand UndoMany (int many)
                {
                        if (many <= 0 || many > undoStack.Count)
                                throw new CommanderException ("Trying to undo too many/too few!");

                        IUndoableCommand cmd = null;
                        for (int i = 0; i < many; i++)
                                cmd = Undo ();

                        return cmd;
                }
                                 
                /* Redo one command */
                public IUndoableCommand Redo ()
                {
                        if (! CanRedo)
                                throw new CommanderException ("Nothing to redo!");

                        IUndoableCommand cmd = redoStack.Pop ();
                        RedoCommand (cmd);
                        
                        if (StackChange != null)
                                StackChange (this, EventArgs.Empty);

                        return cmd;
                }

                /* Redo many commands */
                public IUndoableCommand RedoMany (int many)
                {
                        if (many <= 0 || many > redoStack.Count)
                                throw new CommanderException ("Trying to redo too many/too few!");

                        IUndoableCommand cmd = null;
                        for (int i = 0; i < many; i++)
                                cmd = Redo ();

                        return cmd;
                }

                /* Get the enumerator listing all current undoable commands */
                public IEnumerator GetUndoableEnumerator ()
                {
                        return undoStack.GetEnumerator ();
                }

                /* Get the enumerator listing all current redoable commands */
                public IEnumerator GetRedoableEnumerator ()
                {
                        return redoStack.GetEnumerator ();
                }
                
                /* Inject command to the undo stack. DO NOT use manually */
                public void InjectUndoCommand (IUndoableCommand cmd)
                {
                        undoStack.Push (cmd);
                }
                
                /* Inject command to the redo stack. DO NOT use manually */
                public void InjectRedoCommand (IUndoableCommand cmd)
                {
                        redoStack.Push (cmd);
                }

                public List <object> GetDepObjects ()
                {
                        List <object> lst = new List <object> ();
                        foreach (object o in undoStack) 
                                lst.Add (o);

                        foreach (object o in redoStack) 
                                lst.Add (o);

                        return lst;
                }

                public void FlushAll ()
                {
                        FlushUndoStack ();
                        FlushRedoStack ();

                        if (StackChange != null)
                                StackChange (this, EventArgs.Empty);
                }
                
                public void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        foreach (object o in undoStack) 
                                container.Add (new RefParameter ("undo", o, provider));

                        foreach (object o in redoStack) 
                                container.Add (new RefParameter ("redo", o, provider));
                }

                // Private methods /////////////////////////////////////////////

                /* Clear the whole undo stack */
                void FlushUndoStack ()
                {
                        if (undoStack.Count == 0)
                                return;
                        
                        undoStack = new Stack<IUndoableCommand> ();
                }

                /* Clear the whole redo stack */
                void FlushRedoStack ()
                {
                        if (redoStack.Count == 0)
                                return;
                        
                        redoStack = new Stack<IUndoableCommand> ();
                }

                /* Trim the undoStack to fit the capacity.
                 * Returns the amount of commands removed */
                int TrimUndoStack ()
                {
                        /*
                        if (undoStack.Count <= capacity)
                            return 0;

                        int removeCount = undoStack.Count - capacity;

                        for (int i = 0; i < removeCount; i++)
                                undoStack.Dequeue ();

                        return removeCount;
                        */
                        return 0;
                }
                
                bool DoActionStage (ICommand command)
                {
                        try {
                                command.DoAction (parentProject);
                        } catch (Exception excp) {
                                throw CommanderException.DoActionStage (command, excp);
                        }
                        
                        return true;
                }
                
                bool NotUndoableStage (ICommand command)
                {
                        if (! (command is INotUndoableCommand))
                                return true;
                        
                        try {
                                if (command is INotUndoableCommand) {
                                        FlushUndoStack ();
                                        FlushRedoStack ();
                                }
                        } catch (Exception excp) {
                                throw CommanderException.NotUndoableStage (command, excp);
                        }
                        
                        return true;
                }
                
                bool PostUndoableStage (ICommand command)
                {
                        if (! (command is IUndoableCommand))
                                return true;
                        
                        try {
                                if (command is IRepetitiveCommand)
                                        throw new Exception ("Not supported! FIXME!");
                                else {
                                        // A simple undoable command
                                        undoStack.Push (command as IUndoableCommand);
                                        TrimUndoStack ();
                                        FlushRedoStack ();
                                }
                        }
                        catch (Exception excp) {
                                throw CommanderException.PostUndoableStage (command, excp);
                        }
                        
                        return true;
                }
                

                /* Execute the given command */
                void ExecuteCommand (ICommand command)
                {
                        try {
                                CheckIfCommandSane (command);

                                // Try actually doing it
                                if (! DoActionStage (command))
                                        return;

                                // If it's not undoable, flush stacks
                                if (! NotUndoableStage (command))
                                        return;
                                
                                // Do the post-undoable actions
                                if (! PostUndoableStage (command))
                                        return;
                                
                        } catch (Exception excp) {
                                throw CommanderException.CommandExecution (command, excp);
                        }
                }

                /* Undo a given command */
                void UndoCommand (IUndoableCommand cmd)
                {
                        try {
                                cmd.UndoAction (parentProject);
                                redoStack.Push (cmd);
                        } catch (Exception excp) {
                                throw CommanderException.UndoActionStage (cmd, excp);
                        }
                }

                /* Redo a given command */
                void RedoCommand (IUndoableCommand cmd)
                {
                        try {
                                cmd.DoAction (parentProject);
                                undoStack.Push (cmd);
                        } catch (Exception excp) {
                                throw CommanderException.DoActionStage (cmd, excp);
                        }
                }

                /* Check if a given command is sane */
                void CheckIfCommandSane (ICommand command)
                {
                        if (command is IUndoableCommand &&
                            command is INotUndoableCommand)
                                throw CoreException.NewSanity (command);

                        if (command is IPreparableCommand &&
                            command is ITaskPreparableCommand)
                                throw CoreException.NewSanity (command);
                }
                
        }

}

                

