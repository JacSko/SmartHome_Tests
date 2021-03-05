#include "TestCore.h"
#include "system_config_values.h"


TestCore::TestCore()
{

}
bool TestCore::runTest(const std::string& test_name)
{

}
bool TestCore::stopTest()
{

}
void TestCore::setState(RELAY_ID id, RELAY_STATE state)
{

}
void TestCore::setState(INPUT_ID id, INPUT_STATE state)
{

}
void TestCore::setState(DHT_SENSOR_ID id, const std::string& temp, const std::string& hum)
{

}
RELAY_STATE TestCore::getState(RELAY_ID)
{

}
INPUT_STATE TestCore::getState(INPUT_ID)
{

}
