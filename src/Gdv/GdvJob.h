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

#ifndef __GDVJOB_H__
#define __GDVJOB_H__

#include <gst/gst.h>
#include "GdvConst.h"
#include "GdvTypes.h"
#include "GdvToken.h"

enum _GdvJobType {
        GDV_JOB_REQUEST,               /* A typicall request (frame, etc.) */
        GDV_JOB_CLEAR_SET,             /* A request to remove a given set from queue */
        GDV_JOB_FINISH,                /* A request to finish all processing */
};        

struct _GdvJob {
        GdvJobType Type;                   /* Type of the job. See the .c file */
        GdvToken Token;                    /* The token of this job */
        GdvToken SetToken;                 /* The token of the set this job belongs to */
                
        gpointer Custom;                   /* Custom data struct */
        GSourceFunc IdleFunc;              /* Function fired upon completition */
        GdvJobWorkFunc WorkFunc;           /* A function to perform the work */
        GdvJobFreeFunc FreeFunc;           /* Function used to dispose the results/job */
        gboolean Success;                  /* If the job was successfull or not */
};

GdvJob                  *gdv_job_new                    (GdvToken set, GdvJobType type);

void                    gdv_job_free                    (GdvJob *this);

#endif
