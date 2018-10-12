/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__HISTOGRAM_H__INCLUDED
#define BALKEN__HISTOGRAM_H__INCLUDED

#include <util.h>
#include <cmath>
#include <limits>
#include <unordered_map>
#include <utility>
#include "view.h"

namespace balken {
namespace histogram {
namespace detail {

template <class ImageT>
auto generate(const ImageT & img) {
  auto hist =
    std::array<float,
               std::numeric_limits<typename ImageT::ElementType>::max() + 1>();

  for (size_t i = 0UL; i < img.rows(); ++i) {
    for (size_t j = 0UL; j < img.columns(); ++j) { ++hist[img(i, j)]; }
  }

  for (size_t i = 0UL; i < hist.size(); ++i) {
    hist[i] = hist[i] / (img.columns() * img.rows());
  }

  return hist;
}

template <class HistT>
auto accumulate(HistT & hist) {
  for (size_t i = 1UL; i < hist.size(); ++i) { hist[i] += hist[i - 1]; }
  return hist;
}

}  // namespace detail

namespace views {

template <class ImageT>
class StretchedView : public view::ViewBase<ImageT, StretchedView<ImageT>>
{
  using self_t = StretchedView<ImageT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  // blaze
  using ElementType = typename ImageT::ElementType;

public:
  StretchedView() = delete;
  StretchedView(const ImageT & img)
   : base_t(img), _max(blaze::max(img)), _min(blaze::min(img)) {}
  ~StretchedView() = default;

  // Element Access
private:
  constexpr uint8_t normalize(const ElementType & ref) const {
    auto factor =
      std::numeric_limits<uint8_t>::max() / static_cast<float>(_max - _min);
    return static_cast<uint8_t>((ref - _min) * factor);
  }

public:
  constexpr uint8_t operator()(size_t i, size_t j) const {
    return normalize(this->_img(i, j));
  }

private:
  const ElementType _max;
  const ElementType _min;
};

template <class ImageT>
auto stretch(const ImageT & img) {
  return StretchedView<ImageT>(img);
}

}  // namespace views

template <class ImageT>
auto equalize(ImageT & img) {
  auto hist     = detail::generate(img);
  auto cum_hist = detail::accumulate(hist);

  for (size_t i = 0UL; i < img.rows(); ++i) {
    for (size_t j = 0UL; j < img.columns(); ++j) {
      img(i, j) =
        floor(std::numeric_limits<typename ImageT::ElementType>::max() *
              cum_hist[img(i, j)]);
    }
  }
  return img;
}

template <class ImageT>
decltype(auto) stretch(ImageT && img) {
  const int max = std::numeric_limits<typename ImageT::ElementType>::max();

  auto hist    = detail::generate(img);
  int  lowest  = static_cast<int>(max);
  int  highest = 0;

  for (int i = 0; i < static_cast<int>(hist.size()); ++i) {
    if (hist[i] && (lowest == max)) {
      lowest  = i;
      highest = i;
    } else if (hist[i]) {
      highest = i;
    }
  }

  float factor = static_cast<float>(max) / (highest - lowest);

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
