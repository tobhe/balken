/*
 * Copyright (C) 2018 Tobias Heider <heidert@nm.ifi.lmu.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v3 See the file LICENSE in the top level
 * directory for more details.
 */

#include <blaze/math/DynamicMatrix.h>
#include <iostream>

#include <string>
#include "datamatrix.h"
#include "draw.h"
#include "edt.h"
#include "filter.h"
#include "histogram.h"
#include "morph.h"
#include "regions.h"
#include "util.h"

using namespace balken;

template <class RegionsT>
decltype(auto) filter(RegionsT && regions);

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

  auto          gaussed    = filter::gauss(img);
  auto          closed     = filter::close(gaussed, se1);
  decltype(img) bottom_hat = closed - img;
  auto          binarized  = filter::binarize(bottom_hat, threshold);
  auto          dilated    = filter::dilate(binarized, se2);
  auto          region_vec = regions::find(dilated);
  std::cout << "Size before: " << region_vec.size() << '\n';
  auto filtered_regions = regions::filter(region_vec);
  std::cout << "Size after: " << filtered_regions.size() << '\n';

  auto hull_img =
    blaze::DynamicMatrix<uint8_t>(dilated.rows(), dilated.columns(), 0);

  for (auto & region : filtered_regions) {
    auto hull = regions::convex_hull(region);
    draw::draw_shape(hull_img, hull, 255UL);
  }

  util::compare(histogram::make_normalized(cpy), hull_img);
}

