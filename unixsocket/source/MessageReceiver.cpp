/*
 * MessageReceiver.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: mateusz
 */

#include <cassert>
#include "MessageReceiver.h"
#include "MessageCommon.h"

using namespace unixsocketipc;

MessageReceiver::MessageReceiver() {
}

MessageReceiver::~MessageReceiver() {
   // close the sockets
   if (client_socket_fd)
      ::close(client_socket_fd);

   if (server_socket_fd)
      ::close(server_socket_fd);

   // remove socket file
   unlink(socket_filename.c_str());

   // get rid of the buffer
   delete[] message_buf;
   message_buf = nullptr;
}

bool MessageReceiver::init(const char *filename, CallbackFunc cb) {
   // remember socket filename
   socket_filename = filename;

   // get a socket filedescriptor
   server_socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);

   // check socket for failure
   if (server_socket_fd == -1) {
      DEBUG_MSG("%s: socket(AF_UNIX, SOCK_STREAM, 0) failed\n", __FUNCTION__);
      return false;
   }

   // prepare buffer
   message_buf = new char[MESSAGE_BUFF_SIZE];

   DEBUG_MSG("%s: binding to %s...\n", __FUNCTION__, socket_filename.c_str());
      // prepare address struct
      sockaddr_un local;
      local.sun_family = AF_UNIX;
      strcpy(local.sun_path, socket_filename.c_str());

      // delete socket file if such already exists
      unlink(local.sun_path);

      // bind socket to address in UNIX domain
      unsigned local_length = strlen(local.sun_path) + sizeof(local.sun_family);
      if (bind(server_socket_fd, (sockaddr*)&local, local_length) == -1) {
         DEBUG_MSG("%s: bind failed\n", __FUNCTION__);
         return false;
      }
   DEBUG_MSG("%s: done.\n", __FUNCTION__);

   // setup message reception callback
   callback = cb;

   // success
   return true;
}

void MessageReceiver::listen() {
   if (!server_socket_fd) {
      DEBUG_MSG("%s: not initialized\n", __FUNCTION__);
      return;
   }

   // mark socket as listening socket
   ::listen(server_socket_fd, 1); // 1 is max size of waiting connections queue, more connection will be rejected

   // listen for incoming connection
   DEBUG_MSG("%s: listening for incoming connection...\n", __FUNCTION__);
   sockaddr_un remote;
   unsigned remote_length = sizeof(remote);
   client_socket_fd = accept(server_socket_fd, (sockaddr*)&remote, &remote_length); // remote is filled with remote config
   if (client_socket_fd == -1) {
      DEBUG_MSG("%s: accept failed\n", __FUNCTION__);
      return;
   }

   // got client connected. Start reception loop
   DEBUG_MSG("%s: client connected. waiting for msg...\n", __FUNCTION__);

   while (true) {
      uint32_t id;
      uint32_t size;

      if (!receive_message(id, message_buf, size)) {
         DEBUG_MSG("%s: receive_message failed\n", __FUNCTION__);
         break;
      }

      if (id == STOP_LISTENING_MSG_ID) {
         DEBUG_MSG("%s: got STOP LISTENING\n", __FUNCTION__);
         break;
      }

      DEBUG_MSG("%s: calling callback: id:%d, size:%d, buf:%08X\n", __FUNCTION__, id, size, (uintptr_t) message_buf);
      if (size > 0)
         callback(id, message_buf, size);
      else
         callback(id, nullptr, 0);
   }

   DEBUG_MSG("%s: listening done\n", __FUNCTION__);
}

bool MessageReceiver::receive_message(uint32_t &id, char* buf, uint32_t &size ) {
   if (!receive_buffer(reinterpret_cast<char*>(&id), sizeof(id)))
      return false;

   if (!receive_buffer(reinterpret_cast<char*>(&size), sizeof(size)))
      return false;

   assert(size <= MESSAGE_BUFF_SIZE && "Received message size exceeds reception buffer size!");

   if (!receive_buffer(buf, size))
      return false;

   return true;
}

bool MessageReceiver::receive_buffer(char* buf, uint32_t size) {
   auto num_bytes_left = size;
   int num_bytes_received;
   while ((num_bytes_left > 0) && ((num_bytes_received = recv(client_socket_fd, buf, num_bytes_left, 0)) > 0)) {
      num_bytes_left -= num_bytes_received;
      buf += num_bytes_received;
      DEBUG_MSG("%s: received %d bytes\n", __FUNCTION__, num_bytes_received);
   }

   return (num_bytes_left == 0);
}

bool MessageReceiver::close() {
   return true;
}
