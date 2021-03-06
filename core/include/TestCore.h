#ifndef _TESTCORE_H_
#define _TESTCORE_H_

/* ============================= */
/**
 * @file TestCore.h
 *
 * @brief Core of test framework that allows to communicate with tested application.
 *
 * @details
 *    TestCore opens 3 tcp servers (where tested app is connecting), and starts the communication.
 *
 * @author Jacek Skowronek
 * @date 05/03/2021
 */
/* ============================= */

/* =============================
 *  Includes of common headers
 * =============================*/
#include <string>
#include <vector>
#include <string>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "relays_types.h"
#include "inputs_types.h"
#include "env_types.h"
#include "system_config_values.h"
#include "notification_types.h"
/* =============================
 *          Defines
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/
typedef struct
{
   uint8_t i2c_address;
   uint16_t state;
   std::vector<std::string> buffer;
   bool buffering_enabled;
} I2C_Board;

typedef struct
{
   NTF_CMD_ID id;
   std::vector<uint8_t> payload;
} APP_NTF;

typedef struct
{
   DHT_SENSOR_ID id;
   DHT_SENSOR_TYPE type;
   int8_t temp_h;
   int8_t temp_l;
   uint8_t hum_h;
   uint8_t hum_l;
} DHT_Device;


class TestCore
{
public:
   TestCore();
   bool runTest(const std::string& test_name);
   bool stopTest();

   void setState(RELAY_ID id, RELAY_STATE state);
   void setState(INPUT_ID id, INPUT_STATE state);
   void setState(DHT_SENSOR_ID id, const std::string& temp, const std::string& hum);
   RELAY_STATE getState(RELAY_ID);
   INPUT_STATE getState(INPUT_ID);

   void startI2CBuffering(uint8_t address);
   void stopI2CBuffering(uint8_t address);
   void clearI2CBuffer(uint8_t address);

   bool checkI2CBufferSize(size_t size);
   bool checkI2CBufferElement(uint16_t idx, std::vector<uint8_t>& exp);
   bool checkLastI2CNotification(uint8_t address, std::vector<uint8_t>& msg);

   void startAppDataBuffering();
   void stopAppDataBuffering();
   void clearAppDataBuffering();

   bool wasAppNtfSent(NTF_CMD_ID id, std::vector<uint8_t>& msg);

private:
   std::vector<APP_NTF> m_app_ntf;
};



#endif
