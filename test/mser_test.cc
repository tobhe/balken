// cpp
#include <cstdint>

// external
#include <blaze/math/DynamicMatrix.h>
#include <gtest/gtest.h>

// own
#include "mser.h"
#include "mser_test.h"


using namespace balken;
using namespace blaze;

TEST_F(MSERTest, Construction) {
  auto img = blaze::DynamicMatrix<uint8_t>{{0, 117}, {56, 255}};
  auto res = mser::detect_regions(img);
}
