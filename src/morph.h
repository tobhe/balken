/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__MORPH_H__INCLUDED
#define BALKEN__MORPH_H__INCLUDED

#include "view.h"

namespace balken {
namespace morph {

template <class ImageT, class StrucT>
class ErodedView : public view::ViewBase<ImageT, ErodedView<ImageT, StrucT>>
{
  using self_t = ErodedView<ImageT, StrucT>;
  using base_t = view::ViewBase<ImageT, self_t>;


public:
  using ElementType = typename ImageT::ElementType;

public:
  ErodedView(const ImageT & img, const StrucT & struc)
   : base_t(img), _struc{struc} {}

public:
  ElementType view_element(const ImageT & _img, size_t i, size_t j) const {
    size_t floor_half_h = floor(_struc.rows() / 2);
    size_t floor_half_w = floor(_struc.columns() / 2);

    if (i < (floor_half_h + 1) || i >= _img.rows() - (floor_half_h + 1) ||
        j < (floor_half_w + 1) || j >= _img.columns() - (floor_half_w + 1)) {
      return 0;
    }

    ElementType min{std::numeric_limits<ElementType>::max()};
    for (size_t h = 0; h < _struc.rows(); ++h) {
      for (size_t w = 0; w < _struc.columns(); ++w) {
        if ((_struc(h, w) == 1) &&
            (_img(i + h - floor_half_h, j + w - floor_half_w) < min)) {
          min = _img(i + h - floor_half_h, j + w - floor_half_w);
        }
      }
    }
    return min;
  }

private:
  const StrucT & _struc;
};

template <class ImageT, class StrucT>
class DilatedView : public view::ViewBase<ImageT, DilatedView<ImageT, StrucT>>
{
  using self_t = DilatedView<ImageT, StrucT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  DilatedView(const ImageT & img, const StrucT & struc)
   : base_t(img),
     _struc{struc},
     _floor_half_w{_struc.columns() / 2},
     _floor_half_h{_struc.rows() / 2} {}

public:
  constexpr ElementType view_element(const ImageT & _img,
                                     size_t         i,
                                     size_t         j) const {
    ElementType max{std::numeric_limits<ElementType>::min()};
    for (size_t h = 0; h < _struc.rows(); ++h) {
      for (size_t w = 0; w < _struc.columns(); ++w) {
        if ((i + h - _floor_half_h) >= 0 && (j + w - _floor_half_w) >= 0 &&
            (i + h - _floor_half_h) < _img.rows() &&
            (j + w - _floor_half_w) < _img.columns() && _struc(h, w) == 1 &&
            _img(i + h - _floor_half_h, j + w - _floor_half_h) > max) {
          max = _img(i + h - _floor_half_h, j + w - _floor_half_h);
        }
      }
    }
    return max;
  }

private:
  const StrucT & _struc;
  const size_t   _floor_half_w;
  const size_t   _floor_half_h;
};  // namespace morph

template <class ImageT, class StrucT>
decltype(auto) dilate(const ImageT & img, const StrucT & struc) {
  return DilatedView<ImageT, StrucT>(img, struc);
}

template <class ImageT, class StrucT>
decltype(auto) erode(const ImageT & img, const StrucT & struc) {
  return ErodedView<ImageT, StrucT>(img, struc);
}

}  // namespace morph
}  // namespace balken

#endif
