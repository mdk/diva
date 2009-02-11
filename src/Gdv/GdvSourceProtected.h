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

struct                          _GdvSourceProtected
{
        GdvTime Length;                      // Length of the source
        
        GdvVideoFormat *VideoFormat;         // The video format
        GdvAudioFormat *AudioFormat;         // The audio format
        
        gboolean HasVideo;                   // If this source has video
        gboolean HasAudio;                   // If this source has audio
        gboolean Consistent;                 // If the source is consistent
                                             // (video/audio is contolled by one element)
};

gboolean                        gdv_source_set_audioformat (GdvSource *this, GdvAudioFormat *aformat);

gboolean                        gdv_source_set_videoformat (GdvSource *this, GdvVideoFormat *vformat);

gboolean                        gdv_source_set_audio_target (GdvSource *this, GstPad *pad);

gboolean                        gdv_source_set_video_target (GdvSource *this, GstPad *pad);

gboolean                        gdv_source_set_length (GdvSource *this, GdvTime length);

gboolean                        gdv_source_send_event_stock (GdvSource *this, GstEvent *event);
