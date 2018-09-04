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
  auto mat = blaze::DynamicMatrix<uint8_t>(image._height, image._width);
  switch (image.depth()) {
    case 3:
      cimg_forXY(image, x, y) {
        auto R = static_cast<int>(image(x, y, 0, 0));
        auto G = static_cast<int>(image(x, y, 0, 1));
        auto B = static_cast<int>(image(x, y, 0, 2));

        // Save grey value weighted by perceived brightness
        mat(y, x) = static_cast<int>(0.299 * R + 0.587 * G + 0.114 * B);
      }
      return mat;
      break;
    case 1:
      cimg_forXY(image, x, y) {
        // Save grey value weighted by perceived brightness
        mat(y, x) = image(x, y, 0, 0) ? 0UL : 1UL;
      }
      return mat;
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
template <class MatrixT>
void view_image(MatrixT & img) {
  cimg_library::CImg<uint8_t> cimg(img.columns(), img.rows(), 1, 1, true);
  for (size_t x = 0; x < img.columns(); ++x) {
    for (size_t y = 0; y < img.rows(); ++y) { cimg(x, y, 0, 0) = img(y, x); }
  }
  cimg.display("Image");
}

inline void print_image(blaze::DynamicMatrix<uint8_t> & img) {
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
