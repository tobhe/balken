#ifndef BALKEN__FILTER_H__INCLUDED
#define BALKEN__FILTER_H__INCLUDED

#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/StaticMatrix.h>

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

template <class ImageT, class KernelT>
ImageT convolve(const ImageT & img, const KernelT & kernel) {
  blaze::DynamicMatrix<uint8_t, blaze::rowMajor> ret(
    img.rows() - kernel.rows() + 1, img.columns() - kernel.columns() + 1);

  for (size_t i = 0UL; i < ret.rows(); ++i) {
    for (size_t j = 0UL; j < ret.columns(); ++j) {
      for (size_t h = i; h < i + kernel.rows(); ++h) {
        for (size_t w = j; w < j + kernel.columns(); ++w) {
          ret(i, j) += static_cast<uint8_t>(kernel(h - i, w - j) *
                                            static_cast<float>(img(h, w)));
        }
      }
    }
  }
  return ret;
}

template <class ImageT, class KernelT>
ImageT erode(const ImageT & img, const KernelT & kernel) {
  blaze::DynamicMatrix<uint8_t, blaze::rowMajor> ret(
    img.rows() - kernel.rows() + 1, img.columns() - kernel.columns() + 1, 255);

  for (size_t i = 0UL; i < ret.rows(); ++i) {
    for (size_t j = 0UL; j < ret.columns(); ++j) {
      int acc{false};
      for (size_t h = i; h < i + kernel.rows(); ++h) {
        for (size_t w = j; w < j + kernel.columns(); ++w) {
          if ((kernel(h - i, w - j) == 1) && (img(h, w) == 255)) {
            acc = true;
          }
        }
      }
      ret(i, j) = acc ? 255 : 0;
    }
  }
  return ret;
}


template <class ImageT, class KernelT>
ImageT dilate(const ImageT & img, const KernelT & kernel) {
  blaze::DynamicMatrix<uint8_t, blaze::rowMajor> ret(
    img.rows() - kernel.rows() + 1, img.columns() - kernel.columns() + 1, 0);

  for (size_t i = 0UL; i < ret.rows(); ++i) {
    for (size_t j = 0UL; j < ret.columns(); ++j) {
      bool acc{true};
      for (size_t h = i; h < i + kernel.rows(); ++h) {
        for (size_t w = j; w < j + kernel.columns(); ++w) {
          if ((kernel(h - i, w - j) == 1) && (img(h, w) == 255)) {
            acc = acc && true;
          } else {
            acc = false;
          }
        }
      }
      ret(i, j) = acc ? 255 : 0;
    }
  }
  return ret;
}

template <class ImageT>
decltype(auto) gauss(const ImageT & img) {
  return convolve(img, detail::gauss_3x3);
}

template <class ImageT>
auto binary(ImageT & img, const int threshold) {
  for (size_t i = 0UL; i < img.rows(); ++i) {
    for (size_t j = 0UL; j < img.columns(); ++j) {
      img(i, j) = abs(img(i, j)) > threshold ? 255 : 0;
    }
  }
  return img;
}

template <class ImageT, class KernelT>
auto open(const ImageT & img, const KernelT & kernel) {
  return dilate(erode(img, kernel), kernel);
}

int orientation(std::pair<int, int> p,
                std::pair<int, int> q,
                std::pair<int, int> r) {
  int val = (q.second - p.first) * (r.second - q.second) -
            (q.second - p.second) * (r.first - q.first);

  if (val == 0) return 0;    // colinear
  return (val > 0) ? 1 : 2;  // clock or counterclock wise
}

template <class PointsT>
auto convex_hull(const PointsT & points) {
  auto hull = std::vector<std::pair<int, int>>();
  auto left = std::pair<int, int>(500, 500);

  for (auto i : points) {
    if (i.second < left.second) { left = i; }
  }

  auto p = int{1};
  auto q = int{};
  do {
    hull.push_back(points[p]);

    q = (p + 1) % points.size();
    for (int i = 0; i < points.size(); i++) {
      // If i is more counterclockwise than current q, then
      // update q
      if (orientation(points[p], points[i], points[q]) == 2) { q = i; }
    }

    p = q;
  } while (p != 1);
  return hull;
}

template <class ImageT>
auto cluster_by_threshold(const ImageT & img, uint8_t threshold) {
  // cpy image to mark already visited
  auto ids = blaze::DynamicMatrix<uint8_t>(img.rows(), img.columns(), 0UL);

  std::cout << "rows: " << img.rows() << ", " << ids.rows() << '\n';
  std::cout << "columns: " << img.columns() << ", " << ids.columns() << '\n';
  // List of pairs (i,j)
  auto region_list = std::vector<std::vector<std::pair<int, int>>>();

  // Go over all pixels
  for (size_t i = 0UL; i < img.rows(); ++i) {
    for (size_t j = 0UL; j < img.columns(); ++j) {
      // Get white pixel
      if (img(i, j) > threshold && ids(i, j) == 0) {
        region_list.emplace_back();

        // Traverse
        auto stack = std::vector<std::pair<int, int>>();
        stack.emplace_back(i, j);
        auto id = 1;
        while (!stack.empty()) {
          auto cur = stack.back();
          stack.pop_back();

          if (img(cur.first, cur.second) == 0 ||
              ids(cur.first, cur.second) > 0) {
            continue;
          }

          ids(cur.first, cur.second) = id;
          region_list.back().push_back(cur);

          if (cur.second > 0) {
            stack.emplace_back(cur.first, cur.second - 1);
          }
          if (cur.second < (static_cast<int>(ids.columns()) - 1)) {
            stack.emplace_back(cur.first, cur.second + 1);
          }
          if (cur.first > 0) { stack.emplace_back(cur.first - 1, cur.second); }
          if (cur.first < (static_cast<int>(ids.rows()) - 1)) {
            stack.emplace_back(cur.first + 1, cur.second);
          }
        }
        ++id;
      }
    }
  }

  return region_list;
}

template <class ImageT>
auto find_regions(const ImageT & img, const int threshold, const int M) {
  auto gauss_img    = gauss(gauss(img));
  auto sobel_x_img  = convolve(gauss_img, detail::sobel_x_8);
  auto sobel_y_img  = convolve(gauss_img, detail::sobel_y);
  auto sobel_tr_img = convolve(gauss_img, detail::sobel_45);
  auto sobel_br_img = convolve(gauss_img, detail::sobel_neg_45);

  auto bin_x  = binary(sobel_x_img, threshold);
  auto bin_y  = binary(sobel_y_img, threshold);
  auto bin_tr = binary(sobel_tr_img, threshold);
  auto bin_br = binary(sobel_br_img, threshold);

  auto kernel = blaze::DynamicMatrix<uint8_t>(
    static_cast<uint32_t>(M), static_cast<uint32_t>(M), 1);


  auto dil_x = open(bin_x, kernel);
  auto dil_y = open(bin_y, kernel);

  blaze::DynamicMatrix<uint8_t, blaze::rowMajor> ret(
    dil_x.rows(), dil_x.columns(), 0);

  for (size_t i = 0UL; i < ret.rows(); ++i) {
    for (size_t j = 0UL; j < ret.columns(); ++j) {
      ret(i, j) = (dil_y(i, j) == 255) and (dil_x(i, j) == 255) ? 255 : 0;
    }
  }

  return dil_x;
}

}  // namespace filter
}  // namespace balken

#endif
