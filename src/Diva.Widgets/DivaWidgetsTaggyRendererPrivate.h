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

#define                         DIVA_WIDGETS_TAGGY_RENDERER_GET_CLASS(obj) ((DivaWidgetTaggyRenderClass *) G_OBJECT_GET_CLASS(obj))

struct                          _DivaWidgetsTaggyRendererPrivate
{
        gchar *TagString;   // A string tag
        GSList *TagList;    // Contains gchars
        gchar *MajorString;
        gchar *MinorString;
};

enum
{
        ARG_0,
        ARG_TAGS,
        ARG_MAJOR,
        ARG_MINOR,
};

static void                     diva_widgets_taggy_renderer_class_init (DivaWidgetsTaggyRendererClass *klass);

static void                     diva_widgets_taggy_renderer_init (DivaWidgetsTaggyRenderer *this);

static void                     diva_widgets_taggy_renderer_dispose (DivaWidgetsTaggyRenderer *this);

static void                     diva_widgets_taggy_renderer_finalize (DivaWidgetsTaggyRenderer *this);

static gboolean                 private_init (DivaWidgetsTaggyRendererPrivate *private);

static void                     private_dispose (DivaWidgetsTaggyRendererPrivate *private);

static void                     dispose_tag_list (GSList *list);

static gchar*                   get_next_tag (DivaCairoGraphics *cairo, GSList **list,
                                              gint32 piexelsleft, gint32 *outwidth, gint32 *outheight);
