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

/* Token is used for schedluing various jobs -- like frame thumnails generating, 
 * etc. Tokens are quaranteed to be unique during a single application run. Two 
 * token "stacks" are provided -- "token" for each individual job and "tokenset" to 
 * mark requests that have something in common (like -- a request for several frames
 * that are displayed in the application in one go.
 * Jobs are schedlued/aborted/identified by tokens
 */

#include "GdvToken.h"

/* Individual token for each job (stack) */
static                  GdvToken CurrentToken           = 1;

/* A token to mark several requests (stack) */
static                  GdvToken CurrentSetToken        = 1;

/* To isolate all operations on token stacks */
static                  GStaticMutex TokenMutex         = G_STATIC_MUTEX_INIT;

/* Get a brand new, unique token */
GdvToken                gdv_token_get_next_token ()
{
        g_static_mutex_lock (&TokenMutex);
        CurrentToken++;
        GDV_DEBUG_EXT ("Getting a new token: %lld", CurrentToken);
        g_static_mutex_unlock (&TokenMutex);
        return CurrentToken;
}

/* Get a brand new, unique token set */
GdvToken                gdv_token_get_next_set_token ()
{
        g_static_mutex_lock (&TokenMutex);
        CurrentSetToken++;
        GDV_DEBUG_EXT ("Getting a new set token: %lld", CurrentSetToken);
        g_static_mutex_unlock (&TokenMutex);
        return CurrentSetToken;
}

/* Check if this is a valid token or "no token" */
gboolean                gdv_token_is_valid (GdvToken token)
{
        if (token != 0) 
                return TRUE;
        else
                return FALSE;
}

/* Check if two tokens are equal */
gboolean                gdv_token_equals (GdvToken a, GdvToken b)
{
        if (a == b)
                return TRUE;
        else
                return FALSE;
}
