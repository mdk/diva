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

#ifndef __DIVA_CAIRO_TYPES_H__
#define __DIVA_CAIRO_TYPES_H__

// Is //////////////////////////////////////////////////////////////////////////

#define                         DIVA_CAIRO_IS_GRAPHICS(obj)     (obj != NULL && obj->Cairo != NULL)

// Structs /////////////////////////////////////////////////////////////////////

typedef struct                  _DivaCairoGraphics              DivaCairoGraphics;

// Enums ///////////////////////////////////////////////////////////////////////

enum _DivaCairoOutline {
        DIVA_CAIRO_OUTLINE_NONE = 0,
        DIVA_CAIRO_OUTLINE_BLACK = 1,
        DIVA_CAIRO_OUTLINE_WHITE = 2
} typedef DivaCairoOutline;

enum _DivaCairoCut {
        DIVA_CAIRO_CUT_NONE = 0, 
        DIVA_CAIRO_CUT_LEFT = 1, 
        DIVA_CAIRO_CUT_RIGHT = 2,
} typedef DivaCairoCut;

enum _DivaCairoState {
        DIVA_CAIRO_STATE_NORMAL = 0, 
        DIVA_CAIRO_STATE_ACTIVE = 1, 
        DIVA_CAIRO_STATE_DRAG = 2
} typedef DivaCairoState;

#endif
