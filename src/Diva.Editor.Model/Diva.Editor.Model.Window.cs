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
        
        public sealed class Window : IModelPart {
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;
                MessageStack messageStack;
                bool locked; 
                
                // Events /////////////////////////////////////////////////////
                
                public event ExceptionHandler InternalException;
                
                public event RequisitionHandler WaitCursorRequest;
                
                public event RequisitionHandler ProgressBarRequest;
                
                public event ProgressHandler UpdateProgress;
                
                public event RequisitionHandler HistoryWindowRequest;
                
                public event RequisitionHandler TagsWindowRequest;

                public event RequisitionHandler AboutWindowRequest;
                
                public event RequisitionHandler FullscreenRequest;

                public event RequisitionHandler ExportWindowRequest;

                public event RequisitionHandler LockRequest;

                public event MessageHandler UpdateMessage;

                // Properties /////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }

                public string CurrentMessage {
                        get { return messageStack.Message; }
                }

                public bool Locked {
                        get { return locked; }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Window (Root root)
                {
                        modelRoot = root;
                        locked = false;
                        messageStack = new MessageStack ();
                        messageStack.MessageChange += OnMessageChange;
                }
                
                public void RequestWaitCursor (bool requisition)
                {
                        if (WaitCursorRequest != null)
                                WaitCursorRequest (modelRoot, new RequisitionArgs (requisition));
                }
                
                public void RequestProgressBar (bool requisition)
                {
                        if (ProgressBarRequest != null)
                                ProgressBarRequest (modelRoot, new RequisitionArgs (requisition));
                }
                
                public void UpdateProgressBar (string message, double fraction)
                {
                        if (UpdateProgress != null)
                                UpdateProgress (modelRoot, new ProgressArgs (fraction,
                                                                             message, 
                                                                             false));
                }
                
                public void PulseProgressBar ()
                {
                        if (UpdateProgress != null)
                                UpdateProgress (modelRoot, new ProgressArgs ());
                }
                
                public void ShowHistoryWindow ()
                {
                        if (HistoryWindowRequest != null)
                                HistoryWindowRequest (modelRoot, new RequisitionArgs (true));
                }
                
                public void HideHistoryWindow ()
                {
                        if (HistoryWindowRequest != null)
                                HistoryWindowRequest (modelRoot, new RequisitionArgs (false));
                }
                
                public void ShowTagsWindow ()
                {
                        if (TagsWindowRequest != null)
                                TagsWindowRequest (modelRoot, new RequisitionArgs (true));
                }
                
                public void HideTagsWindow ()
                {
                        if (TagsWindowRequest != null)
                                TagsWindowRequest (modelRoot, new RequisitionArgs (false));
                }

                public void ShowAboutWindow ()
                {
                        if (AboutWindowRequest != null)
                                AboutWindowRequest (modelRoot, new RequisitionArgs (true));
                }
                
                public void HideAboutWindow ()
                {
                        if (AboutWindowRequest != null)
                                AboutWindowRequest (modelRoot, new RequisitionArgs (false));
                }

                public void PushInstantMessage (string message)
                {
                        messageStack.PushInstantMessage (message);
                }

                public long PushMessage (string message, MessageLayer layer)
                {
                        return messageStack.PushMessage (message, (int) layer);
                }

                public void PopMessage (long token)
                {
                        messageStack.RemoveMessage (token);
                }

                internal void PushException (Exception excp)
                {
                        //                        if (InternalException != null)
                        //         InternalException (modelRoot, new ExceptionArgs (excp));
                        // else
                        throw excp;

                        
                }

                public void StartFullscreen ()
                {
                        if (FullscreenRequest != null)
                                FullscreenRequest (modelRoot, new RequisitionArgs (true));
                }

                public void StopFullscreen ()
                {
                        if (FullscreenRequest != null)
                                FullscreenRequest (modelRoot, new RequisitionArgs (false));
                }

                public void Lock ()
                {
                        if (locked)
                                return;

                        if (LockRequest != null)
                                LockRequest (modelRoot, new RequisitionArgs (true));

                        locked = true;
                }

                public void UnLock ()
                {
                        if (! locked)
                                return;

                        if (LockRequest != null)
                                LockRequest (modelRoot, new RequisitionArgs (false));

                        locked = false;
                }

                public void ShowExportWindow ()
                {
                        if (ExportWindowRequest != null)
                                ExportWindowRequest (modelRoot, new RequisitionArgs (true));
                }

                public void HideExportWindow ()
                {
                        if (ExportWindowRequest != null)
                                ExportWindowRequest (modelRoot, new RequisitionArgs (false));
                }
                
                // Private methods /////////////////////////////////////////////

                void OnMessageChange (object o, MessageArgs args)
                {
                        if (UpdateMessage != null)
                                UpdateMessage (modelRoot, args);
                }
        }
        
}
    
