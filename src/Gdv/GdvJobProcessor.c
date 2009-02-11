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

#include "GdvJobProcessor.h"

/* To prevent processor from being initialized twice */
static gboolean         Initialized                     = FALSE;

/* Queue with jobs */
static GdvJobQueue      *Queue                          = NULL;

/* Initialize the jobprocessor and start the background thread */
void                    gdv_jobprocessor_init (void)
{
        GDV_DEBUG_EXT ("Initializing GdvJobProcessor", NULL);
        
        if (Initialized == TRUE) {
                GDV_WARNING_EXT ("Trying to initialize GdvJobProcessor but it's "
                                 "already initialized!", NULL);
                return;
        }
        
        Queue = gdv_jobqueue_new ();
        g_assert (Queue != NULL);
        
        g_thread_create ((GThreadFunc) gdv_jobprocessor_thread_start, NULL, 
                         TRUE, NULL);
}

/* The main loop */
void                    gdv_jobprocessor_thread_start (void)
{
        GDV_DEBUG_EXT ("GdvJobProcessor thread starting work", NULL);
        g_assert (Queue != NULL);
        
        while (TRUE) {
                GdvJob *job = gdv_jobqueue_pop_head (Queue);
                g_assert (job != NULL);
                
                GDV_DEBUG_EXT ("Got job %lld:%lld. Parsing...", job->Token, job->SetToken);
                gdv_jobprocessor_parse_job (job);
        }
}

/* To get the global Queue */
GdvJobQueue             *gdv_jobprocessor_get_queue (void)
{
        g_assert (Queue != NULL);
        return Queue;
}

/* Actually execute the job. Returns false if the loop should be stopped */
gboolean                gdv_jobprocessor_parse_job (GdvJob *job)
{
        g_assert (job != NULL);
        
        switch (job->Type) {
                
                case GDV_JOB_REQUEST:
                // Work
                // FIXME: Debug messages
                if (job->WorkFunc != NULL)
                        job->Success = job->WorkFunc (job->Custom);
                else 
                        GDV_WARNING_EXT ("Got request job without a workfunc!", NULL);
                
                // Idle
                // FIXME: Debug messages
                if (job->IdleFunc != NULL)
                        g_idle_add (job->IdleFunc, job);
                else
                        gdv_job_free (job);
                break;
                
                default:
                GDV_WARNING_EXT ("Got unknown job! Ignoring...", NULL);
                break;
        }
        
        return TRUE;
}


        
        
