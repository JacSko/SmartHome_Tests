/* =============================
 *   Includes of common headers
 * =============================*/
#include <stdlib.h>
#include <stdarg.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <mutex>
/* =============================
 *  Includes of project headers
 * =============================*/
#include "Logger.h"
/* =============================
 *          Defines
 * =============================*/
#define LOGGER_BUFFER_SIZE 4096
/* =============================
 *   Internal module functions
 * =============================*/
void logger_notify_data();
/* =============================
 *       Internal types
 * =============================*/
typedef struct
{
   bool logfile_opened;
   std::ofstream logfile;
   std::string logfile_path;
   std::mutex mtx;
} LOGGER;
typedef struct LOG_GROUP
{
   LogGroup id;
   const char* name;
} LOG_GROUP;
/* =============================
 *      Module variables
 * =============================*/
std::vector<char> m_logger_buffer;
LOG_GROUP LOGGER_GROUPS[LOG_ENUM_MAX] = {
                        {STM_BLUETOOTH, "STM_BLUETOOTH"},
                        {STM_WIFI_NTF, "STM_WIFI_NTF"},
                        {STM_HW_STUB, "STM_HW_STUB"},
                        {TF_TEST_MARKER, "TF_TEST_MARKER"},
                        {TF_ERROR, "TF_ERROR"},
                        {TF_SOCKDRV, "TF_SOCKDRV"},
                        {TF_TC, "TF_TC"}};

LOGGER m_logger;

bool logger_initialize(const std::string& logfile_path)
{
   bool result = true;
   m_logger_buffer.resize(LOGGER_BUFFER_SIZE);
   m_logger.logfile_opened = false;
   m_logger.logfile_path = logfile_path;

   if (logfile_path.size() > 0)
   {
      char complete_path [512];
      snprintf(complete_path, 512, "%s/logs/%s.txt", PROJECT_ROOT_PATH, logfile_path.c_str());

      printf("opening file:%s:\n", complete_path);
      m_logger.logfile.open(std::string(complete_path), std::ios::out);
      if (m_logger.logfile)
      {
         m_logger.logfile_opened = true;
      }
      else
      {
         printf("Cannot open file %s\n", logfile_path.c_str());
         result = false;
      }
   }
   return result;
}

void logger_deinitialize()
{
   m_logger_buffer.clear();
   m_logger.logfile.close();
   m_logger.logfile_opened = false;
   m_logger.logfile_path = "";

}
void logger_notify_data()
{
   if (m_logger.logfile_opened)
   {
      m_logger.logfile << std::string(m_logger_buffer.data());
      m_logger.logfile.flush();
   }
   std::cout << std::string(m_logger_buffer.data());
}
void logger_send(LogGroup group, const char* prefix, const char* fmt, ...)
{
   if (group < LOG_ENUM_MAX)
   {
      std::lock_guard<std::mutex> lock (m_logger.mtx);
      va_list va;
      {
         auto currentTime = std::chrono::system_clock::now();
         auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
         std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
         auto timeinfo = localtime (&tt);
         int idx = strftime (m_logger_buffer.data(),80,"[%F %H:%M:%S",timeinfo);
         idx = sprintf(m_logger_buffer.data(), "%s:%03d] %s - %s - ",m_logger_buffer.data(),(int)millis, LOGGER_GROUPS[group].name, prefix);
         va_start(va, fmt);
         {
             idx += vsprintf(m_logger_buffer.data() + idx, fmt, va);
         }
         va_end(va);
         m_logger_buffer[idx++] = '\n';
         m_logger_buffer[idx] = 0x00;
         logger_notify_data();
      }
   }
}
void logger_send_if(uint8_t cond_bool, LogGroup group, const char* prefix, const char* fmt, ...)
{
   if (cond_bool != 0 && group < LOG_ENUM_MAX)
   {
      std::lock_guard<std::mutex> lock (m_logger.mtx);
      va_list va;
      {
         auto currentTime = std::chrono::system_clock::now();
         auto millis = (currentTime.time_since_epoch().count() / 1000000) % 1000;
         std::time_t tt = std::chrono::system_clock::to_time_t ( currentTime );
         auto timeinfo = localtime (&tt);
         int idx = strftime (m_logger_buffer.data(),80,"[%F %H:%M:%S",timeinfo);
         idx = sprintf(m_logger_buffer.data(), "%s:%03d] %s - %s - ",m_logger_buffer.data(),(int)millis, LOGGER_GROUPS[group].name, prefix);
         va_start(va, fmt);
         {
             idx += vsprintf(m_logger_buffer.data() + idx, fmt, va);
         }
         va_end(va);
         m_logger_buffer[idx++] = '\n';
         m_logger_buffer[idx] = 0x00;
         logger_notify_data();
      }
   }
}
