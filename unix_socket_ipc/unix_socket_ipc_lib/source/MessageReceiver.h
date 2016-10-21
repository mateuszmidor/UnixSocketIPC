/**
 * MessageReceiver.h
 *
 *  Created on: Jun 22, 2016
 *      Author: Mateusz Midor
 */

#ifndef UNIXSOCKET_SOURCE_MESSAGERECEIVER_H_
#define UNIXSOCKET_SOURCE_MESSAGERECEIVER_H_


#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <string>
#include <functional>

namespace unixsocketipc {

/**
 * @class   MessageReceiver
 * @brief   Allows receiving messages that are sent over unix domain socket
 */
class MessageReceiver {
   using CallbackFunc = std::function<void(uint32_t, const char*, uint32_t)>;
   using OnClientConnected = std::function<void(void)>;

   CallbackFunc callback;
   OnClientConnected on_client_connected;
   std::string socket_filename;
   int server_socket_fd = 0;
   char* message_buf = nullptr;

   bool receive_message(int client_socket_fd, uint32_t &id, char* buf, uint32_t &size);
   bool receive_buffer(int client_socket_fd, char* buf, uint32_t size);
   bool handle_next_client(int client_socket_fd);

public:
   bool init(const char *filename, CallbackFunc cb, OnClientConnected oc = OnClientConnected());
   bool listen();

   MessageReceiver();
   virtual ~MessageReceiver();
};

}

#endif /* UNIXSOCKET_SOURCE_MESSAGERECEIVER_H_ */
