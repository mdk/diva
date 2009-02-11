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

namespace Diva.Cairo.Test {
        
        using System;
        using Gtk;
        using Gdv;
        
        public static class Clips {
                
                // FIelds //////////////////////////////////////////////////////

                static Gdk.Pixbuf thmb1 = new Gdk.Pixbuf (null, "thmb1.png");
                
                static Gdk.Pixbuf thmb2 = new Gdk.Pixbuf (null, "thmb2.png");
                
                // Private methods /////////////////////////////////////////////
                
                public static void DrawClips (Graphics gr)
                {
                        Cairo.ReadyMade.Clip (gr, 630 - 10 - 200, 10, 200,
                                              Gdv.Color.FromTango (ColorTango.Chameleon2),
                                              "<b>My clip</b>", CairoCut.None, CairoState.Normal);

                        Cairo.ReadyMade.Clip (gr, 630 - 10 - 400, 10 + 32 + 10, 400,
                                              Gdv.Color.FromTango (ColorTango.SkyBlue2),
                                              "<b>My music clip</b>", CairoCut.None, CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 10, 10 + 32 + 32 + 10 + 10, 306,
                                              Gdv.Color.FromTango (ColorTango.Chameleon2),
                                              "<b>Video 1</b>", CairoCut.Left, CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 10 + 305, 10 + 32 + 32 + 10 + 10, 11,
                                              Gdv.Color.FromTango (ColorTango.Chameleon2),
                                              "<b>Tin Video</b>", CairoCut.None, CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 10 + 305 + 10, 10 + 32 + 32 + 10 + 10, 305 - 11,
                                              Gdv.Color.FromTango (ColorTango.Chameleon2),
                                              "<b>Video 2</b>", CairoCut.Right, CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 10, 32 + 10 + 32 + 32 + 10 + 10 + 10, 610,
                                              Gdv.Color.FromTango (ColorTango.Butter2),
                                              "<b>A very zoomed clip</b>", CairoCut.Left | CairoCut.Right,
                                              CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 320, 32 + 10 + 32 + 32 + 10 + 10 + 10 + 32 + 10 + 32 + 10, 201,
                                              Gdv.Color.FromTango (ColorTango.Chameleon2),
                                              "<b>Thumbnailed clip</b>", 
                                              thmb1,
                                              CairoCut.None, CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 320 + 200, 32 + 10 + 32 + 32 + 10 + 10 + 10 + 32 + 10 + 32 + 10, 100,
                                              Gdv.Color.FromTango (ColorTango.Chameleon2),
                                              "<b>This clips hase a very long name</b>", 
                                              thmb2,
                                              CairoCut.Right, CairoState.Normal);
                        
                        Cairo.ReadyMade.Clip (gr, 320, 32 + 10 + 32 + 32 + 10 + 10 + 10 + 32 + 10 + 32 + 10 + 32 + 10, 300,
                                              Gdv.Color.FromTango (ColorTango.SkyBlue2),
                                              "<b>Active clip</b>", 
                                              thmb1,
                                              CairoCut.None, CairoState.Active);
                        
                        Cairo.ReadyMade.Clip (gr, 320, 32 + 10 + 32 + 32 + 10 + 10 + 10 + 32 + 10 + 32 + 10 + 32 + 10 + 32 + 10, 300,
                                              Gdv.Color.FromTango (ColorTango.SkyBlue2),
                                              "<b>Dragged clip</b>", 
                                              thmb2,
                                              CairoCut.None, CairoState.Drag);
                        
                }
                
        }
        
}
