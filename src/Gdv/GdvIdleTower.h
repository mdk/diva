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

#ifndef __GDVIDLETOWER_H__
#define __GDVIDLETOWER_H__

#include <gst/gst.h>

#include "GdvTypes.h"
#include "GdvConst.h"

typedef struct                  _GdvIdleTowerPrivate GdvIdleTowerPrivate;

typedef void                    (*GdvIdleTowerActionFunc) (gpointer data);

typedef gboolean                (*GdvIdleTowerDisposeFunc) (gpointer data);

typedef void                    (*GdvIdleTowerFinalizeFunc) (gpointer data);

struct                          _GdvIdleTower
{
        GObject Parent;
        GdvIdleTowerPrivate *Private;
};

struct                          _GdvIdleTowerClass
{
        GObjectClass ParentClass;
};

GType                           gdv_idletower_get_type (void);

GdvIdleTower*                   gdv_idletower_new (void);

gint32                          gdv_idletower_add_item (GdvIdleTower *this,
                                                        gint32 gid, GdvIdleTowerActionFunc action,
                                                        GdvIdleTowerDisposeFunc dispose, GdvIdleTowerFinalizeFunc finalize,
                                                        gpointer data);

void                            gdv_idletower_wipe_and_finalize (GdvIdleTower *this);

int                             gdv_idletower_abort_by_gid (GdvIdleTower *this, gint32 gid);

gboolean                        gdv_idletower_has_gid (GdvIdleTower *this, gint32 gid);

#endif
