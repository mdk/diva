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
        using System.Collections.Generic;
        using Widgets;
        
        public sealed class TaskProcessor : IModelPart {
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;
                Queue <Task> taskQueue = null;
                bool taskRunning = false;
                bool cursorChanged = false;
                bool progressChanged = false;
                
                // Properties /////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public TaskProcessor (Root root)
                {
                        modelRoot = root;
                        taskQueue = new Queue <Task> ();
                        taskRunning = false;
                }
                
                public bool PushTask (Task task)
                {
                        if (taskRunning) {
                                taskQueue.Enqueue (task);
                                return false;
                        } else {
                                task.Running += OnTaskRunning;
                                task.Started += OnTaskStarted;
                                task.Finished += OnTaskFinished;
                                task.Exception += OnTaskException;
                                taskRunning = true;
                                task.Start ();
                                return true;
                        }
                }
                
                // Private methods ////////////////////////////////////////////
                
                void OnTaskStarted (object o, EventArgs args)
                {
                        Task task = o as Task;
                        
                        if (task.SuggestCursor) {
                                modelRoot.Window.RequestWaitCursor (true);
                                cursorChanged = true;
                        }

                        if (task.SuggestProgressBar) {
                                modelRoot.Window.RequestProgressBar (true);
                                progressChanged = true;
                        }
                }
                
                void OnTaskRunning (object o, EventArgs args)
                {
                        Task task = o as Task;

                        if (task.SuggestProgressBar == false)
                                return;
                        
                        if (task is MesurableTask) {
                                MesurableTask mesurableTask = task as MesurableTask;
                                modelRoot.Window.UpdateProgressBar (mesurableTask.Message, 
                                                                    mesurableTask.Progress);
                        } else 
                                modelRoot.Window.PulseProgressBar ();
                }
                                
                void OnTaskFinished (object o, EventArgs args)
                {
                        taskRunning = false;
                        if (taskQueue.Count > 0) 
                                PushTask (taskQueue.Dequeue ());
                        
                        // Undo some cursor/progressbar changes
                        if (cursorChanged)
                                modelRoot.Window.RequestWaitCursor (false);
                        if (progressChanged)
                                modelRoot.Window.RequestProgressBar (false);
                        
                        // FIXME: We should unbind the task here
                        (o as Task).ScheduleCustomFinish ();
                }
                
                void OnTaskException (object o, EventArgs args)
                {
                        (o as Task).Exceptionize ();
                        //modelRoot.Window.PushException ((o as Task).CatchedException);
                }
                
        }
        
}
 
