/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__MORPH_H__INCLUDED
#define BALKEN__MORPH_H__INCLUDED

#include <cstdint>
#include <limits>
#include "view.h"

namespace balken {
namespace morph {
namespace views {

template <class ImageT, class StrucT>
class ErodedView : public view::ViewBase<ImageT, ErodedView<ImageT, StrucT>>
{
  using self_t = ErodedView<ImageT, StrucT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  constexpr ErodedView(const ImageT & img, const StrucT & struc)
   : base_t(img),
     _struc{struc},
     _floor_half_w{_struc.columns() / 2},
     _floor_half_h{_struc.rows() / 2} {}

public:
  constexpr ElementType view_element(std::size_t i, std::size_t j) const {
    if (i < (_floor_half_h + 1) ||
        i >= this->_img.rows() - (_floor_half_h + 1) ||
        j < (_floor_half_w + 1) ||
        j >= this->_img.columns() - (_floor_half_w + 1)) {
      return 0;
    }

    ElementType min{std::numeric_limits<ElementType>::max()};
    for (std::size_t h = 0; h < _struc.rows(); ++h) {
      for (std::size_t w = 0; w < _struc.columns(); ++w) {
        if ((_struc(h, w) == 1) &&
            (this->_img(i + h - _floor_half_h, j + w - _floor_half_w) < min)) {
          min = this->_img(i + h - _floor_half_h, j + w - _floor_half_w);
        }
      }
    }
    return min;
  }

private:
  const StrucT &    _struc;
  const std::size_t _floor_half_w;
  const std::size_t _floor_half_h;
};

template <class ImageT, class StrucT>
class DilatedView : public view::ViewBase<ImageT, DilatedView<ImageT, StrucT>>
{
  using self_t = DilatedView<ImageT, StrucT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  constexpr DilatedView(const ImageT & img, const StrucT & struc)
   : base_t(img),
     _struc{struc},
     _floor_half_w{_struc.columns() / 2},
     _floor_half_h{_struc.rows() / 2} {}

public:
  constexpr ElementType view_element(const std::size_t i,
                                     const std::size_t j) const {
    if (i < (_floor_half_h + 1) ||
        i >= this->_img.rows() - (_floor_half_h + 1) ||
        j < (_floor_half_w + 1) ||
        j >= this->_img.columns() - (_floor_half_w + 1)) {
      return 0;
    }

    ElementType max{std::numeric_limits<ElementType>::min()};
    for (std::size_t h = 0; h < _struc.rows(); ++h) {
      for (std::size_t w = 0; w < _struc.columns(); ++w) {
        if ((_struc(h, w) == 1) &&
            (this->_img(i + h - _floor_half_h, j + w - _floor_half_w) > max)) {
          max = this->_img(i + h - _floor_half_h, j + w - _floor_half_w);
        }
      }
    }
    return max;
  }

private:
  const StrucT &    _struc;
  const std::size_t _floor_half_w;
  const std::size_t _floor_half_h;
};

/**
 * View Constructor Wrappers
 */
template <class ImageT, class StrucT>
decltype(auto) dilate(const ImageT & img, const StrucT & struc) {
  return DilatedView<ImageT, StrucT>(img, struc);
}

template <class ImageT, class StrucT>
decltype(auto) erode(const ImageT & img, const StrucT & struc) {
  return ErodedView<ImageT, StrucT>(img, struc);
}

template <class LeftImageT, class RightImageT>
RightImageT & operator>>(LeftImageT & l, RightImageT & r) {
  return r;
}
}  // namespace views

/**
 * Free Functions
 */

template <class ImageT, class KernelT>
blaze::DynamicMatrix<uint8_t, blaze::rowMajor> erode(const ImageT &  img,
                                                     const KernelT & kernel) {
  // Assert odd kernel dimensions
  assert(kernel.rows() % 2 != 0);
  assert(kernel.columns() % 2 != 0);

  auto ret = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>(
    img.rows(), img.columns(), 0UL);

  size_t floor_half_h = floor(kernel.rows() / 2);
  size_t floor_half_w = floor(kernel.columns() / 2);

  for (size_t i = 0; i < img.rows() - kernel.rows(); ++i) {
    for (size_t j = 0; j < img.columns() - kernel.columns(); ++j) {
      uint8_t min = 255;
      for (size_t h = 0; h < kernel.rows(); ++h) {
        for (size_t w = 0; w < kernel.rows(); ++w) {
          if ((kernel(h, w) == 1) && (img(i + h, j + w) < min)) {
            min = img(i + h, j + w);
          }
        }
      }
      ret(i + floor_half_h, j + floor_half_w) = min;
    }
  }
  return ret;
}

template <class ImageT, class KernelT>
blaze::DynamicMatrix<uint8_t, blaze::rowMajor> dilate(const ImageT &  img,
                                                      const KernelT & kernel) {
  // Assert odd kernel dimensions
  assert(kernel.rows() % 2 != 0);
  assert(kernel.columns() % 2 != 0);

  auto ret = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>(
    img.rows(), img.columns(), 0UL);

  size_t floor_half_h = floor(kernel.rows() / 2);
  size_t floor_half_w = floor(kernel.columns() / 2);

  for (size_t i = 0; i < img.rows() - kernel.rows(); ++i) {
    for (size_t j = 0; j < img.columns() - kernel.columns(); ++j) {
      uint8_t max{0};
      for (size_t h = 0; h < kernel.rows(); ++h) {
        for (size_t w = 0; w < kernel.rows(); ++w) {
          if ((kernel(h, w) == 1) && (img(i + h, j + w) > max)) {
            max = img(i + h, j + w);
          }
        }
      }
      ret(i + floor_half_h, j + floor_half_w) = max;
    }
  }
  return ret;
}

template <class ImageT, class KernelT>
auto open(const ImageT & img, const KernelT & kernel) {
  return dilate(erode(img, kernel), kernel);
}

template <class ImageT, class KernelT>
auto close(const ImageT & img, const KernelT & kernel) {
  return erode(dilate(img, kernel), kernel);
}

}  // namespace morph
}  // namespace balken

#endif
