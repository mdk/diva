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

namespace Diva.Widgets {
        
        using System;
        using Gtk;

        public abstract class Task {
                
                // Events /////////////////////////////////////////////////////
                
                public event EventHandler Started;
                
                public event EventHandler Running;

                public event EventHandler Finished;
                
                public event EventHandler Exception;
                
                public event EventHandler CustomFinish;
                
                // Fields /////////////////////////////////////////////////////
                
                protected int step = 0;
                protected TaskStatus status = TaskStatus.Zero;
                protected Exception exception = null;
                
                object customData = null;
                
                // Properties /////////////////////////////////////////////////
                
                public Exception CatchedException {
                        get { return exception; }
                }
                
                public TaskStatus Status {
                        get { return status; }
                }
                
                public object CustomData {
                        get { return customData; }
                        set { customData = value; }
                }
                
                public virtual bool SuggestCursor {
                        get { return false; }
                }
                
                public virtual bool SuggestProgressBar {
                        get { return false; }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                public Task ()
                {
                        Reset ();
                }
                
                public void Start ()
                {
                        GLib.Idle.Add (RunIdle);
                }
                
                public virtual void Reset ()
                {
                        step = 0;
                        status = TaskStatus.Zero;
                }
                
                public void Abort ()
                {
                        if (status == TaskStatus.Running)
                                status = TaskStatus.Aborted;
                        else {
                                // We "wake" the task for a second
                                status = TaskStatus.Aborted;
                                GLib.Idle.Add (RunIdle);
                        }
                }
                
                public void Exceptionize ()
                {
                        if (exception  != null)
                                throw exception;
                }
                
                public void ScheduleCustomFinish ()
                {
                        if (CustomFinish != null)
                                GLib.Idle.Add (CustomFinishIdle);
                }
                
                // Private methods ////////////////////////////////////////////
                
                protected void Ressurect ()
                {
                        // FIXME: Not sure if this is correct
                        if (status != TaskStatus.Aborted)
                                status = TaskStatus.Running;
                        
                        step++;
                        GLib.Idle.Add (RunIdle);
                }
                
                bool RunIdle ()
                {
                        bool retval = true;
                                                                    
                        // Main body
                        try {
                                Gdk.Threads.Enter ();
                                
                                switch (status) {

                                        case TaskStatus.Zero:
                                        if (Started != null)
                                                Started (this, EventArgs.Empty);
                                        status = TaskStatus.Running;
                                        break;
                                        
                                        case TaskStatus.Blocked:
                                        retval = false;
                                        break;
                                        
                                        case TaskStatus.Running:
                                        status = ExecuteStep (step);
                                        if (status == TaskStatus.Running)
                                                step++;
                                        else
                                                retval = false;
                                        break;
                                        
                                        default: 
                                        retval = false;
                                        break;
                                }
                        
                        } catch (Exception excp) {
                                exception = excp;
                                retval = false;
                                status = TaskStatus.Exception;
                                if (Exception != null)
                                        Exception (this, EventArgs.Empty);
                        } finally {
                                if (! retval && Finished != null && status != TaskStatus.Blocked)
                                        Finished (this, EventArgs.Empty);
                                else if (Running != null)
                                        Running (this, EventArgs.Empty);
                                Gdk.Threads.Leave ();
                        }
                        
                        return retval;
                }
                
                bool CustomFinishIdle ()
                {
                        try {
                                Gdk.Threads.Enter ();
                                if (CustomFinish != null)
                                        CustomFinish (this, EventArgs.Empty);
                        } catch (Exception excp) {
                                exception = excp;
                                status = TaskStatus.Exception;
                                if (Exception != null)
                                        Exception (this, EventArgs.Empty);
                        } finally {
                                Gdk.Threads.Leave ();
                        } 
                                
                        return false;
                }
                
                protected abstract TaskStatus ExecuteStep (int step);
                
        }
        
}
       
