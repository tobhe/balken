#ifndef BALKEN__MSER_H__INCLUDED
#define BALKEN__MSER_H__INCLUDED

#include <cassert>
#include <cstdint>
#include <functional>
#include <iostream>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

namespace balken {
namespace mser {

struct Region
{
  // Constructor
  Region(std::pair<int, int> p, int level) : level{level}, pixels{p} {}

  void merge(Region & other) {
    area += other.area;
    pixels.insert(pixels.end(), other.pixels.begin(), other.pixels.end());
  }

  void accumulate(std::pair<int, int> p) {
    pixels.push_back(p);
    ++area;
  }

  // Members
  int                              level;
  int                              area{0};
  std::vector<std::pair<int, int>> pixels;
};

std::vector<Region> detect_regions(const blaze::DynamicMatrix<uint8_t> & in);

}  // namespace mser
}  // namespace balken

#endif
