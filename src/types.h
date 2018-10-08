/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__TYPES_H__INCLUDED
#define BALKEN__TYPES_H__INCLUDED

namespace balken {

struct Rectangle
{
  std::array<float, 2> U;
  std::array<Point, 4> points;
  size_t               area;
};

struct Point
{
  Point() = default;
  Point(int i, int j) : i{i}, j{j} {}

  int i{0};
  int j{0};
};

bool operator<(const Point & p1, const Point & p2) {
  return p1.i < p2.i || (p1.i == p2.i && p1.j < p2.j);
}


}  // namespace balken

#endif
