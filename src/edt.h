/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__EDT_H__INCLUDED
#define BALKEN__EDT_H__INCLUDED

// cpp
#include <algorithm>
#include <cmath>
#include <limits>

// external
#include <blaze/math/DynamicMatrix.h>

// own
#include "util.h"

namespace balken {
namespace edt {
namespace detail {

/**
 * Calculate manhattan distance between two points p1 and p2
 */
template <class PointT>
constexpr uint32_t manhattan_distance(PointT p1, PointT p2) {
  auto y = std::get<0>(p1) > std::get<0>(p2)
             ? std::get<0>(p1) - std::get<0>(p2)
             : std::get<0>(p2) - std::get<0>(p1);

  auto x = std::get<1>(p1) > std::get<1>(p2)
             ? std::get<1>(p1) - std::get<1>(p2)
             : std::get<1>(p2) - std::get<1>(p1);

  return x + y;
}

/**
 * Calculate chessboard distance between two points p1 and p2
 */
template <class PointT>
constexpr uint32_t chessboard_distance(PointT p1, PointT p2) {
  auto y = std::get<0>(p1) > std::get<0>(p2)
             ? std::get<0>(p1) - std::get<0>(p2)
             : std::get<0>(p2) - std::get<0>(p1);

  auto x = std::get<1>(p1) > std::get<1>(p2)
             ? std::get<1>(p1) - std::get<1>(p2)
             : std::get<1>(p2) - std::get<1>(p1);

  return std::max(x, y);
}

template <class ImageT, class PointT>
constexpr bool is_object(const ImageT & img, PointT p) {
  return img(std::get<0>(p), std::get<1>(p)) == 0;
}

template <class ImageT, class PointT>
constexpr bool is_background(const ImageT & img, PointT p) {
  return img(std::get<0>(p), std::get<1>(p)) != 0;
}

/**
 * Fast Independent Scanning Algorithm
 */
template <class ImageT>
auto fast_independent_scan(const ImageT & img) {
  auto distance_map = blaze::DynamicMatrix<uint16_t, blaze::rowMajor>(
    img.rows(), img.columns(), std::numeric_limits<uint16_t>::max());

  // Row Scanning
  for (size_t i = 0UL; i < img.rows(); ++i) {
    // left to right sweep
    for (size_t j = 0UL; j < img.columns(); ++j) {
      if (detail::is_object(img, std::make_pair(i, j))) {
        if (j != 0) {
          if (distance_map(i, j - 1) != std::numeric_limits<uint16_t>::max()) {
            distance_map(i, j) =
              static_cast<uint16_t>(distance_map(i, j - 1) + 1);
          }
        }
      } else {
        distance_map(i, j) = 0;
      }
    }

    // right to left sweep
    for (int j = img.columns() - 1; j >= 0; --j) {
      if (detail::is_object(img, std::make_pair(i, j))) {
        if (j != static_cast<int>(img.columns()) - 1) {
          if (distance_map(i, j + 1) != std::numeric_limits<uint16_t>::max()) {
            distance_map(i, j) =
              std::min(static_cast<uint16_t>(distance_map(i, j)),
                       static_cast<uint16_t>(distance_map(i, j + 1) + 1));
          }
        }
      }
    }
  }

  // Column Scanning
  for (size_t j = 0UL; j < img.columns(); ++j) {
    // top -> bottom
    for (size_t i = 0UL; i < img.rows(); ++i) {
      if (detail::is_object(img, std::make_pair(i, j))) {
        if (i != 0) {
          if (distance_map(i - 1, j) != std::numeric_limits<uint16_t>::max()) {
            distance_map(i, j) =
              std::min(static_cast<uint16_t>(distance_map(i, j)),
                       static_cast<uint16_t>(distance_map(i - 1, j) + 1));
          }
        }
      }
    }

    // bottom -> top
    for (int i = img.rows() - 1; i >= 0; --i) {
      if (detail::is_object(img, std::make_pair(i, j))) {
        if (i != static_cast<int>(img.rows() - 1)) {
          if (distance_map(i + 1, j) != std::numeric_limits<uint16_t>::max()) {
            distance_map(i, j) =
              std::min(static_cast<uint16_t>(distance_map(i, j)),
                       static_cast<uint16_t>(distance_map(i + 1, j) + 1));
          }
        }
      }
    }
  }
  return distance_map;
}

}  // namespace detail

template <class ImageT>
decltype(auto) transform(ImageT && img) {
  return detail::fast_independent_scan(std::forward<ImageT>(img));
}

}  // namespace edt
}  // namespace balken

#endif
