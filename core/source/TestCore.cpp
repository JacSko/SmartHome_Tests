/* =============================
 *   Includes of common headers
 * =============================*/
/* =============================
 *   Includes of project headers
 * =============================*/
#include "TestCore.h"
#include "SocketDriver.h"
#include "Logger.h"

TestCore::TestCore():
m_bin_exec(TEST_BINARY_ABSOLUTE_PATH),
m_test_bin_pid(0)
{
   m_i2c_map[RELAYS_I2C_ADDRESS].i2c_address = RELAYS_I2C_ADDRESS;
   m_i2c_map[INPUTS_I2C_ADDRESS].i2c_address = INPUTS_I2C_ADDRESS;
   m_i2c_map[SLM_I2C_ADDRESS].i2c_address = SLM_I2C_ADDRESS;

}
bool TestCore::runTest(const std::string& test_name)
{
   bool result = false;
   if (!logger_initialize(test_name))
   {
      logger_send(TF_ERROR, __func__, "Cannot create logfile - there are only console logs available");
   }

   logger_send(TF_TEST_MARKER, "TEST_BEGIN", "%s", test_name.c_str());

   m_hwstub_driver.addListener([&](DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
                                 {
                                    this->onStubEvent(ev, data, size);
                                 });
   m_bluetooth_driver.addListener([&](DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
                                 {
                                    this->onBluetoothEvent(ev, data, size);
                                 });
   m_app_ntf_driver.addListener([&](DriverEvent ev, const std::vector<uint8_t>& data, size_t size)
                                 {
                                    this->onAppEvent(ev, data, size);
                                 });

   /* run testes binary */
   m_test_bin_pid = m_bin_exec.start_test_subject();

   m_hwstub_driver.connect("127.0.0.1", HW_STUB_CONTROL_PORT);
   m_bluetooth_driver.connect("127.0.0.1", BLUETOOTH_FORWARDING_PORT);
   m_app_ntf_driver.connect("127.0.0.1", WIFI_NTF_FORWARDING_PORT);

   auto time = std::chrono::system_clock::now();
   while ( (std::chrono::system_clock::now() - time) < std::chrono::seconds(SOCK_CLIENT_WAIT_TMOUT_S))
   {
      if (m_hwstub_driver.isConnected() &&
          m_bluetooth_driver.isConnected() &&
          m_app_ntf_driver.isConnected())
      {
         result = true;
         break;
      }
      else
      {
         std::this_thread::sleep_for(std::chrono::milliseconds(10));
      }
   }
   logger_send_if(!result, TF_ERROR, __func__, "init error, conn status: STUB:%u BT:%u APP:%u", m_hwstub_driver.isConnected(),
                                                                                                m_bluetooth_driver.isConnected(),
                                                                                                m_app_ntf_driver.isConnected());
   WAIT_S(5); /* test binary need to wakeup */
   return result;
}
void TestCore::stopTest()
{
   logger_send(TF_TEST_MARKER, "TEST_END", "");
   logger_deinitialize();
   m_hwstub_driver.removeListener();
   m_bluetooth_driver.removeListener();
   m_app_ntf_driver.removeListener();

   m_bin_exec.stop_test_subject(m_test_bin_pid);

   m_hwstub_driver.disconnect();
   m_bluetooth_driver.disconnect();
   m_app_ntf_driver.disconnect();

}
void TestCore::onStubEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t count)
{
   if (ev == DriverEvent::DRIVER_DATA_RECV)
   {
      logger_send(STM_HW_STUB, __func__, "%s", data.data());
      std::lock_guard<std::mutex> lock(m_buf_mtx);
      if (decodeBytesFromString(data, count))
      {
         if (m_buffer[1] == m_buffer.size() - 2)
         {
            switch((HW_STUB_EVENT_ID)m_buffer[0])
            {
            case I2C_STATE_NTF:
            {
               uint16_t state = m_buffer[4] << 8;
               state |= (m_buffer[3] & 0x00FF);
               m_i2c_map[m_buffer[2]].state = state;
               if (m_i2c_map[m_buffer[2]].buffering_enabled)
               {
                  m_i2c_map[m_buffer[2]].buffer.push_back(state);
               }
               logger_send(TF_TC, __func__, "got i2c data addr %x, state %.4x", m_buffer[2], state);
            }
            break;
            default:
               break;
            }
         }
         else
         {
            logger_send(TF_TC, __func__, "incomplete data");
         }
      }
      else
      {
         logger_send(TF_TC, __func__, "cannot decode data", data.data());
      }
   }
}
void TestCore::onBluetoothEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t count)
{
   if (ev == DriverEvent::DRIVER_DATA_RECV)
   {
      logger_send(STM_BLUETOOTH, __func__, "%s", data.data());
   }
}
void TestCore::onAppEvent(DriverEvent ev, const std::vector<uint8_t>& data, size_t count)
{
   if (ev == DriverEvent::DRIVER_DATA_RECV)
   {
      logger_send(STM_WIFI_NTF, __func__, "%s", data.data());
      if (data.size() >= NTF_HEADER_SIZE)
      {
         std::lock_guard<std::mutex> lock(m_buf_mtx);
         if (decodeBytesFromString(data, count))
         {
            APP_NTF ntf;
            ntf.id = (NTF_CMD_ID) m_buffer[0];
            ntf.payload.insert(ntf.payload.begin(), m_buffer.begin(), m_buffer.end());
            m_app_ntfs.push_back(ntf);
         }
      }
   }
}
bool TestCore::decodeBytesFromString(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   char byte [4];
   uint8_t byte_idx = 0;
   m_buffer.clear();
   if (data.size() > 0)
   {
      for (uint8_t i = 0; i < size; i++)
      {
         if (data[i] != ' ')
         {
            byte[byte_idx++] = (char)data[i];
         }
         else
         {
            byte[byte_idx] = 0x00;
            m_buffer.push_back(atoi(byte));
            byte_idx = 0;
         }
      }
      byte[byte_idx] = 0x00;
      m_buffer.push_back(atoi(byte));
      result = true;
   }
   logger_send(TF_TC, __func__, "decoded %u bytes", m_buffer.size());
   return result;
}
bool TestCore::setRelayState(RELAY_ID id, RELAY_STATE state)
{
   std::vector<uint8_t> cmd;
   bool result = true;
   if (state == RELAY_STATE_ON)
   {
      m_i2c_map[RELAYS_I2C_ADDRESS].state &= ~rel_id_to_mask(id);
   }
   else
   {
      m_i2c_map[RELAYS_I2C_ADDRESS].state |= rel_id_to_mask(id);
   }

   cmd.push_back(I2C_STATE_SET);
   cmd.push_back(0x03);
   cmd.push_back(RELAYS_I2C_ADDRESS);
   cmd.push_back(m_i2c_map[RELAYS_I2C_ADDRESS].state & 0xFF);
   cmd.push_back((m_i2c_map[RELAYS_I2C_ADDRESS].state >>8) & 0xFF);

   if (!sendToHwStub(cmd))
   {
      result = false;
      logger_send(TF_ERROR, __func__, "cannot write relays state to hw stub");
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s:%u %u => %u", __func__, id, state, result);
   return result;
}
uint16_t TestCore::rel_id_to_mask(RELAY_ID id)
{
   return rel_relay_to_mask(rel_id_to_relay_no(id));
}

uint8_t TestCore::rel_id_to_relay_no(RELAY_ID id)
{
   uint8_t result = 0;
   for (uint8_t i = 0; i < (sizeof(m_rel_id_match) / sizeof(m_rel_id_match[0])); i++)
   {
      if (m_rel_id_match[i] == id)
      {
         result = i;
         break;
      }
   }
   return result;
}
uint16_t TestCore::rel_relay_to_mask(uint8_t relay_no)
{
   return (uint16_t)(1 << (relay_no - 1));
}
bool TestCore::sendToHwStub(const std::vector<uint8_t>& data)
{
   bool result = false;
   char bytes_formatted [1024];
   size_t byte_idx = 0;

   for (auto i : data)
   {
      byte_idx += snprintf(bytes_formatted + byte_idx, 1024, "%d ", i);
   }
   bytes_formatted[--byte_idx] = 0x00;

   std::lock_guard<std::mutex> lock(m_send_buf_mtx);
   m_send_buf.clear();
   m_send_buf.insert(m_send_buf.begin(), bytes_formatted, bytes_formatted + byte_idx);
   result = m_hwstub_driver.write(m_send_buf, m_send_buf.size());
   return result;
}

uint16_t TestCore::inp_id_to_mask(INPUT_ID id)
{
   return inp_input_to_mask(inp_id_to_input_no(id));
}

uint8_t TestCore::inp_id_to_input_no(INPUT_ID id)
{
   uint8_t result = 0;
   for (uint8_t i =0 ; i < INPUTS_INPUT_COUNT + 1; i++)
   {
      if (m_inp_id_match[i] == id)
      {
         result = i;
         break;
      }
   }
   return result;
}

uint16_t TestCore::inp_input_to_mask(uint8_t input)
{
   uint16_t result = 0;

   if (input < 9){
       result = 1 << (16 - input);
   }
   else
   {
       result = 1 << (input-9);
   }

   return result;
}
bool TestCore::setInputState(INPUT_ID id, INPUT_STATE state)
{
   bool result = true;
   std::vector<uint8_t> cmd;
   if (state == INPUT_STATE_ACTIVE)
   {
      m_i2c_map[INPUTS_I2C_ADDRESS].state &= ~inp_id_to_mask(id);
   }
   else
   {
      m_i2c_map[INPUTS_I2C_ADDRESS].state |= inp_id_to_mask(id);
   }

   cmd.push_back(I2C_STATE_SET);
   cmd.push_back(0x03);
   cmd.push_back(INPUTS_I2C_ADDRESS);
   cmd.push_back(m_i2c_map[INPUTS_I2C_ADDRESS].state & 0xFF);
   cmd.push_back((m_i2c_map[INPUTS_I2C_ADDRESS].state >>8) & 0xFF);

   if (!sendToHwStub(cmd))
   {
      result = false;
      logger_send(TF_ERROR, __func__, "cannot write inputs state to hw stub");
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s:%u %u => %u", __func__, id, state, result);
   return result;
}
bool TestCore::setSensorState(DHT_SENSOR_ID id, DHT_SENSOR_TYPE type, int8_t temp, int8_t hum)
{
   bool result = true;
   std::vector<uint8_t> cmd;

   cmd.push_back(DHT_STATE_SET);
   cmd.push_back(0x06);
   cmd.push_back(id);
   cmd.push_back(type);
   cmd.push_back(temp);
   cmd.push_back(0x00);
   cmd.push_back(hum);
   cmd.push_back(0x00);

   if (!sendToHwStub(cmd))
   {
      result = false;
      logger_send(TF_ERROR, __func__, "cannot write DHT sensor data");
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s:%u %u %d %u => %u", __func__, id, type, temp, hum, result);
   return result;
}
bool TestCore::triggerInterrupt()
{
   bool result = true;
   std::vector<uint8_t> cmd;

   cmd.push_back(I2C_INT_TRIGGER);
   cmd.push_back(0x00);

   if (!sendToHwStub(cmd))
   {
      result = false;
      logger_send(TF_ERROR, __func__, "cannot trigger interrupt data");
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s => %u", __func__, result);
   return result;
}
bool TestCore::checkRelayState(RELAY_ID id, RELAY_STATE state)
{
   bool result = getRelayState(id) == state;
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s : %u %u => %u", __func__, id, state, result);
   return result;
}
bool TestCore::checkInputState(INPUT_ID id, INPUT_STATE state)
{
   bool result = getInputState(id) == state;
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s : %u %u => %u", __func__, id, state, result);
   return result;
}
RELAY_STATE TestCore::getRelayState(RELAY_ID id)
{
   RELAY_STATE state = m_i2c_map[RELAYS_I2C_ADDRESS].state & rel_id_to_mask(id)? RELAY_STATE_OFF : RELAY_STATE_ON;
   return state;
}
INPUT_STATE TestCore::getInputState(INPUT_ID id)
{
   INPUT_STATE state = m_i2c_map[INPUTS_I2C_ADDRESS].state & inp_id_to_mask(id)? INPUT_STATE_INACTIVE : INPUT_STATE_ACTIVE;
   return state;
}
void TestCore::startI2CBuffering(uint8_t address)
{
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s", __func__);
   m_i2c_map[address].buffering_enabled = true;
}
void TestCore::stopI2CBuffering(uint8_t address)
{
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s", __func__);
   m_i2c_map[address].buffering_enabled = false;
}
void TestCore::clearI2CBuffer(uint8_t address)
{
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s", __func__);
   m_i2c_map[address].buffer.clear();
}
bool TestCore::waitForI2CNotification(uint8_t address, uint16_t state, uint32_t timeout_ms)
{
   bool result = false;
   auto time = std::chrono::system_clock::now();
   while ( (std::chrono::system_clock::now() - time) < std::chrono::milliseconds(timeout_ms))
   {
      {
         std::lock_guard<std::mutex> lock(m_buf_mtx);
         if (m_i2c_map[address].state == state)
         {
            result = true;
            break;
         }
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s : %u %u %u => %d", __func__, address, state, timeout_ms, result);
   return result;
}
bool TestCore::checkI2CBufferSize(uint8_t address, size_t size)
{
   size_t buf_size = m_i2c_map[address].buffer.size();
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s => %d", __func__, buf_size);
   return (size == buf_size)? true : false;
}
bool TestCore::checkI2CBufferElement(uint8_t address, uint16_t idx, uint16_t exp)
{
   bool result = false;
   if (idx < m_i2c_map[address].buffer.size())
   {
      result = m_i2c_map[address].buffer[idx] == exp;
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s : %u %u %u => %d", __func__, address, idx, exp, result);
   return result;
}
void TestCore::clearAppDataBuffer()
{
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s", __func__);
   m_app_ntfs.clear();
}
bool TestCore::wasAppNtfSent(NTF_CMD_ID id, const std::vector<uint8_t>& msg)
{
   bool result = false;
   for (APP_NTF& ntf : m_app_ntfs)
   {
      if (ntf.id == id && ntf.payload == msg)
      {
         result = true;
         break;
      }
   }
   logger_send(TF_TEST_MARKER, "TEST_STEP", "%s:%u => %u", __func__, id, result);
   return result;
}
