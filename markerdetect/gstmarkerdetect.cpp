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
 * SECTION:element-gstmarkerdetect
 *
 * The markerdetect element does FIXME stuff.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch-1.0 -v fakesrc ! markerdetect ! FIXME ! fakesink
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
#include "gstmarkerdetect.h"

/* OpenCV header files */
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/* Aruco Markers */
#include <opencv2/aruco.hpp>

GST_DEBUG_CATEGORY_STATIC (gst_markerdetect_debug_category);
#define GST_CAT_DEFAULT gst_markerdetect_debug_category

/* prototypes */


static void gst_markerdetect_set_property (GObject * object,
    guint property_id, const GValue * value, GParamSpec * pspec);
static void gst_markerdetect_get_property (GObject * object,
    guint property_id, GValue * value, GParamSpec * pspec);
static void gst_markerdetect_dispose (GObject * object);
static void gst_markerdetect_finalize (GObject * object);

static gboolean gst_markerdetect_start (GstBaseTransform * trans);
static gboolean gst_markerdetect_stop (GstBaseTransform * trans);
static gboolean gst_markerdetect_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info);
static GstFlowReturn gst_markerdetect_transform_frame (GstVideoFilter * filter,
    GstVideoFrame * inframe, GstVideoFrame * outframe);
static GstFlowReturn gst_markerdetect_transform_frame_ip (GstVideoFilter * filter,
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

G_DEFINE_TYPE_WITH_CODE (GstMarkerDetect, gst_markerdetect, GST_TYPE_VIDEO_FILTER,
  GST_DEBUG_CATEGORY_INIT (gst_markerdetect_debug_category, "markerdetect", 0,
  "debug category for markerdetect element"));

static void
gst_markerdetect_class_init (GstMarkerDetectClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseTransformClass *base_transform_class = GST_BASE_TRANSFORM_CLASS (klass);
  GstVideoFilterClass *video_filter_class = GST_VIDEO_FILTER_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(klass),
    gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
      gst_caps_from_string (VIDEO_SRC_CAPS ",width = (int) [1, 1920], height = (int) [1, 1080]")));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS(klass),
    gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
      gst_caps_from_string (VIDEO_SINK_CAPS ", width = (int) [1, 1920], height = (int) [1, 1080]")));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS(klass),
    "Marker detection using the OpenCV Library", 
    "Video Filter", 
    "Marker Detection",
    "FIXME <fixme@example.com>");

  gobject_class->set_property = gst_markerdetect_set_property;
  gobject_class->get_property = gst_markerdetect_get_property;
  gobject_class->dispose = gst_markerdetect_dispose;
  gobject_class->finalize = gst_markerdetect_finalize;
  base_transform_class->start = GST_DEBUG_FUNCPTR (gst_markerdetect_start);
  base_transform_class->stop = GST_DEBUG_FUNCPTR (gst_markerdetect_stop);
  video_filter_class->set_info = GST_DEBUG_FUNCPTR (gst_markerdetect_set_info);
  video_filter_class->transform_frame_ip = GST_DEBUG_FUNCPTR (gst_markerdetect_transform_frame_ip);

}

static void
gst_markerdetect_init (GstMarkerDetect *markerdetect)
{
}

void
gst_markerdetect_set_property (GObject * object, guint property_id,
    const GValue * value, GParamSpec * pspec)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (object);

  GST_DEBUG_OBJECT (markerdetect, "set_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_markerdetect_get_property (GObject * object, guint property_id,
    GValue * value, GParamSpec * pspec)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (object);

  GST_DEBUG_OBJECT (markerdetect, "get_property");

  switch (property_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
      break;
  }
}

void
gst_markerdetect_dispose (GObject * object)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (object);

  GST_DEBUG_OBJECT (markerdetect, "dispose");

  /* clean up as possible.  may be called multiple times */

  G_OBJECT_CLASS (gst_markerdetect_parent_class)->dispose (object);
}

void
gst_markerdetect_finalize (GObject * object)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (object);

  GST_DEBUG_OBJECT (markerdetect, "finalize");

  /* clean up object here */

  G_OBJECT_CLASS (gst_markerdetect_parent_class)->finalize (object);
}

static gboolean
gst_markerdetect_start (GstBaseTransform * trans)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (trans);

  GST_DEBUG_OBJECT (markerdetect, "start");

  return TRUE;
}

static gboolean
gst_markerdetect_stop (GstBaseTransform * trans)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (trans);

  GST_DEBUG_OBJECT (markerdetect, "stop");

  return TRUE;
}

static gboolean
gst_markerdetect_set_info (GstVideoFilter * filter, GstCaps * incaps,
    GstVideoInfo * in_info, GstCaps * outcaps, GstVideoInfo * out_info)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (filter);

  GST_DEBUG_OBJECT (markerdetect, "set_info");

  return TRUE;
}

/* transform */
static GstFlowReturn
gst_markerdetect_transform_frame (GstVideoFilter * filter, GstVideoFrame * inframe,
    GstVideoFrame * outframe)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (filter);

  GST_DEBUG_OBJECT (markerdetect, "transform_frame");

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_markerdetect_transform_frame_ip (GstVideoFilter * filter, GstVideoFrame * frame)
{
  GstMarkerDetect *markerdetect = GST_MARKERDETECT (filter);

  /* Setup an OpenCV Mat with the frame data */
  int width = GST_VIDEO_FRAME_WIDTH(frame);
  int height = GST_VIDEO_FRAME_HEIGHT(frame);
  cv::Mat img(height, width, CV_8UC3, GST_VIDEO_FRAME_PLANE_DATA(frame, 0));

  //
  // Detect ARUCO markers
  //   ref : https://docs.opencv.org/master/d5/dae/tutorial_aruco_detection.html
  //
  
  std::vector<int> markerIds;
  std::vector<std::vector<cv::Point2f>> markerCorners, rejectedCandidates;
  cv::Ptr<cv::aruco::DetectorParameters> parameters = cv::aruco::DetectorParameters::create();
  cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_ARUCO_ORIGINAL);
  cv::aruco::detectMarkers(img, dictionary, markerCorners, markerIds, parameters, rejectedCandidates);

  if ( markerIds.size() > 0 )
  {
    cv::aruco::drawDetectedMarkers(img, markerCorners, markerIds);
  }
  
  if (markerIds.size() >= 4 )
  {
    int tl_id = 0;
    int tr_id = 0;
    int bl_id = 0;
    int br_id = 0;
    cv::Point2f tl_xy, tr_xy, bl_xy, br_xy;
    for ( unsigned i = 0; i < markerIds.size(); i++ )
    {
      switch ( markerIds[i] )
      {
      case 923:
        tl_id = markerIds[i];
        //tl_xy = markerCorners[i][2]; // bottom right corner of top left marker
        tl_xy = markerCorners[i][3]; // bottom left corner of top left marker
        break;
      case 1001:
      case 1002:
      case 1003:
      case 1004:
      case 1005:
      case 1006:
        tr_id = markerIds[i];
        //tr_xy = markerCorners[i][3]; // bottom left corner of top right marker
        tr_xy = markerCorners[i][2]; // bottom right corner of top right marker
        break;
      case 1007:
        bl_id = markerIds[i];
        //bl_xy = markerCorners[i][1]; // top right corner of bottom left marker
        bl_xy = markerCorners[i][0]; // top left corner of bottom left marker
        break;
      case 241:
        br_id = markerIds[i];
        //br_xy = markerCorners[i][0]; // top left corner of bottom right marker
        br_xy = markerCorners[i][1]; // top right corner of bottom right marker
        break;
      default:
        break;
      }
    }
    // Chart 1 - Checkerboard (9x7)
    if ( (tl_id==923) && (tr_id==1001) && (bl_id==1007) && (br_id==241) )
    {
      // Extract ROI (area, ideally within 4 markers)
      std::vector<cv::Point> polygonPoints;
      polygonPoints.push_back(cv::Point(tl_xy.x,tl_xy.y));
      polygonPoints.push_back(cv::Point(tr_xy.x,tr_xy.y));
      polygonPoints.push_back(cv::Point(br_xy.x,br_xy.y));
      polygonPoints.push_back(cv::Point(bl_xy.x,bl_xy.y));
      // Draw border around "checkerboard"
      cv::polylines(img, polygonPoints, true, cv::Scalar (0, 255, 0), 2, 16);
    }
    // Chart 2 - White Reference
    if ( (tl_id==923) && (tr_id==1002) && (bl_id==1007) && (br_id==241) )
    {
      // Extract ROI (area, ideally within 4 markers)
      std::vector<cv::Point> polygonPoints;
      polygonPoints.push_back(cv::Point(tl_xy.x,tl_xy.y));
      polygonPoints.push_back(cv::Point(tr_xy.x,tr_xy.y));
      polygonPoints.push_back(cv::Point(br_xy.x,br_xy.y));
      polygonPoints.push_back(cv::Point(bl_xy.x,bl_xy.y));
      cv::Rect roi = cv::boundingRect( polygonPoints );
      cv::Mat roiImage = img(roi);
      std::vector<cv::Mat> bgr_planes;
      cv::split( roiImage, bgr_planes );

      //
      // Calculate color gains
      //   ref : https://stackoverflow.com/questions/32466616/finding-the-average-color-within-a-polygon-bound-in-opencv
      //
      cv::Point pts[1][4];
      pts[0][0] = cv::Point(tl_xy.x,tl_xy.y);
      pts[0][1] = cv::Point(tr_xy.x,tr_xy.y);
      pts[0][2] = cv::Point(br_xy.x,br_xy.y);
      pts[0][3] = cv::Point(bl_xy.x,bl_xy.y);
      const cv::Point* points[1] = {pts[0]};
      int npoints = 4;
      // Create the mask with the polygon
      cv::Mat1b mask(img.rows, img.cols, uchar(0));
      cv::fillPoly(mask, points, &npoints, 1, cv::Scalar(255));
      // Calculate mean in masked area
      auto bgr_mean = cv::mean( img, mask );
      double b_mean = bgr_mean(0);
      double g_mean = bgr_mean(1);
      double r_mean = bgr_mean(2);
      // Find the gain of a channel
      //double K = (b_mean+g_mean+r_mean)/3;
      //double Kb = K/b_mean;
      //double Kg = K/g_mean;
      //double Kr = K/r_mean;
      //printf( "Stats : B=%5.3f G=%5.3f R=%5.3f > Kb=%5.3f Kg=%5.3f Kr=%5.3f\n", b_mean, g_mean, r_mean, Kb, Kg, Kr );
      
      // Draw bars 
      int plot_w = 100, plot_h = 100;
      cv::Mat plotImage( plot_h, plot_w, CV_8UC3, cv::Scalar(255,255,255) );
      int b_bar = int((b_mean/256.0)*80.0);
      int g_bar = int((g_mean/256.0)*80.0);
      int r_bar = int((r_mean/256.0)*80.0);
      // layout of bars : |<-10->|<---20-->|<-10->|<---20-->|<-10->|<---20-->|<-10->|
      cv::rectangle(plotImage, cv::Rect(10,(80-b_bar),20,b_bar), cv::Scalar(255, 0, 0), cv::FILLED, cv::LINE_8);
      cv::rectangle(plotImage, cv::Rect(40,(80-g_bar),20,g_bar), cv::Scalar(0, 255, 0), cv::FILLED, cv::LINE_8);
      cv::rectangle(plotImage, cv::Rect(70,(80-r_bar),20,r_bar), cv::Scalar(0, 0, 255), cv::FILLED, cv::LINE_8);
      //printf( "Stats : BGR=%5.3f,%5.3f,%5.3f (%d,%d,%d) => Kbgr=%5.3f,%5.3f,%5.3f\n", b_mean, g_mean, r_mean, b_bar, g_bar, r_bar, Kb, Kg, Kr );
      std::stringstream b_str;
      std::stringstream g_str;
      std::stringstream r_str;
      b_str << int(b_mean);
      g_str << int(g_mean);
      r_str << int(r_mean);
      cv::putText(plotImage, b_str.str(), cv::Point(10,90), cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(255,0,0), 1, cv::LINE_AA);
      cv::putText(plotImage, g_str.str(), cv::Point(40,90), cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(0,255,0), 1, cv::LINE_AA);
      cv::putText(plotImage, r_str.str(), cv::Point(70,90), cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(0,0,255), 1, cv::LINE_AA);

      // Calculate transformation matrix
      std::vector<cv::Point2f> srcPoints;
      std::vector<cv::Point2f> dstPoints;
      srcPoints.push_back(cv::Point(       0,       0)); // top left
      srcPoints.push_back(cv::Point(plot_w-1,       0)); // top right
      srcPoints.push_back(cv::Point(plot_w-1,plot_h-1)); // bottom right
      srcPoints.push_back(cv::Point(       0,plot_h-1)); // bottom left
      dstPoints.push_back(tl_xy);
      dstPoints.push_back(tr_xy);
      dstPoints.push_back(br_xy);
      dstPoints.push_back(bl_xy);
      cv::Mat h = cv::findHomography(srcPoints,dstPoints);
      // Warp plot image onto video frame
      cv::Mat img_temp = img.clone();
      cv::warpPerspective(plotImage, img_temp, h, img_temp.size());
      cv::Point pts_dst[4];
      for( int i = 0; i < 4; i++)
      {
        pts_dst[i] = dstPoints[i];
      }
      cv::fillConvexPoly(img, pts_dst, 4, cv::Scalar(0), cv::LINE_AA);
      img = img + img_temp;
    }

    // Chart 3 - Histogram
    if ( (tl_id==923) && (tr_id==1003) && (bl_id==1007) && (br_id==241) )
    {
      // Extract ROI (area, ideally within 4 markers)
      std::vector<cv::Point> polygonPoints;
      polygonPoints.push_back(cv::Point(tl_xy.x,tl_xy.y));
      polygonPoints.push_back(cv::Point(tr_xy.x,tr_xy.y));
      polygonPoints.push_back(cv::Point(br_xy.x,br_xy.y));
      polygonPoints.push_back(cv::Point(bl_xy.x,bl_xy.y));
      cv::Rect roi = cv::boundingRect( polygonPoints );
      cv::Mat roiImage = img(roi);
      std::vector<cv::Mat> bgr_planes;
      cv::split( roiImage, bgr_planes );

      //
      // Calculate color histograms
      //    https://github.com/opencv/opencv/blob/3.4/samples/cpp/tutorial_code/Histograms_Matching/calcHist_Demo.cpp
      //
      int hist_w = 512, hist_h = 400;
      cv::Mat histImage( hist_h, hist_w, CV_8UC3, cv::Scalar( 0,0,0) );
      int histSize = 256; // number of bins
      int bin_w = cvRound( (double) hist_w/histSize );
      float range[] = { 0, 256 }; // ranges for B,G,R (the upper boundary is exclusive)
      const float* histRange = { range };
      bool uniform = true, accumulate = false;
      cv::Mat b_hist, g_hist, r_hist;
      cv::calcHist( &bgr_planes[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate );
      cv::calcHist( &bgr_planes[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate );
      cv::calcHist( &bgr_planes[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate );
      // Draw the histograms for B, G and R
      // Normalize the result to ( 0, histImage.rows )
      cv::normalize(b_hist, b_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
      cv::normalize(g_hist, g_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
      cv::normalize(r_hist, r_hist, 0, histImage.rows, cv::NORM_MINMAX, -1, cv::Mat() );
      // Draw for each channel
      for( int i = 1; i < histSize; i++ )
      {
          cv::line( histImage, 
                cv::Point( bin_w*(i-1), hist_h - cvRound(b_hist.at<float>(i-1)) ),
                cv::Point( bin_w*(i), hist_h - cvRound(b_hist.at<float>(i)) ),
                cv::Scalar( 255, 0, 0), 2, 8, 0  );
          cv::line( histImage, 
                cv::Point( bin_w*(i-1), hist_h - cvRound(g_hist.at<float>(i-1)) ),
                cv::Point( bin_w*(i), hist_h - cvRound(g_hist.at<float>(i)) ),
                cv::Scalar( 0, 255, 0), 2, 8, 0  );
          cv::line( histImage,
                cv::Point( bin_w*(i-1), hist_h - cvRound(r_hist.at<float>(i-1)) ),
                cv::Point( bin_w*(i), hist_h - cvRound(r_hist.at<float>(i)) ),
                cv::Scalar( 0, 0, 255), 2, 8, 0  );
      }

      // Draw border around ROI used for color histogram
      cv::rectangle(img, roi, cv::Scalar (0, 255, 0), 2, cv::LINE_AA);

      // Calculate transformation matrix
      std::vector<cv::Point2f> srcPoints;
      std::vector<cv::Point2f> dstPoints;
      srcPoints.push_back(cv::Point(       0,       0)); // top left
      srcPoints.push_back(cv::Point(hist_w-1,       0)); // top right
      srcPoints.push_back(cv::Point(hist_w-1,hist_h-1)); // bottom right
      srcPoints.push_back(cv::Point(       0,hist_h-1)); // bottom left
      dstPoints.push_back(tl_xy);
      dstPoints.push_back(tr_xy);
      dstPoints.push_back(br_xy);
      dstPoints.push_back(bl_xy);
      cv::Mat h = cv::findHomography(srcPoints,dstPoints);
      // Warp histogram image onto video frame
      cv::Mat img_temp = img.clone();
      cv::warpPerspective(histImage, img_temp, h, img_temp.size());
      cv::Point pts_dst[4];
      for( int i = 0; i < 4; i++)
      {
        pts_dst[i] = dstPoints[i];
      }
      cv::fillConvexPoly(img, pts_dst, 4, cv::Scalar(0), cv::LINE_AA);
      img = img + img_temp;
    }
  }

  GST_DEBUG_OBJECT (markerdetect, "transform_frame_ip");

  return GST_FLOW_OK;
}

static gboolean
plugin_init (GstPlugin * plugin)
{

  /* FIXME Remember to set the rank if it's an element that is meant
     to be autoplugged by decodebin. */
  return gst_element_register (plugin, "markerdetect", GST_RANK_NONE,
      GST_TYPE_MARKERDETECT);
}

/* FIXME: these are normally defined by the GStreamer build system.
   If you are creating an element to be included in gst-plugins-*,
   remove these, as they're always defined.  Otherwise, edit as
   appropriate for your external plugin package. */
#ifndef VERSION
#define VERSION "0.0.0"
#endif
#ifndef PACKAGE
#define PACKAGE "markerdetect"
#endif
#ifndef PACKAGE_NAME
#define PACKAGE_NAME "OpenCV Library"
#endif
#ifndef GST_PACKAGE_ORIGIN
#define GST_PACKAGE_ORIGIN "http://avnet.com"
#endif

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    markerdetect,
    "Marker detection using the OpenCV Library",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, GST_PACKAGE_ORIGIN)  


