#include "gtest/gtest.h"
#include "TestCore.h"
#include "notification_types.h"
#include "stairs_led_types.h"

/* ==================================================================================================================== */
/**
 * @file SlmModuleTests.cpp
 *
 * @brief Tests performed on SmartHome binary to verify the common scenarios for Stairs led lights controller.
 *
 * @tests
 * - LedProgram_started_and_completed_without_interruption
 * - LedProgram_started_sensor_active_when_module_starts_disabling
 * - LedProgram_started_sensor_reactivated_when_off_effect_running
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
      tc.clearAppDataBuffer();
   }

   virtual void TearDown()
   {
      WAIT_S(3);
      tc.stopTest();
   }

   TestCore tc;
};

TEST_F(SlmModuleTestFixture, LedProgram_started_and_completed_without_interruption)
{
   /**
    * <b>scenario</b>: Led program started due to sensor activation, no sensor reactivation during program duration.<br>
    * <b>expected</b>: Correct I2C data sequence sent, RaspberryApp notifications sent.<br>
    * ************************************************
    */
   ASSERT_TRUE(tc.checkInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ON}));

   WAIT_S(19)

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 5000));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT_READY}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0000, 500));

   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF}));

   /**
    * <b>scenario</b>: Led program started due to sensor activation again.<br>
    * <b>expected</b>: Correct I2C data sequence sent, RaspberryApp notifications sent.<br>
    * ************************************************
    */
   WAIT_S(5);
   ASSERT_TRUE(tc.checkInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ON}));

   WAIT_S(19)

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 5000));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT_READY}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0000, 500));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF}));

}

TEST_F(SlmModuleTestFixture, LedProgram_started_sensor_active_when_module_starts_disabling)
{
   /**
    * <b>scenario</b>: Led program started due to sensor activation, sensor is still active when module is starting to disable leds.<br>
    * <b>expected</b>: Correct I2C data sequence sent, RaspberryApp notifications sent, leds ON timer is restarted.<br>
    * ************************************************
    */
   ASSERT_TRUE(tc.checkInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));

   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ON}));

   WAIT_S(30)
   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);
   WAIT_S(9);

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 5000));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT_READY}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0000, 500));

   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF}));

}

TEST_F(SlmModuleTestFixture, LedProgram_started_sensor_reactivated_when_off_effect_running)
{
   /**
    * <b>scenario</b>: Led program started due to sensor activation, sensor reactivated when OFF offect is running.<br>
    * <b>expected</b>: Correct I2C data sequence sent, RaspberryApp notifications sent, OFF effect stopped and all LEDs are enabled again.<br>
    * ************************************************
    */
   ASSERT_TRUE(tc.checkInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));

   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ON}));
   tc.clearAppDataBuffer();
   WAIT_S(18)

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 5000));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));

   /* interrupt trigger when OFF effect is ongoing */
   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_ACTIVE);
   tc.triggerInterrupt();

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   WAIT_S(1);
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ON}));

   WAIT_S(18)
   tc.setInputState(INPUT_STAIRS_SENSOR, INPUT_STATE_INACTIVE);

   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 5000));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF_EFFECT_READY}));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x00FF, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x007F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x001F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x000F, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0007, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0003, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0001, 500));
   EXPECT_TRUE(tc.waitForI2CNotification(SLM_I2C_ADDRESS, 0x0000, 500));

   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_ONGOING_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_SLM_STATE, {NTF_SLM_STATE, NTF_NTF, 1, SLM_STATE_OFF}));

}
