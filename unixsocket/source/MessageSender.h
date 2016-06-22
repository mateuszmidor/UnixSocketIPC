/**
 * MessageSender.h
 *
 *  Created on: Jun 22, 2016
 *      Author: Mateusz Midor
 */

#ifndef UNIXSOCKET_SOURCE_MESSAGESENDER_H_
#define UNIXSOCKET_SOURCE_MESSAGESENDER_H_


#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <unistd.h>
#include <cstdio>
#include <string>

namespace unixsocketipc {

/**
 * @class   MessageSender
 * @brief   Allows sending messages over unix domain socket
 */
class MessageSender {
   std::string socket_filename;
   int server_socket_fd = 0;

   bool send_message(uint32_t id, const char *buf, uint32_t size);
   bool send_buffer(const char *buf, uint32_t size);

public:
   bool init(const char *filename);
   void send(uint32_t id, const char *data, uint32_t size);
   void send_stop_listener();

   MessageSender();
   virtual ~MessageSender();
};

}

#endif /* UNIXSOCKET_SOURCE_MESSAGESENDER_H_ */
