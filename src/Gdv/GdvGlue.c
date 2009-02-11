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

#include "GdvGlue.h"

/* Extract a pointer from a GValue holding a boxed type */
gpointer                        gdv_glue_gvalue_get_boxed (GValue *value)
{
        g_assert (value != NULL);
        
        gpointer ret = g_value_get_boxed (value);
        
        g_assert (ret != NULL);
        return ret;
}

/* Set the value to contain the boxed type */
void                            gdv_glue_gvalue_set_boxed (GValue *value, gpointer boxed)
{
        g_assert (boxed != NULL);
        g_assert (value != NULL);
        
        g_value_set_boxed (value, boxed);        
}

/* Get a pointer to the error message. Not to mess with structs on the
 * unmanaged side */
gchar*                          gdv_glue_gerror_get_string (GError *error)
{
        g_return_val_if_fail (error != NULL, NULL);
        return error->message;
}

/* Free the memory allocated by the error */
void                            gdv_glue_gerror_free (GError *error)
{
        g_return_if_fail (error != NULL);
        g_error_free (error);
}

        
