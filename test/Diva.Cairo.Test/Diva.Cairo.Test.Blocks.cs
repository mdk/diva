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
        
        public static class Blocks {
                
                // Fields //////////////////////////////////////////////////////
                
                static readonly double radius = 11;

                // Private methods /////////////////////////////////////////////
                
                public static void DrawSolids (Graphics gr)
                {
                        Cairo.Draw.RoundedSolid (gr, 10, 10, 300, 220, radius,
                                                 Gdv.Color.FromTango (ColorTango.Chameleon2));
                
                        Cairo.Draw.RoundedSolid (gr, 320, 10, 300, 220, radius, 
                                                    Gdv.Color.FromTango (ColorTango.Butter2));
                        
                        Diva.Cairo.Draw.RoundedSolid (gr, 10, 240, 300, 220, radius, 
                                                      Gdv.Color.FromTango (ColorTango.Plum2));
                        
                        Diva.Cairo.Draw.RoundedSolid (gr, 320, 240, 145, 105, radius, 
                                                      Gdv.Color.FromTango (ColorTango.Orange2));
                        
                        Diva.Cairo.Draw.RoundedSolid (gr, 320 + 145 + 10, 240, 145, 105, radius,
                                                      Gdv.Color.FromTango (ColorTango.SkyBlue2));
                        
                        Diva.Cairo.Draw.RoundedSolid (gr, 320, 240 + 105 + 10, 145, 105, radius,
                                                      Gdv.Color.FromTango (ColorTango.Aluminium2));
                        
                        Diva.Cairo.Draw.RoundedSolid (gr, 320 + 145 + 10, 240 + 105 + 10, 145, 105, radius,
                                                      Gdv.Color.FromTango (ColorTango.ScarletRed2));
                }
                
                public static void DrawGradients (Graphics gr)
                {
                        Cairo.Draw.RoundedGradient (gr, 10, 10, 300, 220, radius,
                                                    Gdv.Color.FromTango (ColorTango.Chameleon2),
                                                    Gdv.Color.FromTango (ColorTango.Chameleon3));
                        
                        Cairo.Draw.RoundedGradient (gr, 320, 10, 300, 220, radius, 
                                                    Gdv.Color.FromTango (ColorTango.Butter2),
                                                    Gdv.Color.FromTango (ColorTango.Butter3));
                        
                        Diva.Cairo.Draw.RoundedGradient (gr, 10, 240, 300, 220, radius, 
                                                         Gdv.Color.FromTango (ColorTango.Plum2),
                                                         Gdv.Color.FromTango (ColorTango.Plum3));
                        
                        Diva.Cairo.Draw.RoundedGradient (gr, 320, 240, 145, 105, radius, 
                                                         Gdv.Color.FromTango (ColorTango.Orange2),
                                                         Gdv.Color.FromTango (ColorTango.Orange3));
                        
                        Diva.Cairo.Draw.RoundedGradient (gr, 320 + 145 + 10, 240, 145, 105, radius,
                                                         Gdv.Color.FromTango (ColorTango.SkyBlue2),
                                                         Gdv.Color.FromTango (ColorTango.SkyBlue3));
                        
                        Diva.Cairo.Draw.RoundedGradient (gr, 320, 240 + 105 + 10, 145, 105, radius,
                                                         Gdv.Color.FromTango (ColorTango.Aluminium2),
                                                         Gdv.Color.FromTango (ColorTango.Aluminium3));
                        
                        Diva.Cairo.Draw.RoundedGradient (gr, 320 + 145 + 10, 240 + 105 + 10, 145, 105, radius,
                                                         Gdv.Color.FromTango (ColorTango.ScarletRed2),
                                                         Gdv.Color.FromTango (ColorTango.ScarletRed3));
                }
                
                public static void DrawOutlines (Graphics gr, CairoOutline outline)
                {
                        Cairo.Draw.RoundedOutline (gr, 10, 10, 300, 220, radius, outline);
                
                        Cairo.Draw.RoundedOutline (gr, 320, 10, 300, 220, radius, outline);
                        
                        Diva.Cairo.Draw.RoundedOutline (gr, 10, 240, 300, 220, radius, outline);
                        
                        Diva.Cairo.Draw.RoundedOutline (gr, 320, 240, 145, 105, radius, outline);
                        
                        Diva.Cairo.Draw.RoundedOutline (gr, 320 + 145 + 10, 240, 145, 105, radius, outline);

                        Diva.Cairo.Draw.RoundedOutline (gr, 320, 240 + 105 + 10, 145, 105, radius, outline);
                        
                        Diva.Cairo.Draw.RoundedOutline (gr, 320 + 145 + 10, 240 + 105 + 10, 145, 105, radius, outline);
                }
                
        }
        
}
