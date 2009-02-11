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
        
        public sealed class Root {
                
                // Events //////////////////////////////////////////////////////
                
                public event QuitHandler Quit;

                public event SaveNagActionHandler SaveNag;

                // Fields //////////////////////////////////////////////////////
                
                Stuff stuffModel = null;
                Pipeline pipelineModel = null;
                Window windowModel = null;
                TaskProcessor taskProcessorModel = null;
                CommandProcessor commandProcessorModel = null;
                Timeline timelineModel = null;
                ToolSet toolSetModel = null;
                Tags tagsModel = null;
                Tracks tracksModel = null;
                Export exportModel = null;
                Clips clipsModel = null;
                ProjectDetails projectDetailsModel = null;                
                Drag dragModel = null;
                MediaItems mediaItemsModel = null;
                Debug debugModel = null;
                                
                Core.Project project = null;
                                
                // Properties //////////////////////////////////////////////////
                
                public Stuff Stuff { 
                        get { return stuffModel; }
                }
                
                public Window Window {
                        get { return windowModel; }
                }
                
                public TaskProcessor TaskProcessor {
                        get { return taskProcessorModel; }
                }
                
                public Pipeline Pipeline {
                        get { return pipelineModel; }
                }
                
                public CommandProcessor CommandProcessor {
                        get { return commandProcessorModel; }
                }
                
                public Timeline Timeline {
                        get { return timelineModel; }
                }
                
                public ToolSet ToolSet {
                        get { return toolSetModel; }
                }
                
                public Tags Tags {
                        get { return tagsModel; }
                }
                
                public MediaItems MediaItems {
                        get { return mediaItemsModel; }
                }
                
                public Tracks Tracks {
                        get { return tracksModel; }
                }
                
                public Drag Drag { 
                        get { return dragModel; }
                }
                
                public Clips Clips {
                        get { return clipsModel; }
                }

                public Export Export {
                        get { return exportModel; }
                }

                public ProjectDetails ProjectDetails {
                        get { return projectDetailsModel; }
                }

                public Debug Debug {
                        get { return debugModel; }
                }
                
                internal Core.Project Project {
                        get { return project; }
                }
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Root (Core.Project project)
                {
                        this.project = project;
                        
                        stuffModel = new Stuff (this);
                        toolSetModel = new ToolSet (this);
                        timelineModel = new Timeline (this);
                        pipelineModel = new Pipeline (this);
                        taskProcessorModel = new TaskProcessor (this);
                        projectDetailsModel = new ProjectDetails (this);
                        commandProcessorModel = new CommandProcessor (this);
                        tagsModel = new Tags (this);
                        exportModel = new Export (this);
                        mediaItemsModel = new MediaItems (this);
                        tracksModel = new Tracks (this);
                        clipsModel = new Clips (this);
                        windowModel = new Window (this);
                        dragModel = new Drag (this);
                        debugModel = new Debug (this);
                }
                
                public void Save ()
                {
                        Save (null, null);
                }

                public void Save (EventHandler postSave, object postSaveData)
                {
                        // FIXME: That should be moved to post-task
                        project.AddToRecent ();
                        project.NeedsSave = false;
                        
                        Core.SaverTask saverTask = project.GetSaverTask ();
                        
                        if (postSave != null) {
                                saverTask.CustomFinish += postSave;
                                saverTask.CustomData = postSaveData;
                        }
                        
                        TaskProcessor.PushTask (saverTask);
                }
                
                public void QuitModel (QuitMode mode)
                {
                        // FIXME: Do some unbinding?

                        if (project.NeedsSave) {
                                SaveNagActionArgs args = new SaveNagActionArgs (SaveNagAction.Ignore);
                                if (SaveNag != null)
                                        SaveNag (this, args);

                                switch (args.Action) {

                                        case SaveNagAction.Abort:
                                                return;

                                        case SaveNagAction.Ignore:
                                                pipelineModel.Shutdown ();
                                                if (Quit != null)
                                                        Quit (this, new QuitArgs (mode));
                                                break;

                                        case SaveNagAction.Save:
                                                Save (OnQuitAfterSave, mode);
                                                break;

                                        default:
                                                break;
                                }
                        } else {
                                pipelineModel.Shutdown ();
                                if (Quit != null)
                                        Quit (this, new QuitArgs (mode));
                        }
                }

                // Private methods /////////////////////////////////////////////

                void OnQuitAfterSave (object o, EventArgs args)
                {
                        pipelineModel.Shutdown ();
                        if (Quit != null)
                                Quit (this, new QuitArgs ((QuitMode) (o as Core.SaverTask).CustomData));
                }
                
        }
        
}
       
