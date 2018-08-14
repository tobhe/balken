#ifndef BALKEN__MSER_H__INCLUDED
#define BALKEN__MSER_H__INCLUDED

#include <cstdint>
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

  // Public member variables
  int                              level;   // < current level of region
  int                              u[5];    // < image moments
  int                              area;    // < area of region (equal to u_0)
  std::vector<std::pair<int, int>> pixels;  // < list of pixels in area
};

std::vector<Region> detect_regions(const blaze::DynamicMatrix<uint8_t> & in);

}  // namespace mser
}  // namespace balken

#endif
