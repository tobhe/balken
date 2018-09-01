#ifndef BLAKEN__DATAMATRIX_TEST_H__INCLUDED
#define BALKEN__DATAMATRIX_TEST_H__INCLUDED

#include <gtest/gtest.h>
#include "TestBase.h"

class DatamatrixTest : public ::testing::Test {

 public:

  DatamatrixTest() {
    LOG_MESSAGE("Opening test suite: DatamatrixTest");
  }

  virtual ~DatamatrixTest() {
    LOG_MESSAGE("Closing test suite: DatamatrixTest");
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

};

#endif // BALKEN__DATAMATRIX_TEST_H__INCLUDED
