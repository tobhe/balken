/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__FILTER_H__INCLUDED
#define BALKEN__FILTER_H__INCLUDED

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/StaticMatrix.h>
#include <iostream>

#include "view.h"

namespace balken {
namespace filter {
namespace detail {

auto gauss_3x3 =
  blaze::StaticMatrix<float, 3UL, 3UL>{{1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0},
                                       {2.0 / 16.0, 4.0 / 16.0, 2.0 / 16.0},
                                       {1.0 / 16.0, 2.0 / 16.0, 1.0 / 16.0}};

auto sobel_x =
  blaze::StaticMatrix<float, 3UL, 3UL>{{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
auto sobel_x_8 = blaze::StaticMatrix<float, 3UL, 3UL>{
  {-0.125, 0, 0.125}, {-0.25, 0, 0.25}, {-0.125, 0, 0.125}};
auto sobel_y =
  blaze::StaticMatrix<float, 3UL, 3UL>{{1, 2, 1}, {0, 0, 0}, {-1, -2, -1}};
auto sobel_45 =
  blaze::StaticMatrix<float, 3UL, 3UL>{{-1, -2, 0}, {-2, 0, 2}, {0, 2, 1}};
auto sobel_neg_45 =
  blaze::StaticMatrix<float, 3UL, 3UL>{{0, 2, 1}, {-2, 0, 2}, {-1, -2, 0}};

}  // namespace detail

namespace views {

template <class ImageT>
class BinaryView : public view::ViewBase<ImageT, BinaryView<ImageT>>
{
  using self_t = BinaryView<ImageT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  constexpr BinaryView(const ImageT & img, const uint8_t threshold)
   : base_t(img), _threshold(threshold) {}

public:
  constexpr ElementType view_element(std::size_t i, std::size_t j) const {
    return this->_img(i, j) > _threshold ? 255 : 0;
  }

private:
  const uint8_t _threshold;
};


template <class ImageT>
decltype(auto) binarize(const ImageT & img, std::size_t threshold) {
  return BinaryView<ImageT>(img, threshold);
}

}  // namespace views

template <class ImageT, class KernelT>
blaze::DynamicMatrix<uint8_t, blaze::rowMajor> convolve(
  const ImageT & img, const KernelT & kernel) {
  // Assert odd kernel dimensions
  assert(kernel.rows() % 2 != 0);
  assert(kernel.columns() % 2 != 0);

  blaze::DynamicMatrix<uint8_t, blaze::rowMajor> ret(img.rows(),
                                                     img.columns());

  size_t floor_half_h = floor(kernel.rows() / 2);
  size_t floor_half_w = floor(kernel.columns() / 2);

  for (size_t i = 0UL; i < ret.rows() - kernel.rows(); ++i) {
    for (size_t j = 0UL; j < ret.columns() - kernel.columns(); ++j) {
      for (size_t h = i; h < kernel.rows(); ++h) {
        for (size_t w = j; w < kernel.columns(); ++w) {
          ret(i + floor_half_h, j + floor_half_w) += static_cast<uint8_t>(
            kernel(i, j) * static_cast<float>(img(h + i, w + j)));
        }
      }
    }
  }
  return ret;
}

template <class ImageT>
decltype(auto) gauss(const ImageT & img) {
  return convolve(img, detail::gauss_3x3);
}

}  // namespace filter
}  // namespace balken

#endif
