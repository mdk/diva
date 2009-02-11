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

        public class MoveToHBox : Gtk.HBox {
                
                // Translatable ////////////////////////////////////////////////
                

                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                
		HBox spinnerHBox = null;
                SpinButton spinHours = null;
                SpinButton spinMinutes = null;
                SpinButton spinSeconds = null;
                SpinButton spinFrames = null;
		Gdv.Fraction fps;
		bool isPlaying = false;
		bool syncingTo = false;
		bool syncingFrom = false;

                // Properties //////////////////////////////////////////////////

                // Public methods //////////////////////////////////////////////

                /* CONSTRUCTOR */
                public MoveToHBox (Model.Root root)
		{
			modelRoot = root;

			modelRoot.Pipeline.Ticker += OnTicker;
			modelRoot.Pipeline.StateChange += OnStateChange;

			fps = modelRoot.ProjectDetails.Format.VideoFormat.Fps;

			spinHours = new SpinButton (0, 99, 1);
			spinMinutes = new SpinButton (-1, 60, 1);
			spinSeconds = new SpinButton (-1, 60, 1);
			spinFrames = new SpinButton (-1, 25, 1);

			/*
			SpinButton[] buttonsArray;
			buttonsArray = Array.CreateInst
			buttonsArray.SetValue (spinHours, 0 );
			buttonsArray += spinMinutes;
			buttonsArray += spinSeconds;
			buttonsArray += spinFrames;
			*/

			spinHours.Alignment = 1;
			spinMinutes.Alignment = 1;
			spinSeconds.Alignment = 1;
			spinFrames.Alignment = 1;

			SyncFromTicker();

			spinHours.ValueChanged += SpinnerChanged;
			spinMinutes.ValueChanged += SpinnerChanged;
			spinSeconds.ValueChanged += SpinnerChanged;
			spinFrames.ValueChanged += SpinnerChanged;
			
			Label separatorA = new Label ();
			Label separatorB = new Label ();
			Label separatorC = new Label ();
			String separatorString = "<b>:</b>";
			separatorA.Markup = String.Format ( separatorString  );
			separatorB.Markup = String.Format ( separatorString  );
			separatorC.Markup = String.Format ( separatorString  );
			
			Add (spinHours);
			Add (separatorA);

			Add (spinMinutes);
			Add (separatorB);

			Add (spinSeconds);
			Add (separatorC);

			Add (spinFrames);
		
                         
                }
                
                // Private methods /////////////////////////////////////////////
                
		void OnTicker (object o, Model.PipelineTickerArgs args)
		{
			
			//Console.WriteLine ( "Enter OnTicker" );
			
			Gdv.Time t = SpinnerTime();
			if ( syncingTo != true )
			{
				if ( isPlaying == false )
				{
					if ( t != modelRoot.Pipeline.CurrentTicker )
					{
						SyncFromTicker();
					}
				}
			}
		}

		void OnStateChange (object o, Model.PipelineStateArgs args)
		{
				
		//	Console.WriteLine ( "Enter OnStateChange" );
			
			isPlaying = args.Playing;
			Gdv.Time t = SpinnerTime();


			if ( isPlaying == false )
			{
				if ( t != modelRoot.Pipeline.CurrentTicker )
				{
					SyncFromTicker();
				}
			}
			
			
		}

		void SpinnerChanged(object o, EventArgs args)
		{
		


		//	Console.WriteLine ( "Enter SpinnerChanged" );
			if ( syncingFrom == true )
			{
				return;
			}
			Gdv.Time t = SpinnerTime();

			if ( t == modelRoot.Pipeline.CurrentTicker )
			{
				return;
			}
			
			if ( isPlaying == true)
			{
				Console.WriteLine ( isPlaying );
				return;
			} 
			



			if ( spinHours.Value > 0 || spinMinutes.Value > 0 || spinSeconds.Value > 0 )
			{
				spinFrames.SetRange ( -1, fps.FpsDigitize() );
			} else {
				spinFrames.SetRange ( 0, fps.FpsDigitize() );
			}

			if ( spinHours.Value > 0 || spinMinutes.Value > 0 )
			{
				spinSeconds.SetRange ( -1, 60 );
			} else {
				spinSeconds.SetRange ( 0, 60 );
			}

			if ( spinHours.Value > 0 )
			{
				spinMinutes.SetRange ( -1, 60 );
			} else {
				spinMinutes.SetRange ( 0, 60 );
			}
			

			if ( spinFrames.Value == fps.FpsDigitize() )
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
						spinFrames.Value = fps.FpsDigitize() -1;
					}
				}else {
					spinFrames.Value = fps.FpsDigitize() - 1;
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

		private Gdv.Time SpinnerTime()
		{
			String smpte = String.Format ("{0}:{1}:{2}:{3}" , spinHours.Value, spinMinutes.Value, spinSeconds.Value, spinFrames.Value);
			Gdv.Time t = TimeFu.FromSMPTE (smpte, fps);
			return t;
		}	

		private void SyncToTicker()
		{

			syncingTo = true;

			//Console.WriteLine ( "Enter SyncToTicker" );
			Gdv.Time t = SpinnerTime();
			if ( t != modelRoot.Pipeline.CurrentTicker )
			{
				modelRoot.Pipeline.Seek(t);
			}
			
			syncingTo = false;

		}

		private void SyncFromTicker()
		{

			//Console.WriteLine ( "Enter SyncFromTicker" );
			
			syncingFrom = true;
			Gdv.Time startTime = modelRoot.Pipeline.CurrentTicker;

			String hours = TimeFu.SMPTEHours (startTime);
			String minutes = TimeFu.SMPTEMinutes (startTime);
			String seconds = TimeFu.SMPTESeconds (startTime);
			String frames = TimeFu.SMPTEFrames (startTime, fps);	
			
			spinHours.Value = Double.Parse (hours);
			spinMinutes.Value = Double.Parse (minutes);
			spinSeconds.Value = Double.Parse (seconds);
			spinFrames.Value = Double.Parse (frames);

			syncingFrom = false;

		}

        }
        
}
