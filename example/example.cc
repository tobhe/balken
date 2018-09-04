#include <blaze/math/DynamicMatrix.h>
#include <iostream>

#include <string>
#include "datamatrix.h"
#include "util.h"

using namespace balken;

int main(int argc, char * argv[]) {
  std::string filename = argv[1];
  auto        img      = util::load_image(filename);

  auto bmp = datamatrix::from_image(img);
  util::print_image(bmp);

  auto sub =
    blaze::submatrix(bmp, 1UL, 1UL, bmp.rows() - 2, bmp.columns() - 2);

  std::vector<uint8_t> codes = datamatrix::decode(sub);
  for (auto i : codes) {
    std::cout << i - 1 << ": " << static_cast<char>(i - 1) << '\n';
  }
}
