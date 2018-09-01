#ifndef BALKEN__DATAMATRIX_H__INCLUDED
#define BALKEN__DATAMATRIX_H__INCLUDED

// cpp
#include <iostream>
#include <string>
#include <utility>

// external
#include <blaze/math/DynamicMatrix.h>

namespace balken::datamatrix {

/**
 * Returns the size of a single module
 *
 * \param[in] top_left  Most top-left pixel of top-left module in datamatrix
 * \param[in] image     Image
 *
 * \return Size of module or -1 in case of error.
 */
int module_size(const std::pair<int, int> top_left,
                const blaze::DynamicMatrix<uint8_t, blaze::rowMajor> & image) {
  for (size_t i = 1UL; i < image.columns(); ++i) {
    if (image(top_left.second, top_left.first + i) == 0) { return i; }
  }
  // Only if only black
  return -1;
}


/**
 * Find most top-left black bit in image
 *
 * \param[in] image  Image
 *
 * \return pair(x,y) or pair(-1,-1)
 */
auto find_top_left_black(
  const blaze::DynamicMatrix<uint8_t, blaze::rowMajor> & image) {
  // Iterate over Matrix
  for (size_t i = 0UL; i < image.rows(); ++i) {
    for (size_t j = 0UL; j < image.columns(); ++j) {
      if (image(i, j) == 1) {
        return std::pair(static_cast<int>(j), static_cast<int>(i));
      }
    }
  }
  return std::pair<int, int>(-1, -1);
}


/**
 * Find most bottom-right black bit in image
 *
 * \param[in] image  Image
 *
 * \return  pair(x,y) or pair(-1,-1)
 */
auto find_bottom_right_black(
  const blaze::DynamicMatrix<uint8_t, blaze::rowMajor> & image) {
  // Iterate over Matrix
  for (int i = static_cast<int>(image.rows() - 1); i >= 0; --i) {
    for (int j = static_cast<int>(image.columns() - 1); j >= 0; --j) {
      if (image(i, j) == 1) { return std::pair(j, i); }
    }
  }
  return std::pair<int, int>(-1, -1);
}

/**
 * Extracts the actual bitmap of only the datamatrix from a
 * non-distorted or transformed binary image
 *
 * \param[in] img  Binary image
 *
 * \return  Matrix of inner data matrix section
 */
auto extract_bitmap(const blaze::DynamicMatrix<uint8_t> & img) {
  // find top left and bottom right pixels
  auto top_left     = find_top_left_black(img);
  auto bottom_right = find_bottom_right_black(img);

  // find min and max
  auto top    = top_left.second;
  auto bottom = bottom_right.second;
  auto left   = top_left.first;
  auto right  = bottom_right.first;

  auto mod_size = module_size(top_left, img);

  auto matrix_width  = ((right + 1) - left) / mod_size;
  auto matrix_height = ((bottom + 1) - top) / mod_size;
  if (matrix_height <= 0 && matrix_width <= 0) {
    // return empty
    return blaze::DynamicMatrix<uint8_t>();
  }

  auto inner_mat = blaze::DynamicMatrix<uint8_t>(matrix_height, matrix_width);
  for (int i = 0; i < static_cast<int>(inner_mat.rows()); ++i) {
    for (int j = 0; j < static_cast<int>(inner_mat.columns()); ++j) {
      inner_mat(i, j) = img(top + (mod_size / 2) + (i * mod_size),
                            left + (mod_size / 2) + (j * mod_size));
    }
  }
  return inner_mat;
}

std::string decode(const blaze::DynamicMatrix<uint8_t> & img);

// Works only with 10x10
template <class T>
uint8_t decode_region(std::pair<int, int> corner, const T & img) {
  uint8_t val{0};
  uint8_t count{0};

#ifdef DEBGUG
  std::cout << "Image: " << img.rows() << "x" << img.columns() << '\n';
#endif

  for (int i = 0; i < 3; ++i) {
    for (int j = 0; j < 3; ++j) {
      if (i == 2 && j == 0) { continue; }

      auto x = corner.first - j;
      auto y = corner.second - i;

      if (y < 0) {
        y += img.rows();
        x = (x + 8) % img.columns();
      }
      if (x < 0) {
        x += img.columns();
        y = (y + 8) % img.rows();
      }

#ifdef DEBUG
      std::cout << '(' << x << ", " << y << ')' << '='
                << static_cast<int>(img(y, x)) << '\n';
#endif

      val |= (img(y, x) << count++);
    }
  }
  return val;
}


}  // namespace balken::datamatrix

#endif
