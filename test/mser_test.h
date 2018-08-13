#ifndef BLAKEN__MSER_TEST_H__INCLUDED
#define BALKEN__MSER_TEST_H__INCLUDED

#include <gtest/gtest.h>
#include "TestBase.h"

class MSERTest : public ::testing::Test {

public:

  MSERTest() {
    LOG_MESSAGE("Opening test suite: MSERTest");
  }

  virtual ~MSERTest() {
    LOG_MESSAGE("Closing test suite: MSERTest");
  }

  virtual void SetUp() {
  }

  virtual void TearDown() {
  }

};

#endif // BALKEN__MSER_TEST_H__INCLUDED
