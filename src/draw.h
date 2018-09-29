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

namespace balken {
namespace draw {

template <class ImageT, class PointT>
void draw_line(ImageT & img, PointT p0, PointT p1, const int val) {
  if (p0.first < 0 || p0.second < 0 ||
      p0.first > (static_cast<int>(img.rows()) - 1) ||
      p0.second > (static_cast<int>(img.columns()) - 1)) {
    return;
  }

  if (p1.first < 0 || p1.second < 0 ||
      p1.first > (static_cast<int>(img.rows()) - 1) ||
      p1.second > (static_cast<int>(img.columns()) - 1)) {
    return;
  }

  // Shortcut for single pixel
  if (p0.first == p1.first && p0.second == p1.second) {
    img(p0.first, p0.second) = val;
    return;
  }

  const bool steep = (abs(p1.first - p0.first) > abs(p1.second - p0.second));
  if (steep) {
    p0 = std::make_pair(p0.second, p0.first);
    p1 = std::make_pair(p1.second, p1.first);
  }

  if (p0.second > p1.second) { std::swap(p0, p1); }

  int dx = p1.second - p0.second;
  int dy = abs(p1.first - p0.first);

  float     error = static_cast<float>(dx) / 2.0;
  const int step  = (p1.first < p0.first) ? 1 : -1;
  int       y     = p0.first;

  for (int x = p0.second; x <= p1.second; ++x) {
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

template <class ImageT, class PointT>
void draw_box(ImageT & img, const PointT tl, const PointT br, const int val) {
  auto tr = std::pair<int, int>(tl.first, br.second);
  auto bl = std::pair<int, int>(br.first, tl.second);

  std::cout << "i: " << tl.first << " j: " << tl.second << '\n';
  std::cout << "i: " << tr.first << " j: " << tr.second << '\n';
  std::cout << "i: " << bl.first << " j: " << bl.second << '\n';
  std::cout << "i: " << br.first << " j: " << br.second << '\n';

  draw_line(img, tl, tr, val);
  draw_line(img, tr, br, val);
  draw_line(img, bl, br, val);
  draw_line(img, tl, bl, val);
}

template <class ImageT>
void draw_bounding_box(ImageT &                                 img,
                       const std::vector<std::pair<int, int>> & points,
                       const int                                val) {
  auto top_left  = std::pair<int, int>(img.rows() - 1, img.columns() - 1);
  auto bot_right = std::pair<int, int>(0, 0);

  for (auto i : points) {
    if (i.second < top_left.second) { top_left.second = i.second; }
    if (i.second > bot_right.second) { bot_right.second = i.second; }
    if (i.first < top_left.first) { top_left.first = i.first; }
    if (i.first > bot_right.first) { bot_right.first = i.first; }
  }

  draw_box(img, top_left, bot_right, val);
}

}  // namespace draw
}  // namespace balken

#endif
