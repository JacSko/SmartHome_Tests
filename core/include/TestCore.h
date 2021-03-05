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
   DHT_SENSOR_ID id;
   DHT_SENSOR_TYPE type;
   int8_t temp_h;
   int8_t temp_l;
   uint8_t hum_h;
   uint8_t hum_l;
} DHT_Device;

struct Relays_board : public I2C_Board
{
   Relays_board(uint8_t addr):
   i2c_address(addr),
   state(0xFFFF),
   buffering_enabled(false)
   {
   }
};


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

private:
   std::vector<DHT_Device> m_dht_sensors;
   I2C_Board m_rel_board;
   I2C_Board m_inp_board;
   I2C_Board m_led_board;


};



#endif
