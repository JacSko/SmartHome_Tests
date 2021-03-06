/* =============================
 *   Includes of common headers
 * =============================*/
/* =============================
 *   Includes of project headers
 * =============================*/
#include "TestCore.h"
#include "SocketDriver.h"

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
void startI2CBuffering(uint8_t address)
{

}
void stopI2CBuffering(uint8_t address)
{

}
void clearI2CBuffer(uint8_t address)
{

}

bool checkI2CBufferSize(size_t size)
{

}
bool checkI2CBufferElement(uint16_t idx, std::vector<uint8_t>& exp)
{

}
bool checkLastI2CNotification(uint8_t address, std::vector<uint8_t>& msg)
{

}
void startAppDataBuffering()
{

}
void stopAppDataBuffering()
{

}
void clearAppDataBuffering()
{

}
bool wasAppNtfSent(NTF_CMD_ID id, std::vector<uint8_t>& msg)
{

}
