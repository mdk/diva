///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// MIT X11 license, Copyright (c) 2005-2006 by:                              //
//                                                                           //
// Authors:                                                                  //
//      Michael Dominic K. <michaldominik@gmail.com>                         //
//      Senko Rasic <senko@senko.net>                                        //
//                                                                           //
// Permission is hereby granted, free of charge, to any person obtaining a   //
// copy of this software and associated documentation files (the             //
// "Software"), to deal in the Software without restriction, including       //
// without limitation the rights to use, copy, modify, merge, publish,       //
// distribute, sublicense, and/or sell copies of the Software, and to permit //
// persons to whom the Software is furnished to do so, subject to the        //
// following conditions:                                                     //
//                                                                           //
// The above copyright notice and this permission notice shall be included   //
// in all copies or substantial portions of the Software.                    //
//                                                                           //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS   //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN //
// NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,  //
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR     //
// OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE //
// USE OR OTHER DEALINGS IN THE SOFTWARE.                                    //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////
 
#ifndef __GDVTYPES_H__
#define __GDVTYPES_H__
 
#include <gst/gst.h>

/* Basic types */

typedef                         guint64                         GdvTime;

typedef struct                  _GdvTimeSpan                    GdvTimeSpan;

typedef struct                  _GdvFraction                    GdvFraction;

typedef                         gdouble                         GdvFps;

typedef                         guint64                         GdvToken;

/* Constant values */

#define                         GDV_SECOND                      GST_SECOND

#define                         GDV_MSECOND                     (GDV_SECOND / 1000)

#define                         GDV_ZEROTIME                    0

#define                         GDV_INFINITY                    G_MAXINT64

#define                         GDV_INFINITY_SEEK               -1

#define                         GDV_NO_TOKEN                    0

/* Structs */

typedef struct                  _GdvAudioFormat                 GdvAudioFormat;

typedef struct                  _GdvVideoFormat                 GdvVideoFormat;

typedef struct                  _GdvVideoBackend                GdvVideoBackend;

typedef struct                  _GdvProjectFormat               GdvProjectFormat;

typedef struct                  _GdvFrameSink                   GdvFrameSink;

typedef struct                  _GdvCacheEntry                  GdvCacheEntry;

typedef struct                  _GdvCacheKey                    GdvCacheKey;

typedef struct                  _GdvCacheJob                    GdvCacheJob;

typedef struct                  _GdvFrameCache                  GdvFrameCache;

typedef struct                  _GdvFrameRunner                 GdvFrameRunner;

typedef struct                  _GdvCacheManager                GdvCacheManager;

typedef struct                  _GdvBackend                     GdvBackend;

typedef struct                  _GdvPlugin                      GdvPlugin;

typedef struct                  _GdvBackendPlugin               GdvBackendPlugin;

typedef struct                  _GdvSheep                       GdvSheep;

typedef struct                  _GdvBlackSheep                  GdvBlackSheep;

typedef struct                  _GdvSilentSheep                 GdvSilentSheep;

typedef struct                  _GdvImageSheep                  GdvImageSheep;

typedef struct                  _GdvSource                      GdvSource;

typedef struct                  _GdvMediaItem                   GdvMediaItem;

typedef struct                  _GdvAVClip                      GdvAVClip;

typedef struct                  _GdvSoundItem                   GdvSoundItem;

typedef struct                  _GdvAVItem                      GdvAVItem;

typedef struct                  _GdvAudioItem                   GdvAudioItem;

typedef struct                  _GdvPhotoItem                   GdvPhotoItem;

typedef struct                  _GdvImageSink                   GdvImageSink;

typedef struct                  _GdvFrameRenderer               GdvFrameRenderer;

typedef struct                  _GdvFrameDimensions             GdvFrameDimensions;

typedef struct                  _GdvBufferSpoofer               GdvBufferSpoofer;

typedef struct                  _GdvDecodeBin                   GdvDecodeBin;

typedef struct                  _GdvAviSource                   GdvAviSource;

typedef struct                  _GdvMjpegSource                 GdvMjpegSource;

typedef struct                  _GdvVideoCompSink               GdvVideoCompSink;

typedef struct                  _GdvTwinCompSink                GdvTwinCompSink;

typedef struct                  _GdvCompSink                    GdvCompSink;

typedef struct                  _GdvAudioCompSink               GdvAudioCompSink;

typedef struct                  _GdvInspector                   GdvInspector;

typedef struct                  _GdvPixbufSink                  GdvPixbufSink;

typedef struct                  _GdvJobQueue                    GdvJobQueue;

typedef struct                  _GdvJob                         GdvJob;

typedef struct                  _GdvFrameRunnerJobData          GdvFrameRunnerJobData;

typedef struct                  _GdvColor                       GdvColor;

typedef struct                  _GdvClip                        GdvClip;

typedef struct                  _GdvTrack                       GdvTrack;

typedef struct                  _GdvPlaySink                    GdvPlaySink;

typedef struct                  GstPad                          GdvPhantomPad;

typedef struct                  GstPad                          GdvSourcePad;

typedef struct                  GstPad                          GdvClipPad;

typedef struct                  GstBuffer                       GdvVideoBuffer;

typedef struct                  GstBuffer                       GdvAudioBuffer;

typedef struct                  _GdvFilter                      GdvFilter;

typedef struct                  _GdvMetaFilter                  GdvMetaFilter;

typedef struct                  _GdvDataFilter                  GdvDataFilter;

typedef struct                  _GdvVideoDataFilter             GdvVideoDataFilter;

typedef struct                  _GdvVideoMetaFilter             GdvVideoMetaFilter;

typedef struct                  _GdvMetaIntroducer              GdvMetaIntroducer;

typedef struct                  _GdvFilterCan                   GdvFilterCan;

typedef struct                  _GdvBumperStamper               GdvBumperStamper;

typedef struct                  _GdvDvSource                    GdvDvSource;

typedef struct                  _GdvWavSource                   GdvWavSource;

typedef struct                  _GdvSheepSource                 GdvSheepSource;

typedef struct                  _GdvPipeline                    GdvPipeline;

typedef struct                  _GdvClipStore                   GdvClipStore;

typedef struct                  _GdvSingleViewBin               GdvSingleViewBin;

typedef struct                  _GdvTwinViewBin                 GdvTwinViewBin;

typedef struct                  _GdvExporter                    GdvExporter;

typedef struct                  _GdvVideoBoy                    GdvVideoBoy;

typedef struct                  _GdvAudioBoy                    GdvAudioBoy;

typedef struct                  _GdvIdleTower                   GdvIdleTower;

typedef struct                  _GdvClipGate                    GdvClipGate;

typedef struct                  _GdvCorrector                   GdvCorrector;

/* Classes */

typedef struct                  _GdvMediaItemClass              GdvMediaItemClass;

typedef struct                  _GdvAVClipClass                 GdvAVClipClass;

typedef struct                  _GdvSoundItemClass              GdvSoundItemClass;

typedef struct                  _GdvAVItemClass                 GdvAVItemClass;

typedef struct                  _GdvAudioItemClass              GdvAudioItemClass;

typedef struct                  _GdvPhotoItemClass              GdvPhotoItemClass;

typedef struct                  _GdvImageSinkClass              GdvImageSinkClass;

typedef struct                  _GdvFrameRendererClass          GdvFrameRendererClass;

typedef struct                  _GdvBufferSpooferClass          GdvBufferSpooferClass;

typedef struct                  _GdvSourceClass                 GdvSourceClass;

typedef struct                  _GdvSheepSourceClass            GdvSheepSourceClass;

typedef struct                  _GdvDecodeBinClass              GdvDecodeBinClass;

typedef struct                  _GdvBlackSheepClass             GdvBlackSheepClass;

typedef struct                  _GdvSheepClass                  GdvSheepClass;

typedef struct                  _GdvImageSheepClass             GdvImageSheepClass;

typedef struct                  _GdvSilentSheepClass            GdvSilentSheepClass;

typedef struct                  _GdvAviSourceClass              GdvAviSourceClass;

typedef struct                  _GdvMjpegSourceClass            GdvMjpegSourceClass;

typedef struct                  _GdvVideoCompSinkClass          GdvVideoCompSinkClass;

typedef struct                  _GdvTwinCompSinkClass           GdvTwinCompSinkClass;

typedef struct                  _GdvCompSinkClass               GdvCompSinkClass;

typedef struct                  _GdvAudioCompSinkClass          GdvAudioCompSinkClass;

typedef struct                  _GdvInspectorClass              GdvInspectorClass;

typedef struct                  _GdvAudioFormatClass            GdvAudioFormatClass;

typedef struct                  _GdvVideoFormatClass            GdvVideoFormatClass;

typedef struct                  _GdvProjectFormatClass          GdvProjectFormatClass;

typedef struct                  _GdvVideoBackendClass           GdvVideoBackendClass;

typedef struct                  _GdvCacheManagerClass           GdvCacheManagerClass;

typedef struct                  _GdvPixbufSinkClass             GdvPixbufSinkClass;

typedef struct                  _GdvFrameRunnerClass            GdvFrameRunnerClass;

typedef struct                  _GdvClipClass                   GdvClipClass;

typedef struct                  _GdvTrackClass                  GdvTrackClass;

typedef struct                  _GdvPlaySinkClass               GdvPlaySinkClass;

typedef struct                  _GdvBlacknessClass              GdvBlacknessClass;

typedef struct                  _GdvFilterClass                 GdvFilterClass;

typedef struct                  _GdvMetaFilterClass             GdvMetaFilterClass;

typedef struct                  _GdvDataFilterClass             GdvDataFilterClass;

typedef struct                  _GdvVideoDataFilterClass        GdvVideoDataFilterClass;

typedef struct                  _GdvVideoMetaFilterClass        GdvVideoMetaFilterClass;

typedef struct                  _GdvMetaIntroducerClass         GdvMetaIntroducerClass;

typedef struct                  _GdvFilterCanClass              GdvFilterCanClass;

typedef struct                  _GdvBumperStamperClass          GdvBumperStamperClass;

typedef struct                  _GdvDvSourceClass               GdvDvSourceClass;

typedef struct                  _GdvWavSourceClass              GdvWavSourceClass;

typedef struct                  _GdvPipelineClass               GdvPipelineClass;

typedef struct                  _GdvClipStoreClass              GdvClipStoreClass;

typedef struct                  _GdvSingleViewBinClass          GdvSingleViewBinClass;

typedef struct                  _GdvTwinViewBinClass            GdvTwinViewBinClass;

typedef struct                  _GdvExporterClass               GdvExporterClass;

typedef struct                  _GdvVideoBoyClass               GdvVideoBoyClass;

typedef struct                  _GdvAudioBoyClass               GdvAudioBoyClass;

typedef struct                  _GdvIdleTowerClass              GdvIdleTowerClass;

typedef struct                  _GdvClipGateClass               GdvClipGateClass;

typedef struct                  _GdvCorrectorClass              GdvCorrectorClass;

/* Interface classes */

typedef struct                  _GdvTimeableInterface           GdvTimeableInterface;

typedef struct                  _GdvFrameableInterface          GdvFrameableInterface;

typedef struct                  _GdvThumbnailableInterface      GdvThumbnailableInterface;

typedef struct                  _GdvFileBasedInterface          GdvFileBasedInterface;

/* GTypes */

#define                 GDV_TYPE_PIXBUFSINK             (gdv_pixbufsink_get_type ())

#define                 GDV_TYPE_TIMESPAN               (gdv_timespan_get_type ())

#define                 GDV_TYPE_MEDIAITEM              (gdv_mediaitem_get_type ())

#define                 GDV_TYPE_CACHEMANAGER           (gdv_cachemanager_get_type ())

#define                 GDV_TYPE_AVITEM                 (gdv_avitem_get_type ())

#define                 GDV_TYPE_AUDIOITEM              (gdv_audioitem_get_type ())

#define                 GDV_TYPE_PHOTOITEM              (gdv_photoitem_get_type ())

#define                 GDV_TYPE_FRAMEDIMENSIONS        (gdv_framedimensions_get_type ())

#define                 GDV_TYPE_FRACTION               (gdv_fraction_get_type ())

#define                 GDV_TYPE_COLOR                  (gdv_color_get_type ())

#define                 GDV_TYPE_BUFFERSPOOFER          (gdv_bufferspoofer_get_type ())

#define                 GDV_TYPE_SOURCE                 (gdv_source_get_type ())

#define                 GDV_TYPE_DECODEBIN              (gdv_decodebin_get_type ())

#define                 GDV_TYPE_AVISOURCE              (gdv_avisource_get_type ())

#define                 GDV_TYPE_MJPEGSOURCE            (gdv_mjpegsource_get_type ())

#define                 GDV_TYPE_VIDEOCOMPSINK          (gdv_videocompsink_get_type ())

#define                 GDV_TYPE_COMPSINK               (gdv_compsink_get_type ())

#define                 GDV_TYPE_TWINCOMPSINK           (gdv_twincompsink_get_type ())

#define                 GDV_TYPE_AUDIOCOMPSINK          (gdv_audiocompsink_get_type ())

#define                 GDV_TYPE_INSPECTOR              (gdv_inspector_get_type ())

#define                 GDV_TYPE_VIDEOFORMAT            (gdv_videoformat_get_type ())

#define                 GDV_TYPE_AUDIOFORMAT            (gdv_audioformat_get_type ())

#define                 GDV_TYPE_PROJECTFORMAT          (gdv_projectformat_get_type ())

#define                 GDV_TYPE_VIDEOBACKEND           (gdv_videobackend_get_type ())

#define                 GDV_TYPE_TIMEABLE               (gdv_timeable_get_type ())

#define                 GDV_TYPE_FRAMEABLE              (gdv_frameable_get_type ())

#define                 GDV_TYPE_THUMBNAILABLE          (gdv_thumbnailable_get_type ())

#define                 GDV_TYPE_FILE_BASED             (gdv_file_based_get_type ())

#define                 GDV_TYPE_FRAMERUNNER            (gdv_framerunner_get_type ())

#define                 GDV_TYPE_CLIP                   (gdv_clip_get_type ())

#define                 GDV_TYPE_TRACK                  (gdv_track_get_type ())

#define                 GDV_TYPE_PLAYSINK               (gdv_playsink_get_type ())

#define                 GDV_TYPE_FILTER                 (gdv_filter_get_type ())

#define                 GDV_TYPE_METAFILTER             (gdv_metafilter_get_type ())

#define                 GDV_TYPE_DATAFILTER             (gdv_datafilter_get_type ())

#define                 GDV_TYPE_VIDEODATAFILTER        (gdv_videodatafilter_get_type ())

#define                 GDV_TYPE_METAINTRODUCER         (gdv_metaintroducer_get_type ())

#define                 GDV_TYPE_VIDEOMETAFILTER        (gdv_videometafilter_get_type ())

#define                 GDV_TYPE_FILTERCAN              (gdv_filtercan_get_type ())

#define                 GDV_TYPE_BUMPERSTAMPER          (gdv_bumperstamper_get_type ())

#define                 GDV_TYPE_BLACKSHEEP             (gdv_blacksheep_get_type ())

#define                 GDV_TYPE_IMAGESHEEP             (gdv_imagesheep_get_type ())

#define                 GDV_TYPE_SHEEP                  (gdv_sheep_get_type ())

#define                 GDV_TYPE_SILENTSHEEP            (gdv_silentsheep_get_type ())

#define                 GDV_TYPE_DVSOURCE               (gdv_dvsource_get_type ())

#define                 GDV_TYPE_WAVSOURCE              (gdv_wavsource_get_type ())

#define                 GDV_TYPE_SHEEPSOURCE            (gdv_sheepsource_get_type ())

#define                 GDV_TYPE_PIPELINE               (gdv_pipeline_get_type ())

#define                 GDV_TYPE_CLIPSTORE              (gdv_clipstore_get_type ())

#define                 GDV_TYPE_SINGLEVIEWBIN          (gdv_singleviewbin_get_type ())

#define                 GDV_TYPE_TWINVIEWBIN            (gdv_twinviewbin_get_type ())

#define                 GDV_TYPE_EXPORTER               (gdv_exporter_get_type ())

#define                 GDV_TYPE_VIDEOBOY               (gdv_videoboy_get_type ())

#define                 GDV_TYPE_AUDIOBOY               (gdv_audioboy_get_type ())

#define                 GDV_TYPE_IDLETOWER              (gdv_idletower_get_type ())

#define                 GDV_TYPE_CLIPGATE               (gdv_clipgate_get_type ())

#define                 GDV_TYPE_CORRECTOR              (gdv_corrector_get_type ())

/* Is */

#define                 GDV_IS_TIMEABLE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_TIMEABLE))

#define                 GDV_IS_FRAMEABLE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_FRAMEABLE))

#define                 GDV_IS_THUMBNAILABLE(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_THUMBNAILABLE))

#define                 GDV_IS_FRAME_BASED              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_FILE_BASED))

#define                 GDV_IS_MEDIAITEM(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_MEDIAITEM))

#define                 GDV_IS_INSPECTOR(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_INSPECTOR))

#define                 GDV_IS_VIDEOFORMAT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_VIDEOFORMAT))

#define                 GDV_IS_AUDIOFORMAT(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_AUDIOFORMAT))

#define                 GDV_IS_PROJECTFORMAT(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_PROJECTFORMAT))

#define                 GDV_IS_VIDEOBACKEND(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_VIDEOBACKEND))

#define                 GDV_IS_SOURCE(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_SOURCE))

#define                 GDV_IS_DECODEBIN(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_DECODEBIN))

#define                 GDV_IS_AVITEM(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_AVITEM))

#define                 GDV_IS_AUDIOITEM(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_AUDIOITEM))

#define                 GDV_IS_PHOTOITEM(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_PHOTOITEM))

#define                 GDV_IS_PIXBUFSINK(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_PIXBUFSINK))

#define                 GDV_IS_TOKEN(obj)               (gdv_token_is_valid (obj))

#define                 GDV_IS_FRAMERUNNER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_FRAMERUNNER))

#define                 GDV_IS_CLIP(obj)                (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_CLIP))

#define                 GDV_IS_TRACK(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_TRACK))

#define                 GDV_IS_PLAYSINK(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_PLAYSINK))

#define                 GDV_IS_BLACKNESS(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_BLACKNESS))

#define                 GDV_IS_VIDEOBUFFER(obj)         (gdv_videobuffer_is (obj))

#define                 GDV_IS_AUDIOBUFFER(obj)         (gdv_audiobuffer_is (obj))

#define                 GDV_IS_FILTER(obj)              (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_FILTER))

#define                 GDV_IS_METAFILTER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_METAFILTER))

#define                 GDV_IS_DATAFILTER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_DATAFILTER))

#define                 GDV_IS_VIDEODATAFILTER(obj)     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_VIDEODATAFILTER))

#define                 GDV_IS_VIDEOMETAFILTER(obj)     (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_VIDEOMETAFILTER))

#define                 GDV_IS_METAINTRODUCER(obj)      (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_METAINTRODUCER))

#define                 GDV_IS_OPACITY(obj)             (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_OPACITY))

#define                 GDV_IS_FILTERCAN(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_FILTERCAN))

#define                 GDV_IS_BUMPERSTAMPER(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_BUMPERSTAMPER))

#define                 GDV_IS_SHEEP(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_SHEEP))

#define                 GDV_IS_SHEEPSOURCE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_SHEEPSOURCE))

#define                 GDV_IS_BLACKSHEEP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_BLACKSHEEP))

#define                 GDV_IS_SILENTSHEEP(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_SILENTSHEEP))

#define                 GDV_IS_IMAGESHEEP(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_IMAGESHEEP))

#define                 GDV_IS_DVSOURCE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_DVSOURCE))

#define                 GDV_IS_WAVSOURCE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_WAVSOURCE))

#define                 GDV_IS_VIDEOCOMPSINK(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_VIDEOCOMPSINK))

#define                 GDV_IS_TWINCOMPSINK(obj)        (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_TWINCOMPSINK))

#define                 GDV_IS_AUDIOCOMPSINK(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_AUDIOCOMPSINK))

#define                 GDV_IS_COMPSINK(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_COMPSINK))

#define                 GDV_IS_PIPELINE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_PIPELINE))

#define                 GDV_IS_CLIPSTORE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_CLIPSTORE))

#define                 GDV_IS_SINGLEVIEWBIN(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_SINGLEVIEWBIN))

#define                 GDV_IS_TWINVIEWBIN(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_TWINVIEWBIN))

#define                 GDV_IS_EXPORTER(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_EXPORTER))

#define                 GDV_IS_VIDEOBOY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_VIDEOBOY))

#define                 GDV_IS_AUDIOBOY(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_AUDIOBOY))

#define                 GDV_IS_IDLETOWER(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_IDLETOWER))

#define                 GDV_IS_CLIPGATE(obj)            (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_CLIPGATE))

#define                 GDV_IS_CORRECTOR(obj)           (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GDV_TYPE_CORRECTOR))

/* Type casts */

#define                 GDV_SOURCE(obj)                 (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_SOURCE, GdvSource))

#define                 GDV_CLIP(obj)                   (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_CLIP, GdvClip))

#define                 GDV_METAFILTER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_METAFILTER, GdvMetaFilter))

#define                 GDV_VIDEOMETAFILTER(obj)        (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_VIDEOMETAFILTER, GdvVideoMetaFilter))

#define                 GDV_METAINTRODUCER(obj)         (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_METAINTRODUCER, GdvMetaIntroducer))

#define                 GDV_BUMPERSTAMPER(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_BUMPERSTAMPER, GdvBumperStamper))

#define                 GDV_BLACKSHEEP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj)\, \
                                                                                             GDV_TYPE_BLACKSHEEP, \
                                                                                             GdvBlackSheep))

#define                 GDV_IMAGESHEEP(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj)\, \
                                                                                             GDV_TYPE_IMAGESHEEP, \
                                                                                             GdvImageSheep))

#define                 GDV_SILENTSHEEP(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
                                                                                             GDV_TYPE_SILENTSHEEP, \
                                                                                             GdvSilentSheep))

#define                 GDV_DVSOURCE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_DVSOURCE, GdvDvSource))

#define                 GDV_WAVSOURCE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_WAVSOURCE, GdvWavSource))

#define                 GDV_SHEEPSOURCE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_SHEEPSOURCE, GdvSheepSource))

#define                 GDV_VIDEOCOMPSINK(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_VIDEOCOMPSINK, GdvVideoCompSink))

#define                 GDV_COMPSINK(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_COMPSINK, GdvCompSink))

#define                 GDV_TWINCOMPSINK(obj)           (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_TWINCOMPSINK, GdvTwinCompSink))

#define                 GDV_AUDIOCOMPSINK(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_AUDIOCOMPSINK, GdvAudioCompSink))

#define                 GDV_PIPELINE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_PIPELINE, GdvPipeline))

#define                 GDV_TRACK(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_TRACK, GdvTrack))

#define                 GDV_SHEEP(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_SHEEP, GdvSheep))

#define                 GDV_CLIPSTORE(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_CLIPSTORE, GdvClipStore))

#define                 GDV_SINGLEVIEWBIN(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_SINGLEVIEWBIN, GdvSingleViewBin))

#define                 GDV_TWINVIEWBIN(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_TWINVIEWBIN, GdvTwinViewBin))

#define                 GDV_EXPORTER(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_EXPORTER, GdvExporter))

#define                 GDV_VIDEOBOY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_VIDEOBOY, GdvVideoBoy))

#define                 GDV_AUDIOBOY(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_AUDIOBOY, GdvAudioBoy))

#define                 GDV_IDLETOWER(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_IDLETOWER, GdvIdleTower))

#define                 GDV_CLIPGATE(obj)               (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_CLIPGATE, GdvClipGate))

#define                 GDV_CORRECTOR(obj)              (G_TYPE_CHECK_INSTANCE_CAST ((obj), GDV_TYPE_CORRECTOR, GdvCorrector))

/* Interface get */

#define                 GDV_TIMEABLE_GET_INTERFACE(inst)(G_TYPE_INSTANCE_GET_INTERFACE ((inst),\
                                                         GDV_TYPE_TIMEABLE, GdvTimeableInterface))

#define                 GDV_FRAMEABLE_GET_INTERFACE(inst)(G_TYPE_INSTANCE_GET_INTERFACE ((inst),\
                                                          GDV_TYPE_FRAMEABLE, GdvFrameableInterface))

#define                 GDV_FILEBASED_GET_INTERFACE(inst)(G_TYPE_INSTANCE_GET_INTERFACE ((inst),\
                                                          GDV_TYPE_FILE_BASED, GdvFileBasedInterface))

/* Enums */

typedef enum            _GdvError                       GdvError;

typedef enum            _GdvSourceState                 GdvSourceState;

typedef enum            _GdvJobType                     GdvJobType;

typedef enum            _GdvItemType                    GdvItemType;

typedef enum            _GdvSourceType                  GdvSourceType;

typedef enum            _GdvColorConst                  GdvColorConst;

typedef enum            _GdvColorTango                  GdvColorTango;

typedef enum            _GdvEngineCapab                 GdvEngineCapab;

typedef enum            _GdvFlavor                      GdvFlavor;

typedef enum            _GdvFormatReturn                GdvFormatReturn;

/* Functions */

typedef GdvSource*      (*GdvMediaItemSourceFunc)       (GdvMediaItem *this);

typedef GdvSource*      (*GdvSourceCloneFunc)           (GdvSource *this);

typedef gboolean        (*GdvSourceSeekFunc)            (GdvSource *this, GdvTimeSpan *span, gboolean flush);

typedef gboolean        (*GdvJobWorkFunc)               (gpointer data);

typedef void            (*GdvJobFreeFunc)               (GdvJob *this);

typedef GdvFormatReturn (*GdvVideoBackendFormatFunc)    (GdvVideoBackend *this, GdvVideoFormat *vformat);

typedef gboolean        (*GdvVideoBackendInitializeFunc)(GdvVideoBackend *this, GError **error);

typedef gboolean        (*GdvMetaFilterBufferFunc)      (GdvMetaFilter *this, gpointer *buffer);

typedef gboolean        (*GdvDataFilterBufferFunc)      (GdvDataFilter *this, gpointer *buffer);

typedef gboolean        (*GdvVideoComposeFunc)          (GdvVideoCompSink *this, GList *buffers);

typedef gboolean        (*GdvAudioCompSinkNewPad)       (GdvAudioCompSink *this, GstPad *pad);

/* Global enums defined */

enum _GdvFlavor {
        GDV_FLAVOR_NONE,
        GDV_FLAVOR_VIDEO,
        GDV_FLAVOR_AUDIO
};

#endif

