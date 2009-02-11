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
        
        public sealed class StatusHBox : HBox {
                
                // Fields //////////////////////////////////////////////////////
                
                Model.Root modelRoot = null;
                ZoomToComboBox zoomToComboBox = null;
                CounterButton counterButton = null;
                MessageArea messageArea = null;
                TogglesHBox togglesHBox = null;
                StripeHintDrawingArea stripeHint = null;
                MouseOverTimeLabel mouseOverTimeLabel = null;
		JumpToWindow jumpToWindow = null;

                // Public methods //////////////////////////////////////////////
                
                /* CONSTRUCTOR */
                public StatusHBox (Model.Root root) : base (false, 6)
                {
                        modelRoot = root;
                        
                        // Zoom to combo
                        zoomToComboBox = new ZoomToComboBox (root);
                        PackStart (zoomToComboBox, false, false, 0);
                        
                        // Counter
                        counterButton = new CounterButton (root);
                       	counterButton.Clicked += OnCounterButtonClicked;
		        PackStart (counterButton, false, false, 0);

			// MoveToHBox
			// MoveToHBox moveToHBox = new MoveToHBox (modelRoot);
			// PackStart (moveToHBox, false, false, 0);
                        
                        // Message area
                        messageArea = new MessageArea (root);
                        PackStart (messageArea, true, true, 0);
                        
                        // Toggles
                        togglesHBox = new TogglesHBox (root);
                        PackEnd (togglesHBox, false, false, 0);

                        // Separator
                        PackEnd (new VSeparator (), false, false, 0);

                        // Stripe hint
                        stripeHint = new StripeHintDrawingArea (modelRoot);
                        PackEnd (stripeHint, false, false, 0);

                        // Separator
                        PackEnd (new VSeparator (), false, false, 0);
                        
                        // Mouse over time
                        mouseOverTimeLabel = new MouseOverTimeLabel (root);
                        PackEnd (mouseOverTimeLabel, false, false, 0);
                }
                
		void OnCounterButtonClicked (object o, EventArgs args)
		{
			//FIXME: actually send it the window object here instead of null
			//FIXME: Make sure only one JumpToWindow is/can be open
			jumpToWindow = new JumpToWindow (modelRoot, null);

		}


        }
        
}
       
