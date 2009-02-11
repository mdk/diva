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
        using System.Collections;
        
        public sealed class Stuff : IModelPart, IEnumerable {
                
                // Fields /////////////////////////////////////////////////////
                
                Root modelRoot = null;
                StuffTreeStore stuffGtkModel = null;
                OrganizeByMethod organizeByMethod; // Current organizing method
                                
                // Events /////////////////////////////////////////////////////
                
                public event OrganizeByMethodHandler OrganizeByMethodChange;
                
                public event GtkModelHandler GtkModelChange;
                
                public event Core.AppliedTagHandler StuffAppliedTagChange;
                
                // Properties /////////////////////////////////////////////////
                
                public Root Root {
                        get { return modelRoot; }
                }
                
                public Gtk.TreeModel GtkModel {
                        get { return stuffGtkModel; }
                }
                
                public OrganizeByMethod OrganizeByMethod {
                        get { return organizeByMethod; }
                        set {
                                organizeByMethod = value;
                                SpawnNewModel ();
                                
                                if (OrganizeByMethodChange != null)
                                        OrganizeByMethodChange (modelRoot, 
                                                                new OrganizeByMethodArgs (value));
                                if (GtkModelChange != null)
                                        GtkModelChange (modelRoot, 
                                                        new GtkModelArgs (stuffGtkModel));
                        }
                }
                                
                // Public methods /////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public Stuff (Root root)
                {
                        modelRoot = root;
                        organizeByMethod = OrganizeByMethod.ByType;
                        SpawnNewModel ();
                        
                        modelRoot.Project.Stuff.StuffStatusChange += OnStuffStatusChanged;
                        modelRoot.Project.Stuff.StuffAppliedTagChange += OnStuffAppliedTagChanged;
                        
                }
                
                public void AddMediaFiles (params string[] fileName)
                {
                        // FIXME: Try/catch
                        Core.ICommand cmd;
                        
                        if (fileName.Length == 1)
                                cmd = new Commands.AddFileToProject (fileName [0]);
                        else
                                cmd = new Commands.AddManyFilesToProject (fileName);
                        
                        modelRoot.CommandProcessor.PushCommand (cmd);
                }
                
                public Core.Stuff GetStuffForIter (Gtk.TreeIter iter)
                {
                        return stuffGtkModel.GetStuffForIter (iter);
                }
                
                public IEnumerator GetEnumerator ()
                {
                        return modelRoot.Project.Stuff.GetEnumerator ();
                }
                
                // Private methods /////////////////////////////////////////////
                
                /* Don't forget to fire the event */
                void SpawnNewModel ()
                {
                        IStuffOrganizer organizer = null;
                        switch (organizeByMethod) {
                                
                                case OrganizeByMethod.ByType:
                                organizer = new ByTypeOrganizer ();
                                break;
                                
                                case OrganizeByMethod.ByTag:
                                organizer = new ByTagOrganizer ();
                                break;

                                case OrganizeByMethod.ByLength:
                                organizer = new ByLengthOrganizer ();
                                break;

                                case OrganizeByMethod.ByUsage:
                                organizer = new ByUsageOrganizer (modelRoot);
                                break;
                                
                                default:
                                throw new Exception ("Unsupported organizing method");
                        }
                                
                        stuffGtkModel = new StuffTreeStore (organizer);
                        
                        foreach (Core.Stuff stuff in modelRoot.Project.Stuff)
                                stuffGtkModel.Add (stuff);
                }
                
                void OnStuffStatusChanged (object o, Core.StuffStatusArgs args)
                {
                        if (args.Present == true)
                                stuffGtkModel.Add (args.Stuff);
                        else
                                stuffGtkModel.Remove (args.Stuff);
                }
                
                void OnStuffAppliedTagChanged (object o, Core.AppliedTagArgs args)
                {
                        if (StuffAppliedTagChange != null)
                                StuffAppliedTagChange (modelRoot, args);
                }
                
        }
        
}
