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

/* Stuff that represent a MediaItem (typically a file imported into the
 * project */

namespace Diva.Core {

        using System;
        using System.Collections;
        using System.Collections.Generic;        
        using Gdv;
        using Gtk;
        using Util;
        using Basics;
        
        public class MediaItemStuff : Stuff, IBoilable {

                // Fields ///////////////////////////////////////////////////
                
                MediaItem mediaItem; 
                                
                // Properties ///////////////////////////////////////////////
                
                public MediaItem MediaItem {
                        get { return mediaItem; }
                }
                
                public override bool Generic { 
                        get { return false; }
                }
                
                public override bool Border { 
                        get { 
                                if (mediaItem is IThumbnailable) {
                                        if ((mediaItem as IThumbnailable).Thumbnail != null)
                                                return true;
                                        else
                                                return false;
                                } else
                                        return false;
                        }
                        
                }
                
                public override string Major {
                        get { return mediaItem.Name; }
                }

                public override string Minor {
                        get {
                                string minor = String.Empty;
                                
                                if (mediaItem is ITimeable) 
                                        minor = TimeFu.ToShortString ((mediaItem as ITimeable).Length);

                                if (mediaItem.HasVideo)
                                        minor += " " + mediaItem.VideoFormat;
                                else
                                        minor += " " + mediaItem.AudioFormat;

                                return minor;
                        }
                }
                
                public override string Name {
                        get {
                                return mediaItem.Name;
                        }
                }
                
                public override Gdk.Pixbuf Pixbuf {
                        get {
                                if (mediaItem is IThumbnailable) {
                                        if ((mediaItem as IThumbnailable).Thumbnail != null)
                                                return (mediaItem as IThumbnailable).Thumbnail;
                                        else
                                                return IconFu.GetStockIcon ("stock_unknown", IconSize.Dialog);
                                } else {
                                        if (mediaItem is AudioItem)
                                                return IconFu.GetStockIcon ("gnome-mime-audio", IconSize.Dialog);
                                        
                                        return IconFu.GetStockIcon ("stock_unknown", IconSize.Dialog);
                                }
                        }
                }

                public override Gdk.Pixbuf DragPixbuf {
                        get {
                                if (mediaItem is IThumbnailable) {
                                        return (mediaItem as IThumbnailable).SmallThumbnail;
                                } else {
                                        if (mediaItem is AudioItem)
                                                return IconFu.GetStockIcon ("gnome-mime-audio", IconSize.Button);
                                        
                                        return IconFu.GetStockIcon ("stock_unknown", IconSize.Button);
                                }
                        }
                }
                
                // Public methods /////////////////////////////////////////////

                /* CONSTRUCTOR */
                public MediaItemStuff (MediaItem item)
                {
                        this.mediaItem = item;
                }

                /* CONSTRUCTOR */
                public MediaItemStuff (ObjectContainer container, IBoilProvider provider)
                {
                        ParseContainer (container, provider);
                        mediaItem = (MediaItem) container.FindRef ("mediaitem").ToObject (provider);
                }

                /* IBoilable */
                public override List <object> GetDepObjects ()
                {
                        List <object> lst = base.GetDepObjects ();
                        lst.Add (mediaItem);

                        return lst;
                }

                /* IBoilable */
                public override void Boil (ObjectContainer container, IBoilProvider provider)
                {
                        base.Boil (container, provider);
                        container.Add (new RefParameter ("mediaitem", mediaItem, provider));
                }
                        

        }
        
}
