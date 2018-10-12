/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__GEOMETRY_H__INCLUDED
#define BALKEN__GEOMETRY_H__INCLUDED

// cpp
#include <cmath>

// thirdparty
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/StaticMatrix.h>
#include <blaze/math/StaticVector.h>

// own
#include "view.h"

namespace balken {
namespace geometry {

template <class ImageT>
class ScaledView : public view::ViewBase<ImageT, ScaledView<ImageT>>
{
  using self_t = ScaledView<ImageT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  constexpr ScaledView(const ImageT & img, const size_t factor)
   : base_t(img),
     _factor{factor},
     _rows{img.rows() * factor},
     _columns{img.columns() * factor} {}

public:
  auto view_element(const int i, const int j) const {
    return this->_img(i / _factor, j / _factor);
  }

  constexpr size_t rows() const { return _rows; }
  constexpr size_t columns() const { return _columns; }

private:
  const size_t _factor;
  const size_t _rows;
  const size_t _columns;
};


template <class ImageT>
class TranslatedView : public view::ViewBase<ImageT, TranslatedView<ImageT>>
{
  using self_t = TranslatedView<ImageT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  constexpr TranslatedView(const ImageT & img, const int i, const int j)
   : base_t(img), _i(i), _j(j), _rows{img.rows()}, _columns{img.columns()} {}

  constexpr TranslatedView(const ImageT & img,
                           const int      i,
                           const int      j,
                           const size_t   rows,
                           const size_t   columns)
   : base_t(img), _i(i), _j(j), _rows{rows}, _columns{columns} {}

public:
  auto view_element(const int i, const int j) const {
    auto k = i - _i;
    auto l = j - _j;
    if (k >= 0 && k < static_cast<int>(this->_img.rows()) && l >= 0 &&
        l < static_cast<int>(this->_img.columns())) {
      return this->_img(k, l);
    }
    return static_cast<uint8_t>(0);
  }

  constexpr size_t rows() const { return _rows; }
  constexpr size_t columns() const { return _columns; }

private:
  const blaze::StaticMatrix<double, 2UL, 2UL> _M;
  const int                                   _i;
  const int                                   _j;
  const size_t                                _rows;
  const size_t                                _columns;
};

/**
 * Rotate image clockwise.
 */
template <class ImageT>
class RotatedView : public view::ViewBase<ImageT, RotatedView<ImageT>>
{
  using self_t = RotatedView<ImageT>;
  using base_t = view::ViewBase<ImageT, self_t>;

public:
  using ElementType = typename ImageT::ElementType;

public:
  constexpr RotatedView(const ImageT & img, const double angle)
   : base_t(img),
     _M({{std::cos(angle), -std::sin(angle)},
         {std::sin(angle), std::cos(angle)}}),
     _angle(angle),
     _rows{img.rows()},
     _columns{img.columns()} {}

  constexpr RotatedView(const ImageT & img,
                        const double   angle,
                        const size_t   rows,
                        const size_t   columns)
   : base_t(img),
     _M({{std::cos(angle), -std::sin(angle)},
         {std::sin(angle), std::cos(angle)}}),
     _angle(angle),
     _rows{rows},
     _columns{columns} {}

public:
  auto view_element(std::size_t i, std::size_t j) const {
    auto old_p = blaze::StaticVector<double, 2UL>{static_cast<double>(j),
                                                  -static_cast<double>(i)};

    decltype(old_p) new_p = _M * old_p;
    auto            k     = std::round(-new_p[1]);
    auto            l     = std::round(new_p[0]);
    if (k >= 0 && k < this->_img.rows() && l >= 0 &&
        l < this->_img.columns()) {
      return this->_img(k, l);
    }
    return static_cast<uint8_t>(0);
  }

  constexpr size_t rows() const { return _rows; }
  constexpr size_t columns() const { return _columns; }

private:
  const blaze::StaticMatrix<double, 2UL, 2UL> _M;
  const double                                _angle;
  const size_t                                _rows;
  const size_t                                _columns;
};

template <class ImageT>
decltype(auto) scale(const ImageT & img, const size_t factor) {
  return ScaledView<ImageT>(img, factor);
}

template <class ImageT>
decltype(auto) rotate(const ImageT & img, const double angle) {
  return RotatedView<ImageT>(img, angle);
}

template <class ImageT>
decltype(auto) rotate(const ImageT & img,
                      const double   angle,
                      const size_t   heigth,
                      size_t         width) {
  return RotatedView<ImageT>(img, angle, heigth, width);
}

template <class ImageT>
decltype(auto) translate(const ImageT & img, const int i, const int j) {
  return TranslatedView<ImageT>(img, i, j);
}

template <class ImageT>
decltype(auto) translate(const ImageT & img,
                         const int      i,
                         const int      j,
                         const size_t   heigth,
                         const size_t   width) {
  return TranslatedView<ImageT>(img, i, j, heigth, width);
}

}  // namespace geometry
}  // namespace balken
#endif
