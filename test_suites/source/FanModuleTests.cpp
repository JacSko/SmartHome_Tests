#include "gtest/gtest.h"
#include "TestCore.h"
#include "notification_types.h"
#include "fan_types.h"

#define TEST_NAME "FanModuleTests"

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
      tc.runTest(TEST_NAME);
   }

   virtual void TearDown()
   {
      tc.stopTest();
   }

   TestCore tc;
};


TEST_F(FanModuleTestFixture, Fan_enabled_when_humidity_rised_above_threshold)
{
   ASSERT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, 95);
   WAIT_S(40);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_ON));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_ON}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_ON}));

   tc.setSensorState(DHT_SENSOR2, DHT_TYPE_DHT11, 24, 60);
   WAIT_S(40);
   EXPECT_TRUE(tc.checkRelayState(RELAY_BATHROOM_FAN, RELAY_STATE_OFF));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_FAN_STATE, {NTF_FAN_STATE, NTF_NTF, 1, FAN_STATE_OFF}));
   EXPECT_TRUE(tc.wasAppNtfSent(NTF_RELAYS_STATE, {NTF_RELAYS_STATE, NTF_NTF, 2, 11, RELAY_STATE_OFF}));

}
