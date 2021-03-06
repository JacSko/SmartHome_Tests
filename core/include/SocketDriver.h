#ifndef _SOCKETDRIVER_H_
#define _SOCKETDRIVER_H_

/**
 * @file SocketDriver.h
 *
 * @brief
 *    Implementation of ISocketDriver interface.
 *
 * @author Jacek Skowronek
 * @date   05/02/2021
 *
 */
/* =============================
 *   Includes of common headers
 * =============================*/
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <functional>
#include <netinet/in.h>
/* =============================
 *   Includes of project headers
 * =============================*/
/* =============================
 *           Defines
 * =============================*/
#define SOCKDRV_MAX_RW_SIZE 1024
#define SOCKDRV_RECV_BUFFER_SIZE 1024
enum class DriverEvent
{
   DRIVER_CONNECTED,    /**< Driver connects successfully to server */
   DRIVER_DISCONNECTED, /**< Driver disconnected - server closed or error appears */
   DRIVER_DATA_RECV,    /**< New data received by driver */
};
typedef std::function<void(DriverEvent ev, const std::vector<uint8_t>& data, size_t count)> SocketListener;

class SocketDriver
{
public:
   SocketDriver();
   ~SocketDriver();

   bool connect(const std::string& ip_address, uint16_t port);
   bool disconnect();
   bool isConnected();
   void addListener(SocketListener callback);
   void removeListener();
   bool write(const std::vector<uint8_t>& data, size_t size = 0);

private:
   void setDelimiter(char c);
   void threadExecute();
   void notify_callbacks(DriverEvent ev, const std::vector<uint8_t>& data, size_t count);

   std::string m_server_address;
   uint16_t m_server_port;
   std::atomic<bool> m_is_connected;
   char m_delimiter;
   size_t m_recv_buffer_size;
   std::thread m_thread;
   std::atomic<bool> m_thread_running;
   std::mutex m_mutex;
   int m_sock_fd;
   int m_client;
   struct sockaddr_in m_serv_addr;
   SocketListener m_listener;
#if defined (SOCKDRV_FRIEND_TESTS)
   SOCKDRV_FRIEND_TESTS
#endif
};

#endif
