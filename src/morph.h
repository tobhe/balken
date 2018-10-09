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
  constexpr ElementType view_element(const ImageT & _img,
                                     std::size_t    i,
                                     std::size_t    j) const {
    return _img(i, j) > _threshold ? 255 : 0;
  }

private:
  const uint8_t _threshold;
};

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
  constexpr ElementType view_element(const ImageT & _img,
                                     std::size_t    i,
                                     std::size_t    j) const {
    if (i < (_floor_half_h + 1) || i >= _img.rows() - (_floor_half_h + 1) ||
        j < (_floor_half_w + 1) || j >= _img.columns() - (_floor_half_w + 1)) {
      return 0;
    }

    ElementType min{std::numeric_limits<ElementType>::max()};
    for (std::size_t h = 0; h < _struc.rows(); ++h) {
      for (std::size_t w = 0; w < _struc.columns(); ++w) {
        if ((_struc(h, w) == 1) &&
            (_img(i + h - _floor_half_h, j + w - _floor_half_w) < min)) {
          min = _img(i + h - _floor_half_h, j + w - _floor_half_w);
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
  constexpr ElementType view_element(const ImageT &    _img,
                                     const std::size_t i,
                                     const std::size_t j) const {
    if (i < (_floor_half_h + 1) || i >= _img.rows() - (_floor_half_h + 1) ||
        j < (_floor_half_w + 1) || j >= _img.columns() - (_floor_half_w + 1)) {
      return 0;
    }

    ElementType max{std::numeric_limits<ElementType>::min()};
    for (std::size_t h = 0; h < _struc.rows(); ++h) {
      for (std::size_t w = 0; w < _struc.columns(); ++w) {
        if ((_struc(h, w) == 1) &&
            (_img(i + h - _floor_half_h, j + w - _floor_half_w) > max)) {
          max = _img(i + h - _floor_half_h, j + w - _floor_half_w);
        }
      }
    }
    return max;
  }

private:
  const StrucT &    _struc;
  const std::size_t _floor_half_w;
  const std::size_t _floor_half_h;
};  // namespace morph

template <class ImageT, class StrucT>
decltype(auto) dilate(const ImageT & img, const StrucT & struc) {
  return DilatedView<ImageT, StrucT>(img, struc);
}

template <class ImageT>
decltype(auto) binarize(const ImageT & img, std::size_t threshold) {
  return BinaryView<ImageT>(img, threshold);
}

template <class ImageT, class StrucT>
decltype(auto) erode(const ImageT & img, const StrucT & struc) {
  return ErodedView<ImageT, StrucT>(img, struc);
}

}  // namespace morph
}  // namespace balken

#endif
