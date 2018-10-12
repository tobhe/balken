/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__DATAMATRIX_H__INCLUDED
#define BALKEN__DATAMATRIX_H__INCLUDED

// cpp
#include <iostream>
#include <string>
#include <utility>

// external
#include <blaze/math/DynamicMatrix.h>
#include <dmtx.h>

// own
#include "types.h"
#include "util.h"

#define DEBUG (1)

namespace balken {
namespace datamatrix {
namespace detail {

/**
 * Returns the size of a single module
 *
 * \param[in] top_left  Most top-left pixel of top-left module in datamatrix
 * \param[in] image     Image
 *
 * \return Size of module or -1 in case of error.
 */
template <class ImageT>
int module_size(const Point    top_left,
                const Point    bottom_right,
                const ImageT & image) {
  auto cur   = image(top_left.i, top_left.j);
  auto count = size_t{1};

  for (int i = 1UL; i <= bottom_right.j; ++i) {
    if (image(top_left.i, top_left.j + i) != cur) {
      cur = image(top_left.i, top_left.j + i);
      ++count;
    }
  }

  // smallest possible mat is size 6
  if (count > 6) {
    auto distance = bottom_right.j - top_left.j;
    std::cout << "Distance, Count: " << distance << ", " << count << '\n';
    return static_cast<int>(round(distance / static_cast<float>(count)));
  }
  return -1;
}


/**
 * Find most top-left black bit in image
 *
 * \param[in] image  Image
 *
 * \return pair(x,y) or pair(-1,-1)
 */
template <class ImageT>
Point find_top_left_black(const ImageT & image) {
  // Iterate over Matrix
  for (size_t i = 0UL; i < image.rows(); ++i) {
    for (size_t j = 0UL; j < image.columns(); ++j) {
      if (image(i, j) == 0) {
        std::cout << "TopLeft: " << i << ", " << j << '\n';
        return Point(i, j);
      }
    }
  }
  return Point(-1, -1);
}


/**
 * Find most bottom-right black bit in image
 *
 * \param[in] image  Image
 *
 * \return  pair(x,y) or pair(-1,-1)
 */
template <class ImageT>
auto find_bottom_right_black(const ImageT & image) {
  // Iterate over Matrix
  for (int i = static_cast<int>(image.rows() - 1); i >= 0; --i) {
    for (int j = static_cast<int>(image.columns() - 1); j >= 0; --j) {
      if (image(i, j) == 0) {
        std::cout << "BottomRight: " << i << ", " << j << '\n';
        return Point(i, j);
      }
    }
  }
  return Point(-1, -1);
}

template <class CodeT>
auto access_wrap(int row, int column, CodeT && img) {
  if (row < 0) {
    row += img.rows();
    column += 4 - ((img.rows() + 4) % 8);
  }
  if (column < 0) {
    column += img.columns();
    row += 4 - ((img.columns() + 4) % 8);
  }
  return img(row, column);
}

/**
 * Parse content of a codeword.
 *
 * Codewords are regions in the mosaic part of the datamatrix encoding a single
 * byte of information. Perfect codewords are L-shaped, as illustrated below:
 *
 *   1 1 - - - - - -
 *   1 1 1 2 2 - - -  single
 *   1 1 1 2 2 2 - -  region   1 1
 *   0 3 3 2 2 2 - -  ======>  1 1 1
 *   0 3 3 3 3 4 4 -           1 1 1
 *   - 3 3 3 3 4 4 4
 *   - - - - - 4 4 4
 *   - - - - - - - -
 *
 * Codewords at the border of the mosaic region are non-perfect in shape as
 * they "wrap-over" to the other edge of the region.
 *
 * \param[in] corner  Bottom left corner of the codewords
 * \param[in] img     Image
 *
 * \return  Decoded byte value
 */
template <class CodeT>
uint8_t decode_codeword(Point corner, const CodeT & img) {
  uint8_t val{0};
  uint8_t count{0};

  int column = corner.j;
  int row    = corner.i;

  // Handle special corner cases
  if (row == static_cast<int>(img.rows() - 2) && column == 0) {
    /* Corner case 2
     *   . . --------+
     *            4 3|
     *              2|
     * .            1|
     * .            0.
     * |             .
     * |             .
     * |             |
     * |7 6 5        |
     * +--- . . . ---+
     */
    val |= (access_wrap(0, img.rows() - 1, img) << 7);
    val |= (access_wrap(1, img.rows() - 1, img) << 6);
    val |= (access_wrap(2, img.rows() - 1, img) << 5);
    val |= (access_wrap(img.columns() - 2, 0, img) << 4);
    val |= (access_wrap(img.columns() - 1, 0, img) << 3);
    val |= (access_wrap(img.columns() - 1, 1, img) << 2);
    val |= (access_wrap(img.columns() - 1, 2, img) << 1);
    val |= (access_wrap(img.columns() - 1, 3, img) << 0);

  } else if ((row == static_cast<int>(img.rows() - 2)) and (column == 0) and
             (img.columns() % 4)) {
    /* Corner case 2
     *   . . --------+
     *        4 3 2 1|
     *              0|
     * .             |
     * .             .
     * |             .
     * |7            .
     * |6            |
     * |5            |
     * +--- . . . ---+
     */
    val |= (access_wrap(0, img.rows() - 3, img) << 7);
    val |= (access_wrap(0, img.rows() - 2, img) << 6);
    val |= (access_wrap(0, img.rows() - 1, img) << 5);
    val |= (access_wrap(img.columns() - 4, 0, img) << 4);
    val |= (access_wrap(img.columns() - 3, 0, img) << 3);
    val |= (access_wrap(img.columns() - 2, 0, img) << 2);
    val |= (access_wrap(img.columns() - 1, 0, img) << 1);
    val |= (access_wrap(img.columns() - 1, 1, img) << 0);
  } else if ((row == static_cast<int>(img.rows() - 2)) and (column == 0) and
             ((img.columns() % 8) == 4)) {
    /* Corner case 3
     *   . . --------+
     *            4 3|
     *              2|
     * .            1|
     * .            0.
     * |             .
     * |7            .
     * |6            |
     * |5            |
     * +--- . . . ---+
     */
    val |= (access_wrap(0, img.rows() - 3, img) << 7);
    val |= (access_wrap(0, img.rows() - 2, img) << 6);
    val |= (access_wrap(0, img.rows() - 1, img) << 5);
    val |= (access_wrap(img.columns() - 2, 0, img) << 4);
    val |= (access_wrap(img.columns() - 1, 0, img) << 3);
    val |= (access_wrap(img.columns() - 1, 1, img) << 2);
    val |= (access_wrap(img.columns() - 1, 2, img) << 1);
    val |= (access_wrap(img.columns() - 1, 3, img) << 0);
  } else if ((row == static_cast<int>(img.rows() + 4)) and (column == 2) and
             !(img.columns() % 8)) {
    /* Corner case 4
     *         ------+
     *          5 4 3|
     *          2 1 0|
     *               |
     * .             .
     * .             .
     * .             .
     * |             |
     * |7            |
     * +--- . . . ---+
     */
    val |= (access_wrap(0, img.rows() - 1, img) << 7);
    val |= (access_wrap(img.columns() - 1, img.rows() - 1, img) << 6);
    val |= (access_wrap(img.columns() - 3, 0, img) << 5);
    val |= (access_wrap(img.columns() - 2, 0, img) << 4);
    val |= (access_wrap(img.columns() - 1, 0, img) << 3);
    val |= (access_wrap(img.columns() - 3, 1, img) << 2);
    val |= (access_wrap(img.columns() - 2, 2, img) << 1);
    val |= (access_wrap(img.columns() - 1, 3, img) << 0);

  } else {
    // Default
    for (int i = 0; i < 3; ++i) {
      for (int j = 0; j < 3; ++j) {
        if (i == 2 && j == 0) { continue; }

        auto x = corner.j - j;
        auto y = corner.i - i;

        if (y < 0) {
          y += img.rows();
          x += 4 - ((img.rows() + 4) % 8);
        }
        if (x < 0) {
          x += img.columns();
          y += 4 - ((img.columns() + 4) % 8);
        }

        val |= (img(y, x) << count++);
      }
    }
  }
  return val;
}

}  // namespace detail


/**
 * Extracts the actual bitmap of only the datamatrix from a
 * non-distorted or transformed binary image
 *
 * \param[in] img  Binary image
 *
 * \return  Matrix of inner data matrix section
 */
template <class ImageT>
auto code(const ImageT & img) {
  // find top left and bottom right pixels
  auto top_left     = detail::find_top_left_black(img);
  auto bottom_right = detail::find_bottom_right_black(img);

  // find min and max
  auto top  = top_left.i;
  auto left = top_left.j;

  // Count change of values to get width
  auto cur   = img(top_left.i, top_left.j);
  auto count = size_t{1};
  for (int i = 1UL; i <= bottom_right.j; ++i) {
    if (img(top_left.i, top_left.j + i) != cur) {
      cur = img(top_left.i, top_left.j + i);
      ++count;
    }
  }

  auto mod_size = int{0};
  // smallest possible mat is size 6
  if (count > 6) {
    auto distance = bottom_right.j - top_left.j;
    std::cout << "Distance, Count: " << distance << ", " << count << '\n';
    mod_size = static_cast<int>(round(distance / static_cast<float>(count)));
  }

  // Assume width == height
  auto matrix_width  = count;
  auto matrix_height = count;
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

/**
 * Decode a datamatrix code and return a vector of bytes of the matrices
 * content.
 *
 * \param[in] code  Bitmap of a datamatrix code
 *
 * \return  vector of bytes of matrix content
 */
template <class CodeT>
std::vector<uint8_t> decode(const CodeT & code) {
  // Fix coordinates of codeword #1
  auto res = std::vector<uint8_t>();

  auto column = 0;
  auto row    = 4;

  auto inner =
    blaze::submatrix(code, 1UL, 1UL, code.rows() - 2, code.columns() - 2);

  // #ifdef DEBUG
  //   std::cout << "(" << column << ", " << row
  //             << ") = " << static_cast<int>(res.back()) << std::endl;
  // #endif

  while (row < static_cast<int>(inner.rows()) ||
         column < static_cast<int>(inner.columns())) {
    if (row == static_cast<int>(inner.rows() - 2) && column == 0) {
      res.push_back(detail::decode_codeword(Point(row, column), inner));
    } else if ((row == static_cast<int>(inner.rows() - 2)) and
               (column == 0) and (inner.columns() % 4)) {
      res.push_back(detail::decode_codeword(Point(row, column), inner));

    } else if ((row == static_cast<int>(inner.rows() - 2)) and
               (column == 0) and ((inner.columns() % 8) == 4)) {
      res.push_back(detail::decode_codeword(Point(row, column), inner));
    } else if ((row == static_cast<int>(inner.rows() + 4)) and
               (column == 2) and !(inner.columns() % 8)) {
      res.push_back(detail::decode_codeword(Point(row, column), inner));
    }

    while ((row >= 0) && column < static_cast<int>(inner.columns())) {
      res.push_back(detail::decode_codeword(Point(row, column), inner));
      row -= 2;
      column += 2;
#ifdef DEBUG
      std::cout << "(" << column << ", " << row
                << ") = " << static_cast<int>(res.back()) << std::endl;
#endif
    }
    row += 1;
    column += 3;

#ifdef DEBUG
    std::cout << "(" << column << ", " << row
              << ") = " << static_cast<int>(res.back()) << std::endl;
#endif

    while ((column >= 0) and row < static_cast<int>(inner.rows())) {
      res.push_back(detail::decode_codeword(Point(row, column), inner));
      row += 2;
      column -= 2;
#ifdef DEBUG
      std::cout << "(" << column << ", " << row
                << ") = " << static_cast<int>(res.back()) << std::endl;
#endif
    }
    row += 3;
    column += 1;
#ifdef DEBUG
    std::cout << "(" << column << ", " << row
              << ") = " << static_cast<int>(res.back()) << std::endl;
#endif
  }

  return res;
}

template <class CodeT>
int dmtx_decode(const CodeT & code) {
  auto _data = std::vector<uint8_t>();
  for (size_t i = 0; i < code.rows(); ++i) {
    for (size_t j = 0; j < code.columns(); ++j) {
      _data.push_back(code(i, j));
    }
  }

  for (size_t i = 0; i < code.rows(); ++i) {
    for (size_t j = 0; j < code.columns(); ++j) {
      std::cout << static_cast<int>(_data.data()[j + (i * j)] == 255 ? 1 : 0)
                << ' ';
    }
    std::cout << '\n';
  }

  DmtxImage * img =
    dmtxImageCreate(_data.data(), code.columns(), code.rows(), DmtxPack8bppK);
  assert(img != NULL);

  DmtxDecode * dec = dmtxDecodeCreate(img, 1);
  assert(dec != NULL);

  DmtxRegion * reg = dmtxRegionFindNext(dec, NULL);
  std::cout << "Looking for region" << '\n';

  DmtxMessage * msg;
  if (reg != NULL) {
    std::cout << "Found region" << '\n';
    msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
    if (msg != NULL) {
      for (size_t i = 0; i < static_cast<size_t>(msg->outputIdx); ++i) {
        std::cout << msg->output[i];
      }
      std::cout << '\n';
      dmtxMessageDestroy(&msg);
    }
    dmtxRegionDestroy(&reg);
  }
  dmtxDecodeDestroy(&dec);
  dmtxImageDestroy(&img);

  return 0;
}

}  // namespace datamatrix
}  // namespace balken

#endif
