#include "gtest/gtest.h"
#include "TestCore.h"
#include "notification_types.h"

/* ==================================================================================================================== */
/**
 * @file SlmModuleTests.cpp
 *
 * @brief Tests performed on SmartHome binary to verify the common scenarios for Stairs led lights controller.
 *
 * @tests
 *
 * @author Jacek Skowronek
 * @date 08/03/2021
 */
/* ==================================================================================================================== */

struct SlmModuleTestFixture : public testing::Test
{

   SlmModuleTestFixture ()
   {
   }

   ~SlmModuleTestFixture ()
   {
   }
   virtual void SetUp()
   {
      tc.runTest(::testing::UnitTest::GetInstance()->current_test_info()->name());
      tc.clearI2CBuffer(SLM_I2C_ADDRESS);
      tc.startI2CBuffering(SLM_I2C_ADDRESS);
   }

   virtual void TearDown()
   {
      tc.stopI2CBuffering(SLM_I2C_ADDRESS);
      tc.stopTest();
   }

   TestCore tc;
};

TEST_F(SlmModuleTestFixture, LedProgram_started_and_completed_without_interruption)
{
   /**
    * <b>scenario</b>: Led progrm started due to sensor activation.<br>
    * <b>expected</b>: Correct I2C data sequence sent, RaspberryApp notifications sent.<br>
    * ************************************************
    */
   WAIT_S(5);
   ASSERT_TRUE(tc.checkInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   WAIT_S(1);
   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);
   EXPECT_TRUE(tc.checkRelayState(RELAY_STAIRCASE_LED, RELAY_STATE_ON));

   WAIT_S(30);
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 0, 0x0001));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 1, 0x0003));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 2, 0x0007));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 3, 0x000F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 4, 0x001F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 5, 0x003F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 6, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 7, 0x00FF));

   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 8, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 9, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 10, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 11, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 12, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 13, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 14, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 15, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 16, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 17, 0x00FF));

   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 18, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 19, 0x003F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 20, 0x001F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 21, 0x000F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 22, 0x0007));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 23, 0x0003));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 24, 0x0001));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 25, 0x0000));
   EXPECT_TRUE(tc.checkRelayState(RELAY_STAIRCASE_LED, RELAY_STATE_OFF));

   WAIT_S(5);
   ASSERT_TRUE(tc.checkInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   WAIT_S(1);
   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);
   EXPECT_TRUE(tc.checkRelayState(RELAY_STAIRCASE_LED, RELAY_STATE_ON));

   WAIT_S(30);
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 0, 0x0001));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 1, 0x0003));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 2, 0x0007));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 3, 0x000F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 4, 0x001F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 5, 0x003F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 6, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 7, 0x00FF));

   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 8, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 9, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 10, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 11, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 12, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 13, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 14, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 15, 0x00FF));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 16, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 17, 0x00FF));

   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 18, 0x007F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 19, 0x003F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 20, 0x001F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 21, 0x000F));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 22, 0x0007));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 23, 0x0003));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 24, 0x0001));
   EXPECT_TRUE(tc.checkI2CBufferElement(SLM_I2C_ADDRESS, 25, 0x0000));
   EXPECT_TRUE(tc.checkRelayState(RELAY_STAIRCASE_LED, RELAY_STATE_OFF));

}
