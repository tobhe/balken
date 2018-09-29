/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef BALKEN__UTIL_H__INCLUDED
#define BALKEN__UTIL_H__INCLUDED

// external
#include <CImg.h>
#include <blaze/math/DynamicMatrix.h>

// own
#include "image.h"

namespace balken {
namespace util {

/**
 * \brief  Load image from file to greyscale image as Matrix
 *
 * \param[in]  filename  Name of file to load
 * \return     Matrix containing image
 */
blaze::DynamicMatrix<uint8_t> load_image(std::string filename) {
  auto cimg = cimg_library::CImg<unsigned char>(filename.c_str());
  auto img  = blaze::DynamicMatrix<uint8_t>(cimg._height, cimg._width);

  switch (cimg.spectrum()) {
    case 3:
      cimg_forXY(cimg, x, y) {
        auto R = static_cast<int>(cimg(x, y, 0));
        auto G = static_cast<int>(cimg(x, y, 1));
        auto B = static_cast<int>(cimg(x, y, 2));

        img(y, x) = static_cast<int>(0.33 * R + 0.33 * G + 0.33 * B);
      }
      return img;
      break;
    case 1:
      cimg_forXY(cimg, x, y) {
        // Save grey value weighted by perceived brightness
        img(y, x) = cimg(x, y, 0);
      }
      return img;
      break;
    default:
      std::cout << "Error: image not handled\n";
      break;
  }
  return blaze::DynamicMatrix<uint8_t>();
}

/**
 * \brief      View image from Matrix
 *
 * \param[in]  img  Matrix containing image
 */
template <class ImageT>
void view_image(const ImageT & img) {
  cimg_library::CImg<uint8_t> cimg(img.columns(), img.rows(), 1, 1, 0);
  for (size_t x = 0; x < img.columns(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) { cimg(x, y, 0) = img(y, x); }
  }
  auto disp = cimg_library::CImgDisplay();
  cimg.print();
  disp.set_normalization(0);
  cimg.display(disp);
  while (!disp.is_closed()) {
    disp.wait();
    if (disp.button()) { break; }
  }
}

template <class ImageT1, class ImageT2>
void compare(const ImageT1 & img, const ImageT2 & img2) {
  cimg_library::CImg<uint8_t> cimg(img.columns(), img.rows(), 1, 1, 0);
  for (size_t x = 0; x < img.columns(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) { cimg(x, y, 0) = img(y, x); }
  }

  cimg_library::CImg<uint8_t> cimg2(img2.columns(), img2.rows(), 1, 1, 0);
  for (size_t x = 0; x < img2.columns(); ++x) {
    for (size_t y = 0; y < img2.rows(); ++y) { cimg2(x, y, 0) = img2(y, x); }
  }

  auto disp = cimg_library::CImgDisplay(1920, 1080, 0, 0, false, false);

  cimg.print();
  cimg2.print();
  (cimg, cimg2).display(disp);
  while (!disp.is_closed()) {
    disp.wait();
    if (disp.button()) { break; }
  }
}

template <class ImageT>
inline void print_image(const ImageT & img) {
  for (size_t i = 0; i < img.rows(); ++i) {
    for (size_t j = 0; j < img.columns(); ++j) {
      std::cout << (img(i, j) == 1 ? 1U : 0U) << ' ';
    }
    std::cout << std::endl;
  }
}

}  // namespace util
}  // namespace balken


#endif
