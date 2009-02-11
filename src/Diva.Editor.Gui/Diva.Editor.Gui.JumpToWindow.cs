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
        using Mono.Unix;
        using Gtk;
        using Util;
        using Widgets;
	using Gdv;

        public class JumpToWindow : Gtk.Window {
                
                // Translatable ////////////////////////////////////////////////
                
                readonly static string titleSS = Catalog.GetString
                        ("Jump To");

                readonly static string hoursSS = Catalog.GetString
                        ("Hours:");

                readonly static string minutesSS = Catalog.GetString
                        ("Minutes:");

                readonly static string secondsSS = Catalog.GetString
                        ("Seconds:");

                readonly static string framesSS = Catalog.GetString
                        ("Frame");
                
		readonly static string jumpToSS = Catalog.GetString
                        ("Jump to:");

                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                
                VBox mainVBox = null;
		HBox spinnerHBox = null;

                HButtonBox buttonBox = null;
                SpinButton spinHours = null;
                SpinButton spinMinutes = null;
                SpinButton spinSeconds = null;
                SpinButton spinFrames = null;
                Label jumpToLabel = null;
		Gdv.Fraction fps;


                // Properties //////////////////////////////////////////////////

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public JumpToWindow (Model.Root root, Window parent) : base (titleSS)
                {
			modelRoot = root;

			modelRoot.Pipeline.Ticker += OnTicker;

			fps = modelRoot.ProjectDetails.Format.VideoFormat.Fps;

			//realFps = fps.Numerator / fps.Denominator;
			Console.WriteLine (fps.ToString());
			Console.WriteLine (fps.FpsFrameDuration());
			Console.WriteLine (fps);

			spinHours = new SpinButton (0, 99, 1);
			spinMinutes = new SpinButton (-1, 60, 1);
			spinSeconds = new SpinButton (-1, 60, 1);
			spinFrames = new SpinButton (-1, 25, 1);

			SyncFromTicker();

			spinHours.ValueChanged += SpinnerChanged;
			spinMinutes.ValueChanged += SpinnerChanged;
			spinSeconds.ValueChanged += SpinnerChanged;
			spinFrames.ValueChanged += SpinnerChanged;

			//spinner vbox
			spinnerHBox = new HBox (false, 2);
			Label hoursLabel = new Label (hoursSS);
			Label minutesLabel = new Label (minutesSS);
			Label secondsLabel = new Label (secondsSS);
			Label framesLabel = new Label (framesSS);
			
			VBox hoursHBox = new VBox (false, 6);
			hoursHBox.Add (hoursLabel);
			hoursHBox.Add (spinHours);

			VBox minutesHBox = new VBox (false, 6);
			minutesHBox.Add (minutesLabel);
			minutesHBox.Add (spinMinutes);

			VBox secondsHBox = new VBox (false, 6);
			secondsHBox.Add (secondsLabel);
			secondsHBox.Add (spinSeconds);

			VBox framesHBox = new VBox (false, 6);
			framesHBox.Add (framesLabel);
			framesHBox.Add (spinFrames);

			spinnerHBox.Add (hoursHBox);
			spinnerHBox.Add (minutesHBox);
			spinnerHBox.Add (secondsHBox);
			spinnerHBox.Add (framesHBox);

                        mainVBox = new VBox (false, 6);
			buttonBox = new HButtonBox ();
			TransientFor = parent;	

			//buttons
			Button closeButton = new Button (Stock.Close);
			closeButton.Clicked += OnCloseButtonClicked;
		
			//buttonBox
			buttonBox.Add (closeButton);
			buttonBox.Spacing = 6;

			//cannonical time HBox
			HBox canonicalHBox = new HBox ();
			canonicalHBox.Spacing = 6;
			jumpToLabel = new Label ();
			SpinnerChanged (null, null);

			canonicalHBox.Add (jumpToLabel);

			//mainVBox
			mainVBox.Spacing = 6;
			mainVBox.PackStart (canonicalHBox);
			mainVBox.PackStart (spinnerHBox);
			mainVBox.PackStart (buttonBox);
			
			Add (mainVBox);
			
			//FIXME: change to CenterOfParent when the parent window is being sent correctly
			SetDefaultSize (200, 300);
			WindowPosition = WindowPosition.Center;
			ShowAll();
                         
                }
                
                // Private methods /////////////////////////////////////////////
                
                void OnCloseButtonClicked (object o, EventArgs args)
                {
			Destroy ();
                }

		void OnTicker (object o, Model.PipelineTickerArgs args)
		{
			Console.WriteLine ("state changed!");
			Console.WriteLine (args.Time);
			SyncFromTicker();

		}

		void SpinnerChanged(object o, EventArgs args)
		{
	
			if ( spinFrames.Value == 25 )
			{
				spinFrames.Value = 0;
				spinSeconds.Value += 1;
			}else if ( spinFrames.Value == -1 )
			{
				if ( spinSeconds.Value == 0 )
				{
					if ( spinMinutes.Value == 0 )
					{
						if ( spinHours.Value == 0 )
						{
							spinFrames.Value = 0;
						}else {
							spinHours.Value -= 1;
							spinMinutes.Value -= 1;
							spinSeconds.Value -= 1;
							spinFrames.Value = 25 -1;
						}
					}else {
						spinMinutes.Value -= 1;
						spinSeconds.Value = 59;
						spinFrames.Value = 25 -1;
					}
				}else {
					spinFrames.Value = 25 - 1;
					spinSeconds.Value -= 1;
				}
			}

			if ( spinSeconds.Value == 60 )
			{
				spinSeconds.Value = 0;
				spinMinutes.Value += 1;
			}else if ( spinSeconds.Value == -1 )
			{
				if ( spinMinutes.Value == 0 )
				{
					if ( spinHours.Value == 0 )
					{
						spinSeconds.Value = 0;
					}
				}else {
					spinSeconds.Value = 59;
					spinMinutes.Value -= 1;
				}

			}

			if ( spinMinutes.Value == 60 )
			{
				spinMinutes.Value = 0;
				spinHours.Value += 1;
			}else if ( spinMinutes.Value == -1 )
			{
				if ( spinHours.Value == 0 )
				{
					spinMinutes.Value = 0;
				}else {
					spinHours.Value -= 1;
					spinMinutes.Value = 59;
				}


			}
			

			SyncToTicker();
		}

		private void SyncToTicker()
		{
			String smpte = String.Format ("{0}:{1}:{2}:{3}" , spinHours.Value, spinMinutes.Value, spinSeconds.Value, spinFrames.Value);
			Gdv.Time t = TimeFu.FromSMPTE (smpte, fps);
			jumpToLabel.UseMarkup = true;
			jumpToLabel.Markup = String.Format ("<big><b>{0} {1}</b></big>" , jumpToSS, TimeFu.ToLongString (t) );
			modelRoot.Pipeline.Seek(t);	
		}

		private void SyncFromTicker()
		{

			Gdv.Time startTime = modelRoot.Pipeline.CurrentTicker;

			String hours = TimeFu.SMPTEHours (startTime);
			String minutes = TimeFu.SMPTEMinutes (startTime);
			String seconds = TimeFu.SMPTESeconds (startTime);
			String frames = TimeFu.SMPTEFrames (startTime, fps);	

			spinHours.Value = Double.Parse (hours);
			spinMinutes.Value = Double.Parse (minutes);
			spinSeconds.Value = Double.Parse (seconds);
			spinFrames.Value = Double.Parse (frames);

		}

        }
        
}

