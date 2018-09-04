// cpp
#include <cstdint>
#include <iostream>

// external
#include <blaze/math/DynamicMatrix.h>
#include <blaze/math/Submatrix.h>
#include <gtest/gtest.h>

// own
#include "datamatrix.h"
#include "datamatrix_test.h"

using namespace balken;

TEST_F(DatamatrixTest, find) {
  auto img = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{{0, 1}, {0, 0}};
  auto top_left     = datamatrix::detail::find_top_left_black(img);      // < (1, 0)
  auto bottom_right = datamatrix::detail::find_bottom_right_black(img);  // < (1, 0)
  ASSERT_EQ(top_left.first, 1);
  ASSERT_EQ(top_left.second, 0);
  ASSERT_EQ(bottom_right.first, 1);
  ASSERT_EQ(bottom_right.second, 0);

  img      = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{{1, 1}, {0, 0}};
  top_left = datamatrix::detail::find_top_left_black(img);          // < (0, 0)
  bottom_right = datamatrix::detail::find_bottom_right_black(img);  // < (1, 0)
  ASSERT_EQ(top_left.first, 0);
  ASSERT_EQ(top_left.second, 0);
  ASSERT_EQ(bottom_right.first, 1);
  ASSERT_EQ(bottom_right.second, 0);

  img      = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{{0, 0}, {0, 1}};
  top_left = datamatrix::detail::find_top_left_black(img);          // < (1, 1)
  bottom_right = datamatrix::detail::find_bottom_right_black(img);  // < (1, 1)
  ASSERT_EQ(top_left.first, 1);
  ASSERT_EQ(top_left.second, 1);
  ASSERT_EQ(bottom_right.first, 1);
  ASSERT_EQ(bottom_right.second, 1);

  img = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};
  top_left     = datamatrix::detail::find_top_left_black(img);      // < (1, 1)
  bottom_right = datamatrix::detail::find_bottom_right_black(img);  // < (1, 1)
  ASSERT_EQ(top_left.first, 1);
  ASSERT_EQ(top_left.second, 1);
  ASSERT_EQ(bottom_right.first, 8);
  ASSERT_EQ(bottom_right.second, 8);
}

TEST_F(DatamatrixTest, module_size) {
  /* img:
   * 1 1 0 0
   * 1 1 0 0
   * 1 1 1 1
   * 1 1 1 1
   */
  auto img = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{
    {1, 1, 0, 0}, {1, 1, 0, 0}, {1, 1, 1, 1}, {1, 1, 1, 1}};
  auto pair = datamatrix::detail::find_top_left_black(img);  // < (0,0)
  ASSERT_EQ(pair.first, 0);
  ASSERT_EQ(pair.second, 0);

  auto size = datamatrix::detail::module_size(pair, img);  // < 2
  ASSERT_EQ(size, 2);

  /* img2:
   * 1 1 1 1
   * 1 1 0 0
   * 1 1 1 1
   * 1 1 1 1
   */
  auto img2 = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{
    {1, 1, 1, 1}, {1, 1, 0, 0}, {1, 1, 1, 1}, {1, 1, 1, 1}};
  auto pair2 = datamatrix::detail::find_top_left_black(img2);  // < (0,0)
  ASSERT_EQ(pair2.first, 0);
  ASSERT_EQ(pair2.second, 0);

  auto size2 = datamatrix::detail::module_size(pair2, img2);  // < -1
  ASSERT_EQ(size2, -1);
}

TEST_F(DatamatrixTest, find_inner) {
  // Test basic 4x4 matrix
  auto img = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};


  auto inner_mat = datamatrix::from_image(img);
  ASSERT_EQ(inner_mat.rows(), 4);
  ASSERT_EQ(inner_mat.columns(), 4);

  ASSERT_EQ(inner_mat(0, 0), 1);
  ASSERT_EQ(inner_mat(0, 1), 0);
  ASSERT_EQ(inner_mat(1, 0), 1);
  ASSERT_EQ(inner_mat(1, 1), 1);

  // They can actually be rectangular
  img = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1},
    {0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};


  inner_mat = datamatrix::from_image(img);
  ASSERT_EQ(inner_mat.rows(), 4);
  ASSERT_EQ(inner_mat.columns(), 6);

  ASSERT_EQ(inner_mat(0, 0), 1);
  ASSERT_EQ(inner_mat(0, 1), 0);
  ASSERT_EQ(inner_mat(1, 5), 1);
  ASSERT_EQ(inner_mat(1, 4), 0);
}

TEST_F(DatamatrixTest, decode) {
  // Test basic 4x4 matrix
  auto img = blaze::DynamicMatrix<uint8_t, blaze::rowMajor>{
    {1, 0, 1, 0, 1, 0, 1, 0, 1, 0, 1, 0},
    {1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };

  auto sub =
    blaze::submatrix(img, 1UL, 1UL, img.rows() - 2, img.columns() - 2);

  ASSERT_EQ(255, datamatrix::detail::decode_codeword(std::make_pair(0, 4), sub));
  ASSERT_EQ(0, datamatrix::detail::decode_codeword(std::make_pair(2, 2), sub));
}
