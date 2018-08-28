#include <blaze/math/DynamicMatrix.h>
#include <iostream>

#include "mser.h"
#include "util.h"

using namespace blaze;
using namespace balken;

int main() {
  auto img = util::load_image("barcode.jpg");

  auto res = mser::detect_regions(img, 2, 0.0005, 0.1, 0.5, 0.5);
  util::view_image(img);
}
