/*
 * MessageSender.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: mateusz
 */

#include "MessageSender.h"
#include "MessageCommon.h"

using namespace unixsocketipc;

MessageSender::MessageSender() {
}

MessageSender::~MessageSender() {
   // close the socket

   if (server_socket_fd)
      ::close(server_socket_fd);

   // remove socket file
   unlink(socket_filename.c_str());
}

bool MessageSender::init(const char *filename) {
   // remember socket filename
   socket_filename = filename;

   // get a socket filedescriptor
   server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

   // check socket for failure
   if (server_socket_fd == -1) {
      DEBUG_MSG("%s: socket(AF_UNIX, SOCK_STREAM, 0) failed\n", __FUNCTION__);
      return false;
   }

   DEBUG_MSG("%s: connecting to listener %s...\n", __FUNCTION__, socket_filename.c_str());
      sockaddr_un remote;
      remote.sun_family = AF_UNIX;
      strcpy(remote.sun_path, socket_filename.c_str());
      unsigned length = strlen(remote.sun_path) + sizeof(remote.sun_family);
      if (connect(server_socket_fd, (sockaddr*)&remote, length) == -1) {
         DEBUG_MSG("%s: connect failed\n", __FUNCTION__);
         return false;
      }
   DEBUG_MSG("%s: done.\n", __FUNCTION__);

   // success
   return true;
}

void MessageSender::send(uint32_t id, const char *data, uint32_t size) {
   if (!server_socket_fd) {
      DEBUG_MSG("%s: not initialized\n", __FUNCTION__);
      return;
   }

   // send the message
   DEBUG_MSG ("%s: sending message: id:%d, size:%d, @:%08X\n", __FUNCTION__, id, size, (uintptr_t) data);
   if (!send_message(id, data, size))
      DEBUG_MSG("%s: send_message failed\n", __FUNCTION__);
}

bool MessageSender::send_message(uint32_t id, const char *buf, uint32_t size) {
   if (!send_buffer(reinterpret_cast<char*>(&id), sizeof(id)))
      return false;

   if (!send_buffer(reinterpret_cast<char*>(&size), sizeof(size)))
      return false;

   if (!send_buffer(buf, size))
      return false;

   return true;
}

bool MessageSender::send_buffer(const char *buf, uint32_t size) {
   auto bytes_left = size;
   int num_bytes_sent;
   while ((bytes_left > 0) && ((num_bytes_sent = ::send(server_socket_fd, buf, bytes_left, MSG_NOSIGNAL)) > 0)) {
      bytes_left -= num_bytes_sent;
      buf += num_bytes_sent;
      DEBUG_MSG("%s: sent %d bytes\n", __FUNCTION__, num_bytes_sent);
   }

   return (bytes_left == 0); // success if all bytes sent
}

void MessageSender::send_stop_listener() {
   send(STOP_LISTENING_MSG_ID, nullptr, 0);
}
