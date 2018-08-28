#ifndef BALKEN__UTIL_H__INCLUDED
#define BALKEN__UTIL_H__INCLUDED

// external
#include <CImg.h>
#include <blaze/math/DynamicMatrix.h>

namespace balken {
namespace util {

/**
 * \brief  Load image from file to greyscale image as Matrix
 *
 * \param[in]  filename  Name of file to load
 * \return     Matrix containing image
 */
blaze::DynamicMatrix<uint8_t> load_image(std::string filename) {
  cimg_library::CImg<unsigned char> image(filename.c_str());

  auto mat = blaze::DynamicMatrix<uint8_t>(image._width, image._height);
  cimg_forXY(image, x, y) {
    auto R = static_cast<int>(image(x, y, 0, 0));
    auto G = static_cast<int>(image(x, y, 0, 1));
    auto B = static_cast<int>(image(x, y, 0, 2));

    // Save grey value weighted by perceived brightness
    mat(y, x) = static_cast<int>(0.299 * R + 0.587 * G + 0.114 * B);
  }
  return mat;
}

/**
 * \brief      View image from Matrix
 *
 * \param[in]  img  Matrix containing image
 */
void view_image(blaze::DynamicMatrix<uint8_t> & img) {
  cimg_library::CImg<uint8_t> cimg(img.columns(), img.rows(), 1, 1, true);
  for (size_t x = 0; x < img.columns(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) { cimg(x, y, 0, 0) = img(y, x); }
  }
  cimg.display("Image");
}

}  // namespace util
}  // namespace balken


#endif
