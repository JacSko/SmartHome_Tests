#ifndef _LOGGER_H_
#define _LOGGER_H_
/* ============================= */
/**
 * @file Logger.h
 *
 * @brief Allows to send debug traces to standard output, with possibility to dump traces to file.
 *
 * @details
 *    Logger is adjusted to test framework - it means, that there are some special functions to put some markers into log (like test case begin).
 *
 * @author Jacek Skowronek
 * @date 13/12/2020
 */
/* ============================= */

/* =============================
 *  Includes of common headers
 * =============================*/
#include <stdint.h>
/* =============================
 *  Includes of project headers
 * =============================*/
/* =============================
 *          Defines
 * =============================*/
/* =============================
 *       Data structures
 * =============================*/
enum LogGroup
{
   STM_BLUETOOTH,    /**< Logs from SmartHome binary that are sent by stubbed Bluetooth module */
   STM_WIFI_NTF,     /**< Logs from SmartHome binary that are sent by stubbed WiFi module */
   STM_HW_STUB,      /**< Logs from SmartHome binary hw_stub */
   TF_TEST_MARKER,   /**< Puts test marker for easy log analysis */
   TF_ERROR,         /**< Error logs */
   TF_SOCKDRV,       /**< Logs from test framework - Socket driver */
   LOG_ENUM_MAX,
};

/**
 * @brief Initialize Logger module. If not file_path provided, only standard output will be used
 * @param[in] file_path - path to the output file, where logs will be placed
 * @return True if initialized correctly.
 */
bool logger_initialize(std::string& file_path);
/**
 * @brief Deinitialize Logger module.
 * @return None.
 */
void logger_deinitialize();
/**
 * @brief Puts marker into log file.
 * @return None.
 */
void logger_put_marker(std::string& marker);
/**
 * @brief Sends log string.
 * @param[in] group - the group to which data is related
 * @param[in] prefix - short prefix added to log string
 * @param[in] fmt - printf-like format of string
 * @param[in] ... - list of arguments to format
 * @return None.
 */
void logger_send(LogGroup group, const char* prefix, const char* fmt, ...);
/**
 * @brief Sends log string conditionally.
 * @param[in] cond_bool - expression (log will be send if this is true.
 * @param[in] group - the group to which data is related
 * @param[in] prefix - short prefix added to log string
 * @param[in] fmt - printf-like format of string
 * @param[in] ... - list of arguments to format
 * @return None.
 */
void logger_send_if(uint8_t cond_bool, LogGroup group, const char* prefix, const char* fmt, ...);

#endif
