/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__VIEW_H__INCLUDED
#define BALKEN__VIEW_H__INCLUDED

#include <cstddef>

namespace balken {
namespace view {

template <class ImageT, class ViewT>
class ViewBase
{
  using self_t    = ViewBase<ImageT, ViewT>;
  using derived_t = ViewT;

private:
  derived_t &                 derived() { return static_cast<ViewT &>(*this); }
  constexpr const derived_t & derived() const {
    return static_cast<const derived_t &>(*this);
  }

public:
  constexpr ViewBase()               = delete;
  constexpr ViewBase(const self_t &) = default;
  constexpr ViewBase(self_t &&)      = default;
  constexpr ViewBase(const ImageT & img) : _img{img} {}
  ~ViewBase() = default;

  constexpr decltype(auto) operator()(size_t i, size_t j) const {
    return derived().view_element(i, j);
  }

  // Dimensions
  constexpr auto rows() const { return _img.rows(); }
  constexpr auto columns() const { return _img.columns(); }

protected:
  const ImageT & _img;
};

}  // namespace view
}  // namespace balken

#endif
