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

/* An overriden ComboBox that is used to select a zoom level.
 */

namespace Diva.Editor.Gui {

        using System;
        using Gtk;
        using Mono.Unix;
        using Util;
        
        public sealed class ZoomToComboBox : ComboBox {
                
                // Private enums ///////////////////////////////////////////////
                
                enum ZoomLevel { OneSecond, FewSeconds, OneMinute, FewMinutes, 
                                 OneHour }
                
                // Translatable ////////////////////////////////////////////////
                                
                readonly static string toOneSecondSS =
                        Catalog.GetString ("Zoom to one second");
                
                readonly static string toFewSecondsSS =
                        Catalog.GetString ("Zoom to few seconds");
                
                readonly static string toOneMinuteSS = 
                        Catalog.GetString ("Zoom to one minute");
                
                readonly static string toFewMinutesSS =
                        Catalog.GetString ("Zoom to few minutes");
                
                readonly static string toOneHourSS =
                        Catalog.GetString ("Zoom to one hour");

                // Fields /////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                ZoomLevel currentLevel;      // To avoid some extra proc when nothing changes
                bool trickSwitch = false;    // To avoid recursive events

                // Members /////////////////////////////////////////////////////
                
                Gtk.ListStore listStore = new Gtk.ListStore (typeof (string), 
                                                             typeof (ZoomLevel));

                // Public methods ///////////////////////////////////////////

                /* CONSTRUCTOR */
                public ZoomToComboBox (Model.Root root) : base ()
                {
                        modelRoot = root;
                        
                        listStore.AppendValues (String.Format ("<small>{0}</small>", toOneSecondSS),
                                                ZoomLevel.OneSecond);
                        listStore.AppendValues (String.Format ("<small>{0}</small>", toFewSecondsSS),
                                                ZoomLevel.FewSeconds);
                        listStore.AppendValues (String.Format ("<small>{0}</small>", toOneMinuteSS), 
                                                ZoomLevel.OneMinute);
                        listStore.AppendValues (String.Format ("<small>{0}</small>", toFewMinutesSS),
                                                ZoomLevel.FewMinutes);
                        //listStore.AppendValues (String.Format ("<small>{0}</small>", toOneHourSS), 
                        //                        ZoomLevel.OneHour);
                        
                        this.Model = listStore;
       
                        // Renderers 
                        Gtk.CellRendererText textRenderer = new Gtk.CellRendererText ();
                        PackStart (textRenderer, true);
                        AddAttribute (textRenderer, "markup", 0);
                        
                        // Initial
                        SelectByLevel (TimeSpanToLevel (modelRoot.Timeline.DisplayedTimeSpan));
                        
                        // Model bind
                        modelRoot.Timeline.DisplayedTimeSpanChange += OnDisplayTimeSpanChange;
                }
                
                // Private methods //////////////////////////////////////////
                
                ZoomLevel TimeSpanToLevel (Gdv.TimeSpan timeSpan)
                {
                        //if (timeSpan.Duration.Seconds >= 3600.0)
                        //        return ZoomLevel.OneHour;
                        
                        if (timeSpan.Duration.Seconds >= 180.0)
                                return ZoomLevel.FewMinutes;
                        else if (timeSpan.Duration.Seconds >= 60.0)
                                return ZoomLevel.OneMinute;
                        else if (timeSpan.Duration.Seconds >= 3.0)
                                return ZoomLevel.FewSeconds;
                        else 
                                return ZoomLevel.OneSecond;
                }
                
                Gdv.TimeSpan LevelToTimeSpan (ZoomLevel level)
                {
                        Gdv.Time halfDuration = Gdv.Time.Empty;
                        Gdv.Time zoomCenter = (modelRoot.Timeline.DisplayedTimeSpan.Start +
                                               modelRoot.Timeline.DisplayedTimeSpan.End) / 2;
                        
                        switch (level) {
                                
                                case ZoomLevel.OneSecond:
                                halfDuration = Gdv.Time.FromSeconds (1.0);
                                break;
                                
                                case ZoomLevel.FewSeconds:
                                halfDuration = Gdv.Time.FromSeconds (5.0);
                                break;
                                
                                case ZoomLevel.OneMinute:
                                halfDuration = Gdv.Time.FromSeconds (60.0);
                                break;
                                
                                case ZoomLevel.FewMinutes:
                                halfDuration = Gdv.Time.FromSeconds (180.0);
                                break;
                                
                                //case ZoomLevel.OneHour:
                                //halfDuration = Gdv.Time.FromSeconds (3600.0);
                                //break;
                        }
                        
                        Gdv.Time leftCut = (zoomCenter >= halfDuration) ? halfDuration : zoomCenter;
                        Gdv.TimeSpan newTimeSpan = new Gdv.TimeSpan (zoomCenter - leftCut,
                                                                     zoomCenter + halfDuration + 
                                                                     (halfDuration - leftCut));
                        
                        return newTimeSpan;
                }
                
                void SelectByLevel (ZoomLevel level)
                {
                        TreeIter methodIter = GtkFu.TreeModelIterByInt (listStore, (int) level, 1);
                        trickSwitch = true;
                        SetActiveIter (methodIter);
                        trickSwitch = false;
                        
                        currentLevel = level;
                }
                
                void OnDisplayTimeSpanChange (object o, Model.TimeSpanArgs args)
                {
                        ZoomLevel newLevel = TimeSpanToLevel (args.TimeSpan);
                        if (newLevel != currentLevel)
                                SelectByLevel (newLevel);
                }
                
                /* Selection was changed */
                protected override void OnChanged ()
                {
                        if (trickSwitch)
                                return;
                        
                        TreeIter iter;
                        GetActiveIter (out iter);
                        ZoomLevel newLevel = (ZoomLevel) listStore.GetValue (iter, 1);

                        Gdv.Time currentTicker = modelRoot.Pipeline.CurrentTicker;
                        Gdv.TimeSpan zoomSpan = LevelToTimeSpan (newLevel);
                        Gdv.Time leftCut = zoomSpan.Duration / 2;
                        
                        if (leftCut > currentTicker)
                                leftCut = currentTicker;
                        
                        Gdv.Time start = currentTicker - leftCut;
                        Gdv.Time end = currentTicker + (zoomSpan.Duration - leftCut);
                        
                        modelRoot.Timeline.DisplayedTimeSpan = new Gdv.TimeSpan (start, end);
                }
                
        }

}
