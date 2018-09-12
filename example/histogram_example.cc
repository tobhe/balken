#include <blaze/math/DynamicMatrix.h>
#include <iostream>

#include <string>
#include "datamatrix.h"
#include "histogram.h"
#include "util.h"

using namespace balken;

int main(int, char * argv[]) {
  std::string filename = argv[1];
  auto        img      = util::load_image(filename);
  auto cpy = img;

  auto equal = histogram::stretch(img);
  util::compare(cpy, equal);

  return 0;
}

