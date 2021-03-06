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
#include <map>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "relays_types.h"
#include "inputs_types.h"
#include "env_types.h"
#include "system_config_values.h"
#include "notification_types.h"
#include "SocketDriver.h"
#include "TestSubjectExecutor.h"
/* =============================
 *          Defines
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/
typedef struct
{
   uint8_t i2c_address;
   uint16_t state = 0xFFFF;
   std::vector<uint16_t> buffer;
   bool buffering_enabled = false;
} I2C_Board;

typedef struct
{
   NTF_CMD_ID id;
   std::vector<uint8_t> payload;
} APP_NTF;

typedef struct
{
   DHT_SENSOR_ID id;
   DHT_SENSOR_TYPE type = DHT_TYPE_DHT11;
   int8_t temp_h = 0;
   int8_t temp_l = 0;
   uint8_t hum_h = 0;
   uint8_t hum_l = 0;
} DHT_Device;

typedef enum
{
/* Below enumerations are cloned in test framework, any change here must lead to change in both places*/
//TODO: move to common space
   I2C_STATE_SET = 1,       /*< Sets current state of I2C board - in raw format, e.g. 0xFFFF */
   I2C_STATE_NTF = 2,       /*< Event sent to test framework to notify that new data was written to I2C device */
   DHT_STATE_SET = 3,       /*< Sets current state of DHT sensor */
   I2C_INT_TRIGGER = 4,     /*< Event to simulate I2C interrupt */
   HW_STUB_EV_ENUM_COUNT,
} HW_STUB_EVENT_ID;


class TestCore
{
public:
   TestCore();
   bool runTest(const std::string& test_name);
   void stopTest();

   void setState(RELAY_ID id, RELAY_STATE state);
   void setState(INPUT_ID id, INPUT_STATE state);
   void setState(DHT_SENSOR_ID id, DHT_SENSOR_TYPE type, int8_t temp, int8_t hum);
   void triggerInterrupt();
   RELAY_STATE getState(RELAY_ID);
   INPUT_STATE getState(INPUT_ID);

   void startI2CBuffering(uint8_t address);
   void stopI2CBuffering(uint8_t address);
   void clearI2CBuffer(uint8_t address);

   bool checkI2CBufferSize(uint8_t address, size_t size);
   bool checkI2CBufferElement(uint8_t address, uint16_t idx, uint16_t exp);

   void startAppDataBuffering();
   void stopAppDataBuffering();
   void clearAppDataBuffer();

   bool wasAppNtfSent(NTF_CMD_ID id, std::vector<uint8_t>& msg);

private:

   void onStubEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t count);
   void onBluetoothEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t count);
   void onAppEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t count);
   bool decodeBytesFromString(const std::vector<uint8_t>& data, size_t size);
   bool sendToHwStub(const std::vector<uint8_t>& data);


   pid_t m_test_bin_pid;
   uint8_t rel_id_to_relay_no(RELAY_ID id);
   uint16_t rel_relay_to_mask(uint8_t relay_no);
   uint16_t rel_id_to_mask(RELAY_ID id);
   RELAY_ID m_rel_id_match [RELAYS_RELAY_COUNT + 1] = RELAYS_MATCH;

   uint16_t inp_input_to_mask(uint8_t input);
   uint8_t inp_id_to_input_no(INPUT_ID id);
   uint16_t inp_id_to_mask(INPUT_ID id);
   INPUT_ID m_inp_id_match [INPUTS_INPUT_COUNT + 1] = INPUTS_MATCH;

   std::vector<APP_NTF> m_app_ntfs;
   std::map<uint8_t, I2C_Board> m_i2c_map;

   SocketDriver m_hwstub_driver;
   SocketDriver m_bluetooth_driver;
   SocketDriver m_app_ntf_driver;
   TestSubjectExecutor m_bin_exec;
   std::vector<uint8_t> m_buffer;
   std::mutex m_buf_mtx;
   std::vector<uint8_t> m_send_buf;
   std::mutex m_send_buf_mtx;
};



#endif
