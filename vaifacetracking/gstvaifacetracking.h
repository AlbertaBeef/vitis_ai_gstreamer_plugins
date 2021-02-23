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
 * Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _GST_VAIFACETRACKING_H_
#define _GST_VAIFACETRACKING_H_

#include <gst/video/video.h>
#include <gst/video/gstvideofilter.h>

G_BEGIN_DECLS

#define GST_TYPE_VAIFACETRACKING   (gst_vaifacetracking_get_type())
#define GST_VAIFACETRACKING(obj)   (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VAIFACETRACKING,GstVaifacetracking))
#define GST_VAIFACETRACKING_CLASS(klass)   (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VAIFACETRACKING,GstVaifacetrackingClass))
#define GST_IS_VAIFACETRACKING(obj)   (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VAIFACETRACKING))
#define GST_IS_VAIFACETRACKING_CLASS(obj)   (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VAIFACETRACKING))

typedef struct _GstVaifacetracking GstVaifacetracking;
typedef struct _GstVaifacetrackingClass GstVaifacetrackingClass;

struct _GstVaifacetracking
{
  GstVideoFilter base_vaifacetracking;

};

struct _GstVaifacetrackingClass
{
  GstVideoFilterClass base_vaifacetracking_class;
};

GType gst_vaifacetracking_get_type (void);

G_END_DECLS

#endif
