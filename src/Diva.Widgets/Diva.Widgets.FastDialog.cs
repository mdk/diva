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

namespace Diva.Widgets {
        
        using System;
	using Gtk;
        using Mono.Unix;

        public static class FastDialog {

                // Translatable ////////////////////////////////////////////////
                
                readonly static string notImplementedHeaderSS =
                        Catalog.GetString ("Feature not yet implemented");

                readonly static string notImplementedMessageASS =
                        Catalog.GetString ("Please visit http://www.diva-project.org/ for more information");

                readonly static string notImplementedMessageBSS =
                        Catalog.GetString ("Please visit http://www.diva-project.org/ticket/{0} for more information");
                
		/* A standard Gnome-style Warning dialog with an Ok button */
                public static ResponseType WarningOk (Window parent, string header, string message)
                {
                        string markUp = String.Format ("<big><b>{0}</b></big>\n\n{1}", header, message);
                        MessageDialog dialog = new MessageDialog (parent, DialogFlags.Modal, 
                                                                  MessageType.Warning, ButtonsType.Ok,
                                                                  true, markUp);
                        ResponseType response = (ResponseType) dialog.Run ();
                        dialog.Destroy ();
                        return response;
                }
                
                /* A standard Gnome-style Warning dialog with an Yes/No buttons */
                public static ResponseType WarningYesNo (Window parent, string header, string message)
                {
                        string markUp = String.Format ("<big><b>{0}</b></big>\n\n{1}", header, message);
                        MessageDialog dialog = new MessageDialog (parent, DialogFlags.Modal, 
                                                                  MessageType.Warning, ButtonsType.YesNo,
                                                                  true, markUp);
                        
                        ResponseType response = (ResponseType) dialog.Run ();
                        dialog.Destroy ();
                        return response;
                }
                
                /* A standard Gnome-style Error dialog with an Ok button */
                public static ResponseType ErrorOk (Window parent, string header, string message)
                {
                        string markUp = String.Format ("<big><b>{0}</b></big>\n\n{1}", header, message);
                        MessageDialog dialog = new MessageDialog (parent, DialogFlags.Modal, 
                                                                  MessageType.Error, ButtonsType.Ok,
                                                                  true, markUp);
                        
                        ResponseType response = (ResponseType) dialog.Run ();
                        dialog.Destroy ();
                        return response;
                }
		/* Not implemented error dialog with an Ok button */
		public static ResponseType InfoOkNotImplemented (Window parent)
                {
                        string markUp = String.Format ("<big><b>{0}</b></big>\n\n{1}",
                                                       notImplementedHeaderSS,
                                                       notImplementedMessageASS);
                        
                        MessageDialog dialog = new MessageDialog (parent, DialogFlags.Modal, 
                                                                  MessageType.Info, ButtonsType.Ok,
                                                                  true, markUp);
                        
                        ResponseType response = (ResponseType) dialog.Run ();
                        dialog.Destroy ();
                        return response;
                } 
	
		/* Not implemented error dialog with an Ok button and ticket ID */
		public static ResponseType InfoOkNotImplementedWithTicketID (Window parent, int ticketID)
                {
                        string message = String.Format (notImplementedMessageBSS, ticketID);
                        string markUp = String.Format ("<big><b>{0}</b></big>\n\n{1}", notImplementedHeaderSS, message);
                        MessageDialog dialog = new MessageDialog (parent, DialogFlags.Modal, 
                                                                  MessageType.Info, ButtonsType.Ok,
                                                                  true, markUp);
                        
                        ResponseType response = (ResponseType) dialog.Run ();
                        dialog.Destroy ();
                        return response;
               } 
	}
        
}
                
                
