/* =============================
 *   Includes of project headers
 * =============================*/
#include "SocketDriver.h"
#include "Logger.h"
#include "system_config_values.h"
/* =============================
 *   Includes of common headers
 * =============================*/
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>
#include <string.h>
#include "notification_types.h"

/* namespace wrapper around system function to allow replace in unit tests */
namespace system_call
{
__attribute__((weak)) int connect(int socket, const struct sockaddr *address, socklen_t address_len)
{
   return ::connect(socket, address, address_len);
}
__attribute__((weak)) ssize_t recv(int socket, void *buffer, size_t length, int flags)
{
   return ::recv(socket, buffer, length, flags);
}
__attribute__((weak)) ssize_t send(int socket, const void *message, size_t length, int flags)
{
   return ::send(socket, message, length, flags);
}
__attribute__((weak)) int socket(int domain, int type, int protocol)
{
   return ::socket(domain, type, protocol);
}
__attribute__((weak)) int close (int fd)
{
   return ::close(fd);
}
__attribute__((weak)) int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len)
{
   return ::setsockopt(socket, level, option_name, option_value, option_len);
}

}


SocketDriver::SocketDriver() :
m_server_address(""),
m_server_port(0),
m_is_connected(false),
m_delimiter(NTF_MESSAGE_DELIMITER),
m_recv_buffer_size(0),
m_thread_running(false),
m_sock_fd(-1),
m_client(-1)
{
}
bool SocketDriver::connect(const std::string& ip_address, uint16_t port)
{
   bool result = false;
   logger_send(TF_SOCKDRV, __func__, "");
   do
   {
      m_server_port = port;
      m_server_address = ip_address;
      if (disconnect())
      {
         logger_send(TF_ERROR, __func__, "[%d] sockthread disconnected", m_server_port);
      }

      if (ip_address.empty() || port == 0)
      {
         logger_send(TF_ERROR, __func__, " [%d]invalid data %s:%d", ip_address.c_str(), port, m_server_port);
         break;
      }
      m_sock_fd = system_call::socket(AF_INET, SOCK_STREAM, 0);
      if (m_sock_fd < 0)
      {
         logger_send(TF_ERROR, __func__, "[%d] cannot create socket, err: %s", strerror(errno), m_server_port);
         break;
      }
      struct timeval tv;
      tv.tv_sec = SOCK_RECV_TIMEOUT_S;
      tv.tv_usec = 0;
      if (system_call::setsockopt(m_sock_fd, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval)) < 0)
      {
         logger_send(TF_ERROR, __func__, "[%d] socket timeout err: %s", strerror(errno), m_server_port);
         break;
      }
      int enable = 1;
      if (system_call::setsockopt(m_sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
      {
         logger_send(TF_ERROR, __func__, "[%d] cannot set address reuse: %s", strerror(errno), m_server_port);
         break;
      }
      m_serv_addr.sin_family = AF_INET;
      m_serv_addr.sin_port = htons(port);

      if(inet_pton(AF_INET, ip_address.c_str(), &m_serv_addr.sin_addr)<=0)
      {
         logger_send(TF_ERROR, __func__, "[%d] cannot convert %s", ip_address.c_str(), m_server_port);
         break;
      }

      if (bind(m_sock_fd, (struct sockaddr *)&m_serv_addr, sizeof(m_serv_addr)) != 0)
      {
         logger_send(TF_ERROR, __func__, "[%d] bind failed: %s", m_server_port, strerror(errno));
         break;
      }

      if (listen(m_sock_fd, 1) < 0)
      {
         logger_send(TF_ERROR, __func__, "[%d] listen failed: %s", strerror(errno), m_server_port);
         break;
      }

      m_server_address = ip_address;
      m_server_port = port;
      m_thread = std::thread(&SocketDriver::threadExecute, this);
      while(!m_thread_running);
      result = true;
      logger_send(TF_SOCKDRV, __func__, "[%d] server started, avaiting connection!", m_server_port);

   }while(0);

   if (!result)
   {
      logger_send(TF_ERROR, __func__, "[%d] error", m_server_port);
      disconnect();
   }

   return result;
}

void SocketDriver::threadExecute()
{
   m_thread_running = true;
   size_t addrlen = sizeof(m_serv_addr);
   uint8_t msg_header [SOCK_MSG_HEADER_SIZE + 1];
   std::vector<uint8_t> recv_buffer (SOCKDRV_RECV_BUFFER_SIZE + 1, 0);
   ssize_t recv_bytes = 0;

   while (m_thread_running)
   {
      if(m_client < 0)
      {
         m_client = accept(m_sock_fd, (struct sockaddr *)&m_serv_addr, (socklen_t*)&addrlen);
         if (m_client >= 0)
         {
            logger_send(TF_SOCKDRV, __func__, "[%d] got client", m_server_port);
            notify_callbacks(DriverEvent::DRIVER_CONNECTED, {}, 0);
         }
         else
         {
            std::this_thread::sleep_for(std::chrono::milliseconds(SOCK_RECV_TIMEOUT_S));
         }
      }

      else if (m_client >= 0)
      {
         recv_bytes = recv(m_client, msg_header, SOCK_MSG_HEADER_SIZE, 0);
         if (recv_bytes == SOCK_MSG_HEADER_SIZE)
         {
            msg_header[SOCK_MSG_HEADER_SIZE] = 0x00;
            uint32_t len_to_read = atoi((char*)msg_header);
            recv_bytes = recv(m_client, recv_buffer.data(), len_to_read, 0);
            if (recv_bytes > 0)
            {
               recv_buffer[recv_bytes] = 0x00;
               notify_callbacks(DriverEvent::DRIVER_DATA_RECV, recv_buffer, recv_bytes);
            }
         }

         if (recv_bytes == 0)
         {
            logger_send(TF_SOCKDRV, __func__,"[%d] client disconnected", m_server_port);
            notify_callbacks(DriverEvent::DRIVER_DISCONNECTED, {}, 0);
            close(m_client);
            m_client = -1;
         }
         else if (errno == EAGAIN || errno == EWOULDBLOCK)
         {
         }
      }
   }

   if (m_client >= 0)
   {
      notify_callbacks(DriverEvent::DRIVER_DISCONNECTED, {}, 0);
      close(m_client);
      m_client = -1;
   }

}
void SocketDriver::notify_callbacks(DriverEvent ev, const std::vector<uint8_t>& data, size_t count)
{
   std::lock_guard<std::mutex> lock (m_mutex);

   switch(ev)
   {
   case DriverEvent::DRIVER_CONNECTED:
      m_is_connected = true;
      break;
   case DriverEvent::DRIVER_DISCONNECTED:
      m_is_connected = false;
      break;
   default:
      break;
   }

   if (m_listener)
   {
      m_listener(ev, data, count);
   }
}
bool SocketDriver::disconnect()
{
   bool result = false;
   if (m_thread_running)
   {
      m_thread_running = false;
      m_thread.join();
      result = true;
   }

   if (m_sock_fd >= 0)
   {
      close(m_sock_fd);
   }
   return result;

}
bool SocketDriver::isConnected()
{
   return m_is_connected;
}
void SocketDriver::addListener(SocketListener callback)
{
   std::lock_guard<std::mutex> lock (m_mutex);
   m_listener = callback;
}
void SocketDriver::removeListener()
{
   std::lock_guard<std::mutex> lock (m_mutex);
   m_listener = nullptr;
}
bool SocketDriver::write(const std::vector<uint8_t>& data, size_t size)
{
   bool result = false;
   ssize_t bytes_to_write = size == 0? data.size() : size;
   logger_send(TF_SOCKDRV, __func__, "[%d] writing %u bytes", size, m_server_port);
   uint8_t msg_header [SOCK_MSG_HEADER_SIZE + 1];
   if (bytes_to_write <= SOCKDRV_MAX_RW_SIZE)
   {
      std::snprintf((char*)msg_header, SOCK_MSG_HEADER_SIZE + 1, "%.4ld", size);
      send(m_client, msg_header, SOCK_MSG_HEADER_SIZE, 0);

      ssize_t bytes_written = 0;
      ssize_t current_write = 0;
      result = true;
      while (bytes_to_write > 0)
      {
         current_write = system_call::send(m_client, data.data() + bytes_written, bytes_to_write, 0);
         if (current_write > 0)
         {
            bytes_written += current_write;
         }
         else
         {
            result = false;
            break;
         }
         bytes_to_write -= bytes_written;
      }
   }
   logger_send_if(!result, TF_ERROR, __func__, "[%d] cannot write %u bytes", size, m_server_port);
   return result;
}
void SocketDriver::setDelimiter(char c)
{
   std::lock_guard<std::mutex> lock (m_mutex);
   logger_send(TF_SOCKDRV, __func__, "[%d] Setting new delimiter: %x", c, m_server_port);
   m_delimiter = c;
}
SocketDriver::~SocketDriver()
{
   disconnect();
}
