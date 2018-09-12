#ifndef BALKEN__HISTOGRAM_H__INCLUDED
#define BALKEN__HISTOGRAM_H__INCLUDED

#include <util.h>
#include <cmath>
#include <unordered_map>

namespace balken {
namespace histogram {
namespace detail {

template <class ImageT>
auto generate(const ImageT & mat) {
  auto hist = std::array<float, 256>();

  for (size_t i = 0UL; i < mat.rows(); ++i) {
    for (size_t j = 0UL; j < mat.columns(); ++j) { ++hist[mat(i, j)]; }
  }

  for (size_t i = 0UL; i < hist.size(); ++i) {
    hist[i] = hist[i] / (mat.columns() * mat.rows());
  }

  return hist;
}

template <class HistT>
auto accumulate(HistT & hist) {
  for (size_t i = 1UL; i < hist.size(); ++i) { hist[i] += hist[i - 1]; }
  return hist;
}

}  // namespace detail

template <class ImageT>
auto equalize(ImageT & img) {
  auto hist     = detail::generate(img);
  auto cum_hist = detail::accumulate(hist);

  for (size_t i = 0UL; i < img.rows(); ++i) {
    for (size_t j = 0UL; j < img.columns(); ++j) {
      img(i, j) = floor(255 * cum_hist[img(i, j)]);
    }
  }
  return img;
}

template <class ImageT>
auto stretch(ImageT & img) {
  auto hist    = detail::generate(img);
  int  lowest  = 255;
  int  highest = 0;

  for (int i = 0; i < static_cast<int>(hist.size()); ++i) {
    if (hist[i] && (lowest == 255)) {
      lowest  = i;
      highest = i;
    } else if (hist[i]) {
      highest = i;
    }
  }

  float factor = 255.0 / (highest - lowest);

  std::cout << lowest << ", " << highest << '\n';

  for (size_t i = 0UL; i < img.rows(); ++i) {
    for (size_t j = 0UL; j < img.columns(); ++j) {
      img(i, j) = static_cast<uint8_t>((img(i, j) - lowest) * factor);
    }
  }
  return img;
}

}  // namespace histogram
}  // namespace balken


#endif
