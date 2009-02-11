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

#ifndef __GDVITEMGENERATOR_H__
#define __GDVITEMGENERATOR_H__

#include <gst/gst.h>
#include "GdvTypes.h"
#include "GdvConst.h"
#include "GdvMediaItem.h"
#include "GdvSource.h"
#include "GdvInspector.h"
#include "GdvDecodeBin.h"
#include "GdvAVItem.h"
#include "GdvAudioItem.h"
#include "GdvPhotoItem.h"
#include "GdvProjectFormat.h"

// This is for the C# bindings

enum                            _GdvItemType
{
        GDV_ITEM_NONE,
        GDV_ITEM_AV,
        GDV_ITEM_AUDIO,
        GDV_ITEM_PHOTO
};

enum                            _GdvSourceType
{
        GDV_SOURCE_NONE,
        GDV_SOURCE_DECODEBIN,
        GDV_SOURCE_DVSOURCE,
        GDV_SOURCE_WAVSOURCE,
        GDV_SOURCE_SHEEPSOURCE        
};

GdvMediaItem*                   gdv_itemgenerator_from_source (GdvSource *source);

GdvMediaItem*                   gdv_itemgenerator_from_inspector (GdvProjectFormat *format, GdvInspector *inspector);

GdvItemType                     gdv_itemgenerator_get_item_type (GdvMediaItem *item);

GdvSourceType                   gdv_itemgenerator_get_source_type (GdvSource *source);

#endif
