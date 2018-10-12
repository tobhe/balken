/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

// cpp
#include <iostream>
#include <string>

// thirdparty
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/Submatrix.h>

// own
#include "datamatrix.h"
#include "image/edt.h"
#include "image/filter.h"
#include "image/geometry.h"
#include "image/histogram.h"
#include "image/morph.h"
#include "region/draw.h"
#include "region/regions.h"
#include "util.h"

using namespace balken;

int main(int, char * argv[]) {
  std::string filename  = argv[1];
  int         threshold = atoi(argv[2]);
  int         SE1       = atoi(argv[3]);
  int         SE2       = atoi(argv[4]);
  int         SE3       = atoi(argv[5]);

  auto se1 = blaze::DynamicMatrix<uint8_t>(
    static_cast<size_t>(SE1), static_cast<size_t>(SE1), 1);
  auto se2 = blaze::DynamicMatrix<uint8_t>(
    static_cast<size_t>(SE2), static_cast<size_t>(SE2), 1);
  auto se3 = blaze::DynamicMatrix<uint8_t>(
    static_cast<size_t>(SE3), static_cast<size_t>(SE3), 1);

  auto img = util::load_image(filename);
  auto cpy = img;
  util::view_image(cpy);

  auto closed = morph::close(histogram::views::stretch(img), se1);
  util::view_image(closed);
  decltype(img) bottom_hat = closed - img;
  util::view_image(bottom_hat);
  auto binarized = filter::views::binarize(bottom_hat, threshold);
  util::view_image(binarized);
  auto dilated    = morph::views::dilate(binarized, se2);
  auto region_vec = regions::find(dilated);
  std::cout << "Size before: " << region_vec.size() << '\n';
  regions::filter(img.rows() * img.columns(), region_vec);
  std::cout << "Size after: " << region_vec.size() << '\n';
  util::view_image(dilated);

  auto region = region_vec[1];
  auto hull_img =
    blaze::DynamicMatrix<uint8_t>(dilated.rows(), dilated.columns(), 0);

  auto hull = regions::convex_hull(region);
  draw::draw_shape(hull_img, hull, 255UL);
  auto bounding_box = regions::bounding_box(hull);
  util::view_image(hull_img);

  auto sub = blaze::submatrix(img,
                              bounding_box[0].i,
                              bounding_box[0].j,
                              bounding_box[3].i - bounding_box[0].i,
                              bounding_box[1].j - bounding_box[0].j);

  auto matrix = datamatrix::code(
    filter::views::binarize(histogram::views::stretch(sub), 150));
  util::compare(filter::views::binarize(histogram::views::stretch(sub), 150),
                geometry::scale(matrix, 5));

  return 0;
}
