////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// MIT X11 license, Copyright (c) 2005-2006 by:                               //
//                                                                            //
// Authors:                                                                   //
//      Michael Dominic K. <michaldominik@gmail.com>                          //
//      Travis Hansen <travisghansen@gmail.com>                               //
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

namespace Diva.Util {

        using System;
        using Gdv;
        using Mono.Unix;

        public static class TimeFu {
                
                // Translatable ///////////////////////////////////////////////
                
                readonly static string minSS = Catalog.GetString
                        ("min");
                
                readonly static string secSS = Catalog.GetString
                        ("sec");

                readonly static string minuteSS = Catalog.GetString
                       ("minute");
                
                readonly static string minutesSS = Catalog.GetString
                        ("minutes");
                
                readonly static string secondSS = Catalog.GetString
                        ("second");
                
                readonly static string secondsSS = Catalog.GetString
                        ("seconds");
        
                // Public Methods /////////////////////////////////////////////
                
                /* Convert given time to a SMPTE value. With or without hours */
                public static string ToSMPTE (Time time, Fraction fps, bool withHours)
                {
                        UInt64 secsTotal = (UInt64) time.Seconds;
                        UInt64 minsTotal = secsTotal / 60;

                        UInt64 secs = secsTotal % 60;
                        UInt64 mins = (secsTotal / 60) % 60;
                        UInt64 hours = (minsTotal / 60); 
                        UInt64 frames = (UInt64) (time % Time.OneSecond) / fps.FpsFrameDuration ();
                        
                        if (! withHours && hours == 0)
                                return String.Format ("{0:d2}:{1:d2}:{2:d2}", mins, secs, frames);
                        else 
                                return String.Format ("{0:d2}:{1:d2}:{2:d2}:{3:d2}", hours, mins, secs, frames);
                }

                /* Return the frame value of an SMPTE string give a Time and Fraction */
                public static string SMPTEFrames (Time time, Fraction fps)
                {
                        UInt64 secsTotal = (UInt64) time.Seconds;
                        UInt64 minsTotal = secsTotal / 60;

                        UInt64 secs = secsTotal % 60;
                        UInt64 mins = (secsTotal / 60) % 60;
                        UInt64 hours = (minsTotal / 60); 
                        UInt64 frames = (UInt64) (time % Time.OneSecond) / fps.FpsFrameDuration ();

			return frames.ToString();
                }
                
		/* Return the hour value of an SMPTE string given a Time */
		public static string SMPTEHours (Time time)
                {
                        UInt64 secsTotal = (UInt64) time.Seconds;
                        UInt64 minsTotal = secsTotal / 60;

                        UInt64 secs = secsTotal % 60;
                        UInt64 mins = (secsTotal / 60) % 60;
                        UInt64 hours = (minsTotal / 60);
			return hours.ToString();
                }

                /* Return the minute value of an SMPTE string given a Time */
		public static string SMPTEMinutes (Time time)
                {
                        UInt64 secsTotal = (UInt64) time.Seconds;
                        UInt64 minsTotal = secsTotal / 60;

                        UInt64 secs = secsTotal % 60;
                        UInt64 mins = (secsTotal / 60) % 60;
			return mins.ToString();
                }

                /* Return the seconds value of an SMPTE string given a Time */
		public static string SMPTESeconds (Time time)
                {
                        UInt64 secsTotal = (UInt64) time.Seconds;
                        UInt64 minsTotal = secsTotal / 60;

                        UInt64 secs = secsTotal % 60;
			return secs.ToString();
                }


                /* Convert to SMPTE dropping the hours */
                public static string ToSMPTE (Time time, Fraction fps)
                {
                        return ToSMPTE (time, fps, false);
                } 

                /* Convert to a short, canonical representation of duration */
                public static string ToShortString (Time t)
                {
                        if (t > Time.OneSecond * 60) {
                                UInt64 mins = (UInt64) t.Seconds / 60;
                                UInt64 secs = (UInt64) t.Seconds - mins * 60;
                                return String.Format ("{0} {1} {2} {3}", 
                                                      mins,
                                                      minSS,
                                                      secs,
                                                      secSS);
                        }
                        
                        if (t > Time.OneSecond + (Time.OneSecond / 2))
                                return String.Format ("{0} {1}", (UInt64) (t.Seconds),
                                                      secSS);
                        
                        if (t >= Time.OneSecond)
                                return String.Format ("1 {0}", secSS);
                        
                        return String.Format ("~" + "0 {0}", secSS);
                }

                public static string ToLongString (Time t)
                {
                        if (t > Time.OneSecond * 60) {
                                UInt64 mins = (UInt64) t.Seconds / 60;
                                UInt64 secs = (UInt64) t.Seconds - mins * 60;
                                return String.Format ("{0} {1} {2} {3}", 
                                                      mins,
                                                      (mins == 1) ? minuteSS : minutesSS,
                                                      secs,
                                                      (secs == 1) ? secondSS : secondsSS);
                        }
                        
                        if (t > Time.OneSecond + (Time.OneSecond / 2))
                                return String.Format ("{0} {1}", (UInt64) (t.Seconds),
                                                      (((int) t.Seconds) == 1) ? secondSS : secondsSS);
                        
                        if (t >= Time.OneSecond)
                                return String.Format ("1 {0}", secondSS);
                        
                        return String.Format ("~" + "0 {0}", secondSS);
                }
                
                /* Convert time to pixel amount according to pixels-per-sec */
                public static int ToPixels (Time t, double pixelspersec)
                {
                        return (int) (t.Seconds * pixelspersec);
                        
                }
                
                /* Convert no of pixels to time according to pixels-per-sec */
                public static Time FromPixels (int pixels, double pixelspersec)
                {
                        return  Time.FromSeconds ((double) pixels / pixelspersec);
                }

		/* Get time from SMPTE formatted string */
		public static Time FromSMPTE (string smpte, Fraction fps)
		{
			Time p = FromSMPTEMinusFrames (smpte); 
			Time q = FromSMPTEFps (smpte, fps);
			Time total = p + q;
			return total;
		}

		/* Get fps time from SMPTE formatted string */
		public static Time FromSMPTEFps (string smpte, Fraction fps)
		{
			string formatted = StringToFormattedSMPTE (smpte);
			string[] formattedSplit = formatted.Split(new char[] {':'});
			UInt16 frames = UInt16.Parse(formattedSplit[3]);
			Time p = frames * fps.FpsFrameDuration();
			return p;
		}

		/* Convert SMPTE string to time leaving the frames out */
		public static Time FromSMPTEMinusFrames (string smpte)
		{
			string formatted = StringToFormattedSMPTE (smpte);
			string[] formattedSplit = formatted.Split(new char[] {':'});
			UInt64 hours = UInt64.Parse(formattedSplit[0]);
			UInt64 minutes = UInt64.Parse(formattedSplit[1]);
			UInt64 seconds = UInt64.Parse(formattedSplit[2]);
			UInt64 secondsFromHours = hours * 3600;
			UInt64 secondsFromMinutes = minutes * 60;
			UInt64 totalTime = seconds + secondsFromHours + secondsFromMinutes;
			Time total = Time.FromSeconds(Double.Parse(totalTime.ToString()));
			return total;
		}

		/* Convert partial/complete SMPTE string to complete 00:00:00:00 format */
		private static string StringToFormattedSMPTE (string unformatted)
		{
			char[] separator = {':'};
			int maxFields = 4;
			unformatted = unformatted.Trim();
			string[] unformattedSplit = unformatted.Split(separator);
			int numFields = unformattedSplit.Length;
			string formatted = "";

			if (numFields > maxFields)
			{
				//TODO: Handle this graciously
				throw new FormatException ("This is a problem...please fix");
			}

			if (numFields == 0)
			{
				//TODO: Handle this graciously
				throw new FormatException ("This is a problem...please fix");
			}
                       	
			foreach (string nums in unformattedSplit)
			{
				if ( nums.Length > 2 )
				{
					throw new FormatException ("This is an issue as well");
				}
			}

			foreach (string nums in unformattedSplit)
			{
				if ( ! StringFu.IsDigit (nums) )
				{
					throw new FormatException ("This is a format issue...please fix it");
				}
			}

			switch (numFields)
			{
                                
				case 1:
					formatted = String.Format ("00:00:{0:##}:00",
                                                                   unformattedSplit[0]);
					break;

				case 2:
					formatted = String.Format ("00:{0:##}:{1:##}:00",
                                                                   unformattedSplit[0],
                                                                   unformattedSplit[1]);
					break;

				case 3:
					formatted = String.Format ("{0:##}:{1:##}:{2:##}:00",
                                                                   unformattedSplit[0],
                                                                   unformattedSplit[1],
                                                                   unformattedSplit[2]);
					break;
                                        
				case 4:
					formatted = String.Format ("{0:##}:{1:##}:{2:##}:{3:##}",
                                                                   unformattedSplit[0],
                                                                   unformattedSplit[1],
                                                                   unformattedSplit[2],
                                                                   unformattedSplit[3]);
					break;

				default:
					break;
			}
			return formatted;
		}


        }
                
}
