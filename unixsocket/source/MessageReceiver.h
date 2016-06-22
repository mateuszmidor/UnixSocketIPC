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

   CallbackFunc callback;
   std::string socket_filename;
   int server_socket_fd = 0;
   int client_socket_fd = 0;
   char* message_buf = nullptr;

   bool receive_message(uint32_t &id, char* buf, uint32_t &size);
   bool receive_buffer(char* buf, uint32_t size);
public:
   bool init(const char *filename, CallbackFunc cb);
   void listen();

   MessageReceiver();
   virtual ~MessageReceiver();
};

}

#endif /* UNIXSOCKET_SOURCE_MESSAGERECEIVER_H_ */
