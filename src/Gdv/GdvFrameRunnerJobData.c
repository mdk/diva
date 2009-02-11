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

#include "GdvFrameRunnerJobData.h"

/* Initialize a new structure. It TAKES the dimensions and REFS the runner */
GdvFrameRunnerJobData   *gdv_framerunnerjobdata_new (GdvTime t, GdvFrameDimensions *dimensions,
                                                     GdvFrameRunner *runner)
{
        g_assert (dimensions != NULL);
        g_assert (GDV_IS_FRAMERUNNER (runner) == TRUE);
        
        GdvFrameRunnerJobData *this = g_new (GdvFrameRunnerJobData, 1);
        if (this == NULL)
                return NULL;
        
        this->AtTime = t;
        this->Dimensions = dimensions;
        this->Runner = runner;
        this->Pixbuf = NULL;
        
        g_object_ref (G_OBJECT (runner));
        
        return this;
}

/* Free the structure */
void                    gdv_framerunnerjobdata_free (GdvFrameRunnerJobData *this)
{
        g_assert (this != NULL);
        
        if (this->Pixbuf != NULL)
                gdk_pixbuf_unref (this->Pixbuf);
        
        if (this->Dimensions != NULL)
                gdv_framedimensions_free (this->Dimensions);
        
        if (this->Runner != NULL)
                g_object_unref (G_OBJECT (this->Runner));
}
