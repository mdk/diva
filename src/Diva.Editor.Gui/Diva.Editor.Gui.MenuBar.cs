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

namespace Diva.Editor.Gui {
        
        using System;
        using Gtk;
        using Mono.Unix;
        using Widgets;
        using System.Collections;
        
        public class MenuBar : Gtk.MenuBar {
                
                // Translatable ////////////////////////////////////////////////
                                
                readonly static string fileSS = Catalog.GetString
                        ("_File");
                
                readonly static string editSS = Catalog.GetString
                        ("_Edit");
                
                readonly static string projectSS = Catalog.GetString
                        ("_Project");

                readonly static string playbackSS = Catalog.GetString
                        ("P_layback");

                readonly static string helpSS = Catalog.GetString
                        ("_Help");

                readonly static string saveSS = Catalog.GetString
                        ("Save");
                
                readonly static string undoFrmSS = Catalog.GetString
                        ("Undo: {0}");

                readonly static string redoFrmSS = Catalog.GetString
                        ("Redo: {0}");
                
                readonly static string nothingToUndoSS = Catalog.GetString
                        ("Nothing to undo");
                
                readonly static string nothingToRedoSS = Catalog.GetString
                        ("Nothing to redo");
                
                readonly static string quitToMenuSS = Catalog.GetString
                        ("Quit to main menu");

                readonly static string quitSS = Catalog.GetString
                        ("Quit");
                
                readonly static string tagsSS = Catalog.GetString
                        ("Tags...");

                readonly static string historySS = Catalog.GetString
                        ("History...");

                readonly static string historyClearSS = Catalog.GetString
                        ("Clear history");

                readonly static string historyClearWarningHeaderSS = Catalog.GetString
                        ("Are you sure you want to clear the history?");

                readonly static string historyClearWarningMessageSS = Catalog.GetString
                        ("You will no longer be able to undo/redo previous actions.");

                readonly static string exportSS = Catalog.GetString
                        ("Export");

                readonly static string playSS = Catalog.GetString
                        ("Play");

                readonly static string pauseSS = Catalog.GetString
                        ("Pause");
                
                readonly static string nextCutSS = Catalog.GetString
                        ("Next cut");

                readonly static string prevCutSS = Catalog.GetString
                        ("Previous Cut");
                
                readonly static string rewindSS = Catalog.GetString
                        ("Rewind");

                readonly static string newSS = Catalog.GetString
                        ("New project");

                readonly static string fullscreenSS = Catalog.GetString
                        ("Fullscreen");

                readonly static string saveScreenshotSS = Catalog.GetString
                        ("Save video screenshot");
                
                readonly static string failedScreenshotSaveSS = Catalog.GetString
                        ("Screenshot image file could not be saved. " +
                         "Make sure the disk is not full and you've got the necessary write permissions.");
                
                readonly static string failedScreenshotSaveHeaderSS = Catalog.GetString
                        ("Saving failed");

                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;               // The model root
                
                ImageMenuItem undoMenuItem = null;         // Edit->Undo item
                ImageMenuItem redoMenuItem = null;         // Edit->Redo item
                MenuItem historyMenuItem = null;           // Edit->History item
                ImageMenuItem historyClearMenuItem = null; // Edit->Clear History item
                ImageMenuItem playMenuItem = null;         // Playback->Play (Pause) item
                
                // HistoryWindow historyWindow;
                
                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public MenuBar (Model.Root modelRoot) : base ()
                {
                        this.modelRoot = modelRoot;

                        // File sub-menu
                        MenuItem fileMenuItem = new MenuItem (fileSS);
                        Menu fileMenu = new Menu ();
                        BuildFileMenu (fileMenu);
                        fileMenuItem.Submenu = fileMenu;
                        Append (fileMenuItem);
                        
                        // Edit sub-menu
                        MenuItem editMenuItem = new MenuItem (editSS);
                        Menu editMenu = new Menu ();
                        BuildEditMenu (editMenu);
                        editMenuItem.Submenu = editMenu;
                        Append (editMenuItem);

                        // Playback
                        Menu playbackMenu = new Menu ();
                        MenuItem playbackMenuItem = new MenuItem (playbackSS);
                        BuildPlaybackMenu (playbackMenu);
                        playbackMenuItem.Submenu = playbackMenu;
                        Append (playbackMenuItem);
                        
                        // Project sub-menu
                        Menu projectMenu = new Menu ();
                        MenuItem projectMenuItem = new MenuItem (projectSS);
                        BuildProjectMenu (projectMenu);
                        projectMenuItem.Submenu = projectMenu;
                        Append (projectMenuItem);

                        // Help sub-menu
                        Menu helpMenu = new Menu ();
                        MenuItem helpMenuItem = new MenuItem (helpSS);
                        BuildHelpMenu (helpMenu);
                        helpMenuItem.Submenu = helpMenu;
                        Append (helpMenuItem);

                        // Debug sub-menu. That's not translated
                        #if DEBUG
                        Menu debugMenu = new Menu ();
                        MenuItem debugMenuItem = new MenuItem ("Debug");
                        BuildDebugMenu (debugMenu);
                        debugMenuItem.Submenu = debugMenu;
                        Append (debugMenuItem);
                        #endif

                        // Model bind
                        modelRoot.CommandProcessor.UndoRedoChange += OnUndoRedoChanged;
                        modelRoot.Pipeline.StateChange += OnPipelineStateChanged;
                }

                // Private methods /////////////////////////////////////////////
                
                void BuildFileMenu (Menu fileMenu)
                {
                        ImageMenuItem newMenuItem = new ImageMenuItem (newSS);
                        newMenuItem.Image = new Image (Stock.New, IconSize.Menu);
                        newMenuItem.Activated += OnNewActivated;
                        
                        MenuItem saveMenuItem = new ImageMenuItem (Stock.Save, null);
                        saveMenuItem.Activated += OnSaveActivated;
                        
                        MenuItem quitToMenuItem = new MenuItem (quitToMenuSS);
                        quitToMenuItem.Activated += OnQuitToMainMenuActivated;
                        
                        MenuItem quitItem = new ImageMenuItem (Stock.Quit, null);
                        quitItem.Activated += OnQuitActivated;

                        MenuItem exportItem = new MenuItem (exportSS);
                        Menu exportMenu = new Menu ();

                        // Let's add all export plugins
                        IEnumerator exportPluginsEnumerator = PluginLib.PluginManager.ExportPlugins;
                        while (exportPluginsEnumerator.MoveNext ()) {
                                PluginLib.ExportPlugin plugin = (PluginLib.ExportPlugin) exportPluginsEnumerator.Current;
                                MenuItem item = new MenuItem (plugin.MenuItemName);
                                exportMenu.Append (item);
                                
                                PluginLib.PluginManager.AddPrivate (plugin, item);
                                item.Activated += OnExportActivated;
                        }

                        exportItem.Submenu = exportMenu;

                        MenuItem saveScreenshotItem = new MenuItem (saveScreenshotSS);
                        saveScreenshotItem.Activated += OnSaveScreenshotActivated;
                                                
                        fileMenu.Append (newMenuItem);
                        fileMenu.Append (new SeparatorMenuItem ());
                        fileMenu.Append (saveMenuItem);
                        fileMenu.Append (exportItem);
                        fileMenu.Append (saveScreenshotItem);
                        fileMenu.Append (new SeparatorMenuItem ());
                        fileMenu.Append (quitToMenuItem);
                        fileMenu.Append (quitItem);
                }
                
                void BuildEditMenu (Menu editMenu)
                {
                        undoMenuItem = new ImageMenuItem (String.Empty);
                        undoMenuItem.Image = new Image (Stock.Undo, IconSize.Menu);
                        undoMenuItem.Activated += OnUndoActivated;
                        
                        redoMenuItem = new ImageMenuItem (String.Empty);
                        redoMenuItem.Image = new Image (Stock.Redo, IconSize.Menu);
                        redoMenuItem.Activated += OnRedoActivated;

                        historyClearMenuItem = new ImageMenuItem (historyClearSS);
                        historyClearMenuItem.Image = new Image (Stock.Clear, IconSize.Menu);
                        historyClearMenuItem.Activated += OnHistoryClearActivated;

                        historyMenuItem = new MenuItem (historySS);
                        historyMenuItem.Activated += OnHistoryActivated;
                        
                        RefillUndoRedoItems (modelRoot.CommandProcessor.CanUndo,
                                             modelRoot.CommandProcessor.CanRedo,
                                             modelRoot.CommandProcessor.UndoMessage,
                                             modelRoot.CommandProcessor.RedoMessage);
                        

                        
                        
                        editMenu.Append (undoMenuItem);
                        editMenu.Append (redoMenuItem);
                        editMenu.Append (historyMenuItem);
                        editMenu.Append (new SeparatorMenuItem ());
                        editMenu.Append (historyClearMenuItem);
                }
                
                void BuildProjectMenu (Menu projectMenu)
                {
                        MenuItem tagsMenuItem = new MenuItem (tagsSS);
                        tagsMenuItem.Activated += OnTagsActivated;
                        
                        projectMenu.Append (tagsMenuItem);
                }

                void BuildHelpMenu (Menu helpMenu)
                {
                        MenuItem aboutMenuItem = new ImageMenuItem (Stock.About, null);
                        aboutMenuItem.Activated += OnAboutActivated;
                                                
                        helpMenu.Append (aboutMenuItem);
                }

                void BuildDebugMenu (Menu debugMenu)
                {
                        MenuItem slowCombMenuItem = new MenuItem ("Slow comb seek");
                        slowCombMenuItem.Activated += OnSlowCombActivated;

                        MenuItem fastCombMenuItem = new MenuItem ("Fast comb seek");
                        fastCombMenuItem.Activated += OnFastCombActivated;

                        MenuItem stopCombMenuItem = new MenuItem ("Stop comb seek");
                        stopCombMenuItem.Activated += OnStopCombActivated;
                                                
                        debugMenu.Append (slowCombMenuItem);
                        debugMenu.Append (fastCombMenuItem);
                        debugMenu.Append (stopCombMenuItem);
                }

                void BuildPlaybackMenu (Menu playbackMenu)
                {
                        playMenuItem = new ImageMenuItem (playSS);
                        playMenuItem.Image = new Image (Stock.MediaPlay, IconSize.Menu);
                        playMenuItem.Activated += OnPlayPauseActivated;

                        ImageMenuItem rewindMenuItem = new ImageMenuItem (rewindSS);
                        rewindMenuItem.Image = new Image (Stock.MediaRewind, IconSize.Menu);
                        rewindMenuItem.Activated += OnRewindActivated;

                        ImageMenuItem prevMenuItem = new ImageMenuItem (prevCutSS);
                        prevMenuItem.Image = new Image (Stock.MediaPrevious, IconSize.Menu);
                        prevMenuItem.Activated += OnPrevCutActivated;
                        
                        ImageMenuItem nextMenuItem = new ImageMenuItem (nextCutSS);
                        nextMenuItem.Image = new Image (Stock.MediaNext, IconSize.Menu);
                        nextMenuItem.Activated += OnNextCutActivated;

                        MenuItem fullscreenItem = new MenuItem (fullscreenSS);
                        fullscreenItem.Activated += OnFullscreenActivated;
                        
                        playbackMenu.Append (playMenuItem);
                        playbackMenu.Append (new SeparatorMenuItem ());
                        playbackMenu.Append (rewindMenuItem);
                        playbackMenu.Append (prevMenuItem);
                        playbackMenu.Append (nextMenuItem);
                        playbackMenu.Append (new SeparatorMenuItem ());
                        playbackMenu.Append (fullscreenItem);
                }

                /* "Save" */
                void OnSaveActivated (object o, EventArgs args)
                {
                        modelRoot.Save ();
                }

                /* "Quit" */
                void OnQuitActivated (object o, EventArgs args)
                {
                        modelRoot.QuitModel (Model.QuitMode.Complete);
                }

                /* "Quit to main menu" */
                void OnQuitToMainMenuActivated (object o, EventArgs args)
                {
                        modelRoot.QuitModel (Model.QuitMode.ToMenu);
                }

                /* "Undo" */
                void OnUndoActivated (object o, EventArgs args)
                {
                        if (modelRoot.CommandProcessor.CanUndo)
                                modelRoot.CommandProcessor.Undo ();
                }

                /* "Redo" */
                void OnRedoActivated (object o, EventArgs args)
                {
                        if (modelRoot.CommandProcessor.CanRedo)
                                modelRoot.CommandProcessor.Redo ();
                }

                /* "History..." */
                void OnHistoryActivated (object o, EventArgs args)
                {
                        modelRoot.Window.ShowHistoryWindow ();
                }

                /* "History Clear" */
                void OnHistoryClearActivated (object o, EventArgs args)
                {
                        ResponseType yesNo = FastDialog.WarningYesNo (null, historyClearWarningHeaderSS, historyClearWarningMessageSS);
                        if (yesNo == ResponseType.Yes)
                        {
                                modelRoot.CommandProcessor.WipeHistory ();
                        }
                }
                
                /* "Tags..." */
                void OnTagsActivated (object o, EventArgs args)
                {
                        modelRoot.Window.ShowTagsWindow ();
                }

                /* One of the export options/plugins */
                void OnExportActivated (object o, EventArgs args)
                {
                        // Let's get the plugin by private
                        PluginLib.Plugin plg = PluginLib.PluginManager.GetPluginByPrivate (o);
                        
                        // FIXME: Actual window
                        Core.ExportSettings settings = (plg as PluginLib.ExportPlugin).GetExportSettings (null,
                                                                                                          modelRoot.ProjectDetails.Format);
                        
                        if (settings == null)
                                return;
                        else
                                modelRoot.Export.Start (settings);
                }

                /* "Save video frame screenshot" */
                void OnSaveScreenshotActivated (object o, EventArgs args)
                {
                        FileChooserDialog dialog = new FileChooserDialog (saveScreenshotSS, null, FileChooserAction.Save, 
                                                                          Stock.Cancel, ResponseType.Cancel, 
                                                                          Stock.Ok, ResponseType.Accept);
                        dialog.SelectMultiple = false;
                        FileFilter pngFilter = new FileFilter ();
                        pngFilter.AddPattern ("*.dv");
                        pngFilter.Name = "PNG images"; // FIXME: Translatable!
                        dialog.AddFilter (pngFilter);
                        
                        int resp = dialog.Run ();
                        
                        // FIXME: Add overwrite confirmaion
                        // FIXME: Automatically add .png extension if not supplied
                        
                        if ((ResponseType) resp == ResponseType.Accept) {
                                try {
                                        modelRoot.Pipeline.SaveLastBuffer (dialog.Filename);
                                } catch {
                                        FastDialog.ErrorOk (null, 
                                                            failedScreenshotSaveHeaderSS,
                                                            failedScreenshotSaveSS);
                                }
                        }
                                                
                        dialog.Destroy ();
                }
                
                /* Refresh the undo/redo commands displayed */
                void OnUndoRedoChanged (object o, Model.UndoRedoArgs args)
                {
                        RefillUndoRedoItems (args.CanUndo, args.CanRedo,
                                             args.UndoMessage, args.RedoMessage);
                }
                
                void RefillUndoRedoItems (bool canUndo, bool canRedo, 
                                          string undoMessage, string redoMessage)
                {
                        undoMenuItem.Sensitive = canUndo;
                        redoMenuItem.Sensitive = canRedo;
                        historyMenuItem.Sensitive = (canUndo == true || canRedo == true) ? true : false;
                        historyClearMenuItem.Sensitive = (canUndo == true || canRedo == true) ? true : false;
                        
                        // Undo message
                        Gtk.Label undoLabel = undoMenuItem.Child as Label;
                        if (canUndo)
                                undoLabel.Text = String.Format (undoFrmSS,
                                                                undoMessage);
                        else 
                                undoLabel.Text = nothingToUndoSS;
                        
                        // Redo message
                        Gtk.Label redoLabel = redoMenuItem.Child as Label;
                        if (canRedo)
                                redoLabel.Text = String.Format (redoFrmSS,
                                                                redoMessage);
                        else
                                redoLabel.Text = nothingToRedoSS;
                }

                /* About */
                void OnAboutActivated (object o, EventArgs args)
                {
                        modelRoot.Window.ShowAboutWindow ();
                }

                /* Pipeline state was changed. Let's update the menuitem label */
                public void OnPipelineStateChanged (object o, Model.PipelineStateArgs args)
                {
                        Gtk.Label playLabel = playMenuItem.Child as Label;
                        
                        if (args.Playing == true) {
                                playLabel.Text = pauseSS;
                                playMenuItem.Image = new Image (Stock.MediaPause, IconSize.Menu);
                        } else {
                                playLabel.Text = playSS;
                                playMenuItem.Image = new Image (Stock.MediaPlay, IconSize.Menu);
                        }
                }

                /* "Play" or "Pause" */
                void OnPlayPauseActivated (object o, EventArgs args)
                {
                        modelRoot.Pipeline.PlayPause ();
                }
                
                /* "Rewind" */
                void OnRewindActivated (object o, EventArgs args)
                {
                        modelRoot.Pipeline.SeekToZero ();
                }

                /* "Prev Cut" */
                void OnPrevCutActivated (object o, EventArgs args)
                {
                        modelRoot.Pipeline.SeekToPrevCut ();
                }

                /* "Next Cut" */
                void OnNextCutActivated (object o, EventArgs args)
                {
                        modelRoot.Pipeline.SeekToNextCut ();                        
                }

                /* "New" */
                void OnNewActivated (object o, EventArgs args)
                {
                        modelRoot.QuitModel (Model.QuitMode.ToNewProject);
                }
                
                void OnFullscreenActivated (object o, EventArgs args)
                {
                        modelRoot.Window.StartFullscreen ();
                }

                /* Debug */
                void OnSlowCombActivated (object o, EventArgs args)
                {
                        modelRoot.Debug.StartCombSeek (1000);
                }

                /* Debug */
                void OnFastCombActivated (object o, EventArgs args)
                {
                        modelRoot.Debug.StartCombSeek (300);
                }

                /* Debug */
                void OnStopCombActivated (object o, EventArgs args)
                {
                        modelRoot.Debug.StopCombSeek ();
                }
                

        }
        
}
                 
                
        
        
