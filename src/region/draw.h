/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__DRAW_H__INCLUDED
#define BALKEN__DRAW_H__INCLUDED

#include <util.h>
#include <cmath>
#include <vector>
#include "types.h"

namespace balken {
namespace draw {

template <class ImageT, class PointT>
void draw_line(ImageT & img, PointT p0, PointT p1, const int val) {
  if (p0.i < 0 || p0.j < 0 || p0.i > (static_cast<int>(img.rows()) - 1) ||
      p0.j > (static_cast<int>(img.columns()) - 1)) {
    return;
  }

  if (p1.i < 0 || p1.j < 0 || p1.i > (static_cast<int>(img.rows()) - 1) ||
      p1.j > (static_cast<int>(img.columns()) - 1)) {
    return;
  }

  // Shortcut for single pixel
  if (p0.i == p1.i && p0.j == p1.j) {
    img(p0.i, p0.j) = val;
    return;
  }

  const bool steep = (abs(p1.i - p0.i) > abs(p1.j - p0.j));
  if (steep) {
    p0 = Point(p0.j, p0.i);
    p1 = Point(p1.j, p1.i);
  }
  if (p0.j > p1.j) { std::swap(p0, p1); }

  int dx = p1.j - p0.j;
  int dy = abs(p1.i - p0.i);

  float     error = static_cast<float>(dx) / 2.0f;
  const int step  = (p0.i < p1.i) ? 1 : -1;
  int       y     = p0.i;

  for (int x = p0.j; x <= p1.j; ++x) {
    if (steep) {
      img(x, y) = val;
    } else {
      img(y, x) = val;
    }
    error -= dy;
    if (error < 0) {
      y += step;
      error += dx;
    }
  }
}

template <class ImageT, class ShapeT>
void draw_shape(ImageT & img, ShapeT shape, uint8_t val) {
  for (auto i = size_t{1}; i < shape.size(); ++i) {
    draw_line(img, shape[i - 1], shape[i], val);
  }
  draw_line(img, shape.back(), shape.front(), val);
}

template <class ImageT>
void bounding_box(ImageT &                   img,
                  const std::vector<Point> & points,
                  const int                  val) {
  auto top_left  = Point(img.rows() - 1, img.columns() - 1);
  auto bot_right = Point(0, 0);

  for (auto i : points) {
    if (i.j < top_left.j) { top_left.j = i.j; }
    if (i.j > bot_right.j) { bot_right.j = i.j; }
    if (i.i < top_left.i) { top_left.i = i.i; }
    if (i.i > bot_right.i) { bot_right.i = i.i; }
  }

  draw_box(img, top_left, bot_right, val);
}

}  // namespace draw
}  // namespace balken

#endif
