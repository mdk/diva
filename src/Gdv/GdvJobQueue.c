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

#include "GdvJobQueue.h"

/* Create a new JobQueue. Perhaps that much error checking is too much? */
GdvJobQueue             *gdv_jobqueue_new (void)
{
        GDV_DEBUG_EXT ("Creating a new GdvJobQueue", NULL);
        GdvJobQueue *this = NULL;
        
        this = g_new (GdvJobQueue, 1);
        if (this == NULL)
                goto finish;
        
        this->Queue = NULL;
        this->Cond = NULL;
        this->Mutex = NULL;
        
        this->Queue = g_queue_new ();
        if (this->Queue == NULL)
                goto finish;
        
        this->Cond = g_cond_new ();
        if (this->Cond == NULL)
                goto finish;
        
        this->Mutex = g_mutex_new ();
        if (this->Mutex == NULL)
                goto finish;
        
        return this;
        g_assert_not_reached ();
 
        finish:
        GDV_WARNING_EXT ("Failed to create a GdvJobQueue", NULL);
        // Fallback
        if (this != NULL) {
                if (this->Queue != NULL)
                        g_queue_free (this->Queue);
                if (this->Cond != NULL)
                        g_cond_free (this->Cond);
                if (this->Mutex != NULL)
                        g_mutex_free (this->Mutex);
                g_free (this);
        }
        return NULL;
}

/* Pop an item from the begining of the queue. Will BLOCK if no item
 * is ready! */
GdvJob                  *gdv_jobqueue_pop_head (GdvJobQueue *this)
{
        g_assert (this != NULL);
        GDV_DEBUG_EXT ("Poping a job from the start of the queue", NULL);
        
        g_mutex_lock (this->Mutex);
        GdvJob *job = g_queue_pop_head (this->Queue);
        
        if (job != NULL) {
                // We can finish here...
                g_mutex_unlock (this->Mutex);
                return job;
        }
        
        // We need to wait...
        g_cond_wait (this->Cond, this->Mutex);
        job = g_queue_pop_head (this->Queue);
        
        g_assert (job != NULL);
        
        g_mutex_unlock (this->Mutex);
        return job;
}

/* Pop an item from the begining of the queue. Will BLOCK if no item
 * is ready! */
void                    gdv_jobqueue_push_head (GdvJobQueue *this, GdvJob *job)
{
        g_assert (this != NULL);
        GDV_DEBUG_EXT ("Pushing a job to the start of the queue", NULL);
        
        g_mutex_lock (this->Mutex);
        g_queue_push_head (this->Queue, job);
        g_cond_signal (this->Cond);
        g_mutex_unlock (this->Mutex);
}
