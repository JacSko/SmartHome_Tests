#include "gtest/gtest.h"
#include "TestCore.h"
#include "notification_types.h"
#include "fan_types.h"

/* ==================================================================================================================== */
/**
 * @file FanModuleTests.cpp
 *
 * @brief Tests performed on SmartHome binary to verify the common scenarios for Bathroom fan controller.
 *
 * @par
 * - Humidity_rised_above_threshold_than_dropped_below_threashold,
 * - Humidity_rised_but_do_not_crossed_threashold,
 * - Humidity_rised_fan_suspended_and_unsuspeneded,
 *
 * @author Jacek Skowronek
 * @date 08/03/2021
 */
/* ==================================================================================================================== */

struct FanModuleTestFixture : public testing::Test
{

   FanModuleTestFixture ()
   {
   }

   ~FanModuleTestFixture ()
   {
   }
   virtual void SetUp()
   {
      tc.runTest(::testing::UnitTest::GetInstance()->current_test_info()->name());
   }

   virtual void TearDown()
   {
      tc.stopTest();
   }

   TestCore tc;
};

TEST_F(FanModuleTestFixture, Humidity_rised_above_threshold_than_dropped_below_threashold)
{
   /**
    * <b>scenario</b>: Humidity in bathroom exceeds FAN_HUMIDITY_THRESHOLD threshold.<br>
    * <b>expected</b>: Fan started, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   ASSERT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD + 1);
   WAIT_S((ENV_MEASURE_PERIOD_DEF_MS/1000) * ENV_DEFAULT_SENSORS_COUNT + 10);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_ON));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_ON}));

   /**
    * <b>scenario</b>: Humidity in bathroom drops below FAN_HUMIDITY_THRESHOLD + FAN_THRESHOLD_HYSTERESIS threshold.<br>
    * <b>expected</b>: Fan stopped, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD - FAN_THRESHOLD_HYSTERESIS - 1);
   WAIT_S((ENV_MEASURE_PERIOD_DEF_MS/1000) * ENV_DEFAULT_SENSORS_COUNT + 10);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_OFF}));

}

TEST_F(FanModuleTestFixture, Humidity_rised_but_do_not_crossed_threashold)
{
   /**
    * <b>scenario</b>: Humidity in bathroom is just below FAN_HUMIDITY_THRESHOLD threshold.<br>
    * <b>expected</b>: Fan not started.<br>
    * ************************************************
    */
   ASSERT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD - 1);
   WAIT_S((ENV_MEASURE_PERIOD_DEF_MS/1000) * ENV_DEFAULT_SENSORS_COUNT + 10);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
}

TEST_F(FanModuleTestFixture, Humidity_rised_fan_suspended_and_unsuspeneded)
{
   /**
    * <b>scenario</b>: Humidity in bathroom is exceeds FAN_HUMIDITY_THRESHOLD threshold.<br>
    * <b>expected</b>: Fan started, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   ASSERT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD + FAN_THRESHOLD_HYSTERESIS + 1);
   WAIT_S((ENV_MEASURE_PERIOD_DEF_MS/1000) * ENV_DEFAULT_SENSORS_COUNT + 10);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_ON));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_ON}));

   /**
    * <b>scenario</b>: Humidity is above threshold for more than FAN_MAX_WORKING_TIME.<br>
    * <b>expected</b>: Fan stopped and suspended, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   WAIT_S(FAN_MAX_WORKING_TIME_S);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_SUSPEND}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_OFF}));

   /**
    * <b>scenario</b>: Humidity in bathroom drops below FAN_HUMIDITY_THRESHOLD + FAN_THRESHOLD_HYSTERESIS threshold.<br>
    * <b>expected</b>: Fan switched to OFF state, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD - FAN_THRESHOLD_HYSTERESIS - 1);
   WAIT_S((ENV_MEASURE_PERIOD_DEF_MS/1000) * ENV_DEFAULT_SENSORS_COUNT + 10);
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_OFF}));

   /**
    * <b>scenario</b>: Humidity in bathroom exceeds FAN_HUMIDITY_THRESHOLD threshold again (after suspend).<br>
    * <b>expected</b>: Fan started, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD + FAN_THRESHOLD_HYSTERESIS + 1);
   WAIT_S((ENV_MEASURE_PERIOD_DEF_MS/1000) * ENV_DEFAULT_SENSORS_COUNT + 10);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_ON));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_ON}));

   /**
    * <b>scenario</b>: Humidity in bathroom drops below FAN_HUMIDITY_THRESHOLD + FAN_THRESHOLD_HYSTERESIS threshold.<br>
    * <b>expected</b>: Fan stopped, notification to RaspberryApp sent.<br>
    * ************************************************
    */
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, FAN_HUMIDITY_THRESHOLD - FAN_THRESHOLD_HYSTERESIS - 1);
   WAIT_S(FAN_MIN_WORKING_TIME_S);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_OFF}));
}
