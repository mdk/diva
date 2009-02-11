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

        public class Project {

                // Fields //////////////////////////////////////////////////////

                string name;              // Project name
                string directory;         // Directory base
                Gdv.ProjectFormat format; // The project format we're using
                Gdv.Pipeline pipeline;    // The pipeline
                Commander commander;      // Commander to execute commands and manage undo
                StuffList stuff;          // The list of our stuff
                TagList tags;             // The list of our tags
                ClipList clips;           // The list of our clips
                MediaItemList mediaItems; // The list of our media items
                TrackList tracks;         // All of the tracks in the pipeline
                bool needsSave;           // If we need to save
                
                // Properties //////////////////////////////////////////////////

                public string Name {
                        get { return name; }
                }

                public string Directory {
                        get { return directory; }
                }

                /* A composed project filename location */
                public string FileName {
                        get { return directory + "/" + name + ".div"; }
                }

                public Gdv.ProjectFormat Format {
                        get { return format; }
                }
                
                public Gdv.Pipeline Pipeline {
                        get { return pipeline; }
                }

                public Commander Commander {
                        get { return commander; }
                }
                
                public StuffList Stuff {
                        get { return stuff; }
                }

                public TagList Tags {
                        get { return tags; }
                }
                
                public TrackList Tracks {
                        get { return tracks; }
                }
                
                public ClipList Clips {
                        get { return clips; }
                }
                
                public MediaItemList MediaItems {
                        get { return mediaItems; }
                }

                public bool NeedsSave {
                        get { return needsSave; }
                        set { needsSave = value; }
                }

                // Public methods //////////////////////////////////////////////

                public static Project StartNew (string prjname, string dir, Gdv.ProjectFormat prjformat)
                {
                        Project project = new Project ();
                        
                        project.name = prjname;
                        project.directory = dir;
                        project.format = prjformat;
                        project.pipeline = new Gdv.Pipeline (prjformat);
                        
                        Gdv.Track videoTrack = new Gdv.Track (prjformat, 2);
                        //Gdv.Track audioTrack = new Gdv.Track (prjformat, 1);
                        //Gdv.Track miscTrack = new Gdv.Track (prjformat, 0);
                        
                        project.pipeline.AddTrack (videoTrack);
                        //project.pipeline.AddTrack (audioTrack); 
                        //project.pipeline.AddTrack (miscTrack);
                        
                        // FIXME: Commander storing parent = shit
                        project.commander = new Commander (project);
                        project.stuff = new StuffList ();
                        project.tags = new TagList ();
                        
                        project.tracks = new TrackList ();
                        project.tracks.Add (videoTrack);
                        //project.tracks.Add (audioTrack);
                        //project.tracks.Add (miscTrack);
                        
                        project.clips = new ClipList ();
                        
                        project.mediaItems = new MediaItemList ();
                        project.needsSave = true;
                        
                        return project;
                }
                
                public static Project FromOpener (OpenerTask task)
                {
                        Project project = new Project ();
                                                
                        project.name = task.ProjectName;
                        project.directory = task.ProjectDirectory;
                        project.format = task.ProjectFormat;
                        project.pipeline = task.ProjectPipeline;
                        project.commander = task.ProjectCommander;
                        // FIXME: Commander storing parent = shit
                        project.commander.Parent = project;
                        project.stuff = task.ProjectStuffList;
                        project.tags = task.ProjectTagList;
                        project.tracks = task.ProjectTrackList;
                        project.clips = task.ProjectClipList;
                        project.mediaItems = task.ProjectMediaItemList;
                        project.needsSave = false;

                        // Let's make sure we've got it in recent project list
                        project.AddToRecent ();
                        
                        return project;
                }

                public SaverTask GetSaverTask ()
                {
                        return new SaverTask (this);
                }
                                        
                /* Add the current project to the recent ones */
                public void AddToRecent ()
                {
                        // Now let's try to update the GConf settings
                        List <string> recentProjectsList = new List <string> ();
                        
                        foreach (string str in Config.Projects.Recent)
                                recentProjectsList.Add (str);

                        if (recentProjectsList.Contains (FileName))
                                recentProjectsList.Remove (FileName);
                        
                        recentProjectsList.Add (FileName);
                        Config.Projects.Recent = recentProjectsList.ToArray ();
                }
                
                // Private methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                protected Project ()
                {
                }

        }

}
                
