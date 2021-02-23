/* GStreamer
 * Copyright (C) 2020 FIXME <fixme@example.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstvaifacetracking
 *
 * The vaifacetracking element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! vaifacetracking ! FIXME ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>
#include "gstvaifacetracking.h"

/* OpenCV header files */
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* Vitis-AI-Library specific header files */
#include <vitis/ai/facedetect.hpp>
#include "./facedetectwithtracking.hpp"
#include "./centroidtracker.hpp"

GST_DEBUG_CATEGORY_STATIC (gst_vaifacetracking_debug_category);
#define GST_CAT_DEFAULT gst_vaifacetracking_debug_category

/* prototypes */


static void gst_vaifacetracking_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_vaifacetracking_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_vaifacetracking_dispose (GObject * object);
static void gst_vaifacetracking_finalize (GObject * object);

static gboolean gst_vaifacetracking_start (GstBaseTransform * trans);
static gboolean gst_vaifacetracking_stop (GstBaseTransform * trans);
static gboolean gst_vaifacetracking_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info);
static GstFlowReturn gst_vaifacetracking_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * inframe, GstVideoFrame * outframe);
static GstFlowReturn gst_vaifacetracking_transform_frame_ip (GstVideoFilter * filter,
    GstVideoFrame * frame);

enum
{
  PROP_0
};

/* pad templates */

/* Input format */
#define VIDEO_SRC_CAPS \
    GST_VIDEO_CAPS_MAKE("{ BGR }")

/* Output format */
#define VIDEO_SINK_CAPS \
    GST_VIDEO_CAPS_MAKE("{ BGR }")


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstVaifacetracking, gst_vaifacetracking, GST_TYPE_VIDEO_FILTER,
  GST_DEBUG_CATEGORY_INIT (gst_vaifacetracking_debug_category, "vaifacetracking", 0,
  "debug category for vaifacetracking element"));

static void
gst_vaifacetracking_class_init (GstVaifacetrackingClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS (klass);
  GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
   base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(klass),
    gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
      gst_caps_from_string (VIDEO_SRC_CAPS ",width = (int) [1, 640], height = (int) [1, 360]")));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(klass),
    gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
      gst_caps_from_string (VIDEO_SINK_CAPS ", width = (int) [1, 640], height = (int) [1, 360]")));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
    "Face detection using the Vitis-AI-Library, and tracking", 
    "Video Filter", 
    "Face Detection and Tracking",
    "FIXME <fixme@example.com>");


  gobject_class->set_property = gst_vaifacetracking_set_property;
  gobject_class->get_property = gst_vaifacetracking_get_property;
  gobject_class->dispose = gst_vaifacetracking_dispose;
  gobject_class->finalize = gst_vaifacetracking_finalize;
  base_transform_class->start = GST_DEBUG_FUNCPTR (gst_vaifacetracking_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR (gst_vaifacetracking_stop);
  video_filter_class->set_info = GST_DEBUG_FUNCPTR (gst_vaifacetracking_set_info);
  video_filter_class->transform_frame_ip = GST_DEBUG_FUNCPTR (gst_vaifacetracking_transform_frame_ip);

}

static void
gst_vaifacetracking_init (GstVaifacetracking *facetracking)
{
}

void
gst_vaifacetracking_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (object);

  GST_DEBUG_OBJECT (vaifacetracking, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_vaifacetracking_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (object);

  GST_DEBUG_OBJECT (vaifacetracking, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_vaifacetracking_dispose (GObject * object)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (object);

  GST_DEBUG_OBJECT (vaifacetracking, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_vaifacetracking_parent_class)->dispose (object);
}

void
gst_vaifacetracking_finalize (GObject * object)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (object);

  GST_DEBUG_OBJECT (vaifacetracking, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_vaifacetracking_parent_class)->finalize (object);
}

static gboolean
gst_vaifacetracking_start (GstBaseTransform * trans)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (trans);

  GST_DEBUG_OBJECT (vaifacetracking, "start");

  return TRUE;
}

static gboolean
gst_vaifacetracking_stop (GstBaseTransform * trans)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (trans);

  GST_DEBUG_OBJECT (vaifacetracking, "stop");

  return TRUE;
}

static gboolean
gst_vaifacetracking_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (filter);

  GST_DEBUG_OBJECT (vaifacetracking, "set_info");

  return TRUE;
}

/* transform */
static GstFlowReturn
gst_vaifacetracking_transform_frame (GstVideoFilter * filter, GstVideoFrame * inframe,
    GstVideoFrame * outframe)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (filter);

  GST_DEBUG_OBJECT (vaifacetracking, "transform_frame");

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_vaifacetracking_transform_frame_ip (GstVideoFilter * filter, GstVideoFrame * frame)
{
  GstVaifacetracking *vaifacetracking = GST_VAIFACETRACKING (filter);

  /* Create pose detection object */
  thread_local auto face = vitis::ai::FaceDetectWithTracking::create();

  /* Copy frame data into OpenCV Mat */
  cv::Mat img(360, 640, CV_8UC3, GST_VIDEO_FRAME_PLANE_DATA(frame, 0));

  /* Perform pose detection */
  auto results = face->run(img);

  //for (auto &result : results) {
  // process_result(img, result, false);
  //}

  GST_DEBUG_OBJECT (vaifacetracking, "transform_frame_ip");

  return GST_FLOW_OK;
}



static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "vaifacetracking", GST_RANK_NONE,
      GST_TYPE_VAIFACETRACKING);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.0"
#endif
#ifndef PACKAGE
#define PACKAGE "vaifacetracking"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "GStreamer Xilinx Vitis-AI-Library"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://xilinx.com"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    vaifacetracking,
    "face detection using the Xilinx Vitis-AI-Library, and tracking",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN) 

