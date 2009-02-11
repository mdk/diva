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

/* Jobs are used to handle certain tasks that are handled in a special processing
 * thread. A typicall job is a frame thumbnail request. Jobs can be used to perform
 * many tasks, and this implementation is somewhat abstract. 
 * Jobs can be used to control other jobs in the queue (ie. remove jobs by token set, 
 * stop all processing, etc. 
 */

#include "GdvJob.h"

/* Create a new job object, not really filled. As a basis for more specific 
 * jobs. If set is not given (0) a new set is created. Token automatically
 * increased */
GdvJob                  *gdv_job_new (GdvToken set, GdvJobType type)
{
        GdvJob *this = g_new (GdvJob, 1);
        if (this == NULL)
                return NULL;
        
        this->Type = type;
        this->Token = gdv_token_get_next_token ();
        if (GDV_IS_TOKEN (set))
                this->SetToken = set;
        else
                this->SetToken = gdv_token_get_next_set_token ();
        
        this->Custom = NULL;
        this->WorkFunc = NULL;
        this->IdleFunc = NULL;
        this->FreeFunc = NULL;
        this->Success = TRUE;
 
        GDV_DEBUG_EXT ("Created new job. Token: %lld Set: %lld Type: %d", 
                       this->Token, this->SetToken, this->Type);
        return this;
}

/* Free job memory */
void                    gdv_job_free (GdvJob *this)
{
        GDV_DEBUG_EXT ("Disposing job. Token: %lld Set: %lld Type: %d", 
                       this->Token, this->SetToken, this->Type);
        g_assert (this != NULL);
        
        if (this->FreeFunc != NULL)
                this->FreeFunc (this->Custom);
        
        g_free (this);
}

