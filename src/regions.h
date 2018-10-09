/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__REGIONS_H__INCLUDED
#define BALKEN__REGIONS_H__INCLUDED

// thirdparty
#include <blaze/math/DynamicMatrix.h>

// cpp
#include <cmath>
#include <cstdint>
#include <limits>
#include <stack>
#include <vector>
#include "types.h"

namespace balken {
namespace regions {

namespace detail {

int cross(const Point & O, const Point & A, const Point & B) {
  return (A.j - O.j) * (B.i - O.i) - (A.i - O.i) * (B.j - O.j);
}

template <class BinaryImageT>
std::vector<Point> walk_region(const BinaryImageT &         img,
                               Point                        point,
                               blaze::DynamicMatrix<bool> & visited) {
  auto stack  = std::stack<Point>();
  auto region = std::vector<Point>();

  stack.push(point);
  while (!stack.empty()) {
    // Pop stack head
    auto cur = stack.top();
    stack.pop();

    if (visited(cur.i, cur.j)) { continue; }

    // Mark point visioed
    visited(cur.i, cur.j) = true;
    region.push_back(cur);

    if (!(cur.i < 0) && img(cur.i - 1, cur.j)) {
      stack.emplace(cur.i - 1, cur.j);
    }
    if (!(cur.j < 0) && img(cur.i, cur.j - 1)) {
      stack.emplace(cur.i, cur.j - 1);
    }
    if (!(cur.i == static_cast<int>(img.rows())) && img(cur.i + 1, cur.j)) {
      stack.emplace(cur.i + 1, cur.j);
    }
    if (!(cur.j == static_cast<int>(img.columns())) && img(cur.i, cur.j + 1)) {
      stack.emplace(cur.i, cur.j + 1);
    }
  }

  return region;
}
}  // namespace detail

/**
 * Calculate the regions convex hull
 */
template <class RegionT>
std::vector<Point> convex_hull(RegionT & region) {
  size_t n = region.size();
  size_t k = 0;

  if (n <= 3) { return region; }
  auto hull = std::vector<Point>(2 * n);

  std::sort(region.begin(), region.end());

  // Build lower hull
  for (size_t i = 0; i < n; ++i) {
    while (k >= 2 && detail::cross(hull[k - 2], hull[k - 1], region[i]) <= 0)
      k--;
    hull[k++] = region[i];
  }

  // Build upper hull
  for (size_t i = n - 1, t = k + 1; i > 0; --i) {
    while (k >= t &&
           detail::cross(hull[k - 2], hull[k - 1], region[i - 1]) <= 0)
      k--;
    hull[k++] = region[i - 1];
  }

  hull.resize(k - 1);
  return hull;
}


/**
 * One Component at a time algorithm
 */
template <class BinaryImageT>
std::vector<std::vector<Point>> find(const BinaryImageT & img) {
  auto visited = blaze::DynamicMatrix<bool>(img.rows(), img.columns(), false);
  auto stack   = std::stack<Point>();
  auto regions = std::vector<std::vector<Point>>();

  for (int i = 0; i < static_cast<int>(img.rows()); ++i) {
    for (int j = 0; j < static_cast<int>(img.columns()); ++j) {
      if (img(i, j) == std::numeric_limits<uint8_t>::max() &&
          visited(i, j) == false) {
        regions.push_back(detail::walk_region(img, Point(i, j), visited));
      }
    }
  }
  return regions;
}

/**
 * Convert list of regions to image
 */
template <class ImageT>
blaze::DynamicMatrix<uint8_t> from_regions(
  const ImageT & img, const std::vector<std::vector<Point>> & regions) {
  auto reg   = blaze::DynamicMatrix<uint8_t>(img.rows(), img.columns(), 0);
  auto count = size_t{1};
  for (auto & region : regions) {
    for (auto & point : region) { reg(point.i, point.j) = count; }
    ++count;
  }
  return reg;
}

template <class RegionT>
std::pair<size_t, size_t> dimensions(const RegionT & region) {
  auto max_i = size_t{0};
  auto min_i = size_t{std::numeric_limits<size_t>::max()};

  auto max_j = size_t{0};
  auto min_j = size_t{std::numeric_limits<size_t>::max()};

  for (auto & element : region) {
    min_i = std::min(min_i, static_cast<size_t>(element.i));
    max_i = std::max(max_i, static_cast<size_t>(element.i));
    min_j = std::min(min_j, static_cast<size_t>(element.j));
    max_j = std::max(max_j, static_cast<size_t>(element.j));
  }

  return std::make_pair(max_i - min_i, max_j - min_j);
}

/**
 * Filter regions by size and dimensions
 */
template <class RegionsT>
decltype(auto) filter(RegionsT && regions) {
  std::remove_if(regions.begin(), regions.end(), [](const auto & el) {
    if (el.size() < 500 || el.size() > 1000) { return true; }
    auto dims = regions::dimensions(el);
    if (std::abs(static_cast<int>(dims.first) -
                 static_cast<int>(dims.second)) >
        std::min(dims.first, dims.second)) {
      return true;
    }
    return false;
  });
  return regions;
}

}  // namespace regions
}  // namespace balken

#endif
