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

        using System;
        using Gtk;
        using Mono.Unix;
        using Gdv;
        using TimeSpan = Gdv.TimeSpan;
        
        public class ClipMenu : Menu {
                
                // Translatable /////////////////////////////////////////////
                
                readonly static string leftStickSS = Catalog.GetString 
                        ("Stick left");

                readonly static string rightStickSS = Catalog.GetString 
                        ("Stick right");

                readonly static string expandSS = Catalog.GetString 
                        ("Expand to fit");

                readonly static string expandMessageSS = Catalog.GetString 
                        ("Expand '{0}' to fit {1}");

                readonly static string expandInstantMessageSS = Catalog.GetString 
                        ("'{0}' was expanded to fit {1}");
                
                // Fields ///////////////////////////////////////////////////
                
                Clip clip;            // Clip we were spawned for
                Model.Root modelRoot; // App model
                
                // Public methods ///////////////////////////////////////////
                
                /* Constructor */
                public ClipMenu (Model.Root root, Clip clip)
                {
                        modelRoot = root;
                        this.clip = clip;

                        // Remove
                        ImageMenuItem removeItem = new ImageMenuItem (Stock.Delete, null);
                        removeItem.Activated += OnRemoveActivated;
                        Append (removeItem);

                        // Separator
                        Append (new SeparatorMenuItem ());

                        // Left stick
                        MenuItem leftStick = new MenuItem (leftStickSS);
                        leftStick.Activated += OnLeftStickActivated;
                        Append (leftStick);

                        // Right stick
                        MenuItem rightStick = new MenuItem (rightStickSS);
                        rightStick.Activated += OnRightStickActivated;
                        if (clip.Track.ClipStore.GetClipAfterClip (clip) != null)
                                rightStick.Sensitive = true;
                        else
                                rightStick.Sensitive = false;
                        Append (rightStick);

                        // Expand
                        MenuItem expand = new MenuItem (expandSS);
                        expand.Activated += OnExpandActivated;
                        if (clip.Track.ClipStore.GetClipAfterClip (clip) != null &&
                            ! (clip.ParentItem is ITimeable))
                                expand.Sensitive = true;
                        else
                                expand.Sensitive = false;
                        Append (expand);
                }
                
                // Private methods ////////////////////////////////////////////

                /* Remove */
                public void OnRemoveActivated (object sender, EventArgs args)
                {
                        Core.ICommand cmd = new Commands.RemoveClip (clip);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }

                /* Left stick */
                public void OnLeftStickActivated (object sender, EventArgs args)
                {
                        Gdv.Time time;
                        Clip before = clip.Track.ClipStore.GetClipBeforeClip (clip);

                        if (before != null)
                                time = before.TimelineSpan.End;
                        else
                                time = Time.Zero;

                        Core.ICommand cmd = new Commands.MoveClip (clip, time);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }

                /* Right stick */
                public void OnRightStickActivated (object sender, EventArgs args)
                {
                        Gdv.Time time;
                        Clip after = clip.Track.ClipStore.GetClipAfterClip (clip);

                        if (after == null)
                                return;

                        time = after.TimelineSpan.Start - clip.TimelineSpan.Duration;
                        
                        Core.ICommand cmd = new Commands.MoveClip (clip, time);
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }

                /* Expand */
                public void OnExpandActivated (object sender, EventArgs args)
                {
                        Gdv.Time start;
                        Gdv.Time end;
                        Clip after = clip.Track.ClipStore.GetClipAfterClip (clip);
                        Clip before = clip.Track.ClipStore.GetClipBeforeClip (clip);

                        if (after == null)
                                return;

                        start = (before != null) ? before.TimelineSpan.End : Gdv.Time.Zero;
                        end = after.TimelineSpan.Start;
                        
                        Core.IUndoableCommand cmd1 = new Commands.LeftAdjustClip (clip, start);
                        Core.IUndoableCommand cmd2 = new Commands.RightAdjustClip (clip, end);
                        Commands.MultiCommand multiCmd = new Commands.MultiCommand
                                (String.Format (expandMessageSS, clip.ParentItem.Name,
                                                new TimeSpan (start, end)),
                                 String.Format (expandInstantMessageSS, clip.ParentItem.Name,
                                                new TimeSpan (start, end)));
                                                
                        multiCmd.Add (cmd1);
                        multiCmd.Add (cmd2);
                        modelRoot.CommandProcessor.PushCommand (multiCmd);
                }

        }
        
}
