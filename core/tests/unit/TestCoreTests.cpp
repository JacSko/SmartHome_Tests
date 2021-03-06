#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "TestCore.h"
/* ============================= */
/**
 * @file TestCoreTests.cpp
 *
 * @brief Unit tests to verify behavior of Test Core framework,
 *
 * @author Jacek Skowronek
 * @date 06/03/2021
 */
/* ============================= */

using namespace testing;

struct TestCoreFixture : public testing::Test
{
   void SetUp()
   {
   }
   void TearDown()
   {
   }
   std::unique_ptr<TestCore> m_test_subject;
};

/**
 * @test Tests of connection and disconnection
 */
TEST_F(TestCoreFixture, test)
{

}
