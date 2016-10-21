/**
 * MessageReceiver.cpp
 *
 *  Created on: Jun 22, 2016
 *      Author: Mateusz Midor
 */

#include <cassert>
#include "MessageReceiver.h"
#include "MessageCommon.h"

using namespace unixsocketipc;

MessageReceiver::MessageReceiver() {
}

/**
 * @name    ~MessageReceiver
 * @brief   Destructor. Clean up buffers and underlying unix socket resources
 */
MessageReceiver::~MessageReceiver() {
   // close the socket
   if (server_socket_fd)
      ::close(server_socket_fd);

   // remove socket file
   unlink(socket_filename.c_str());

   // get rid of the buffer
   delete[] message_buf;
   message_buf = nullptr;
}

/**
 * @name    init
 * @brief   Setup the receiver for listening on socket pointed by filename
 * @param   filename Socket filename
 * @param   cb Callback function to be called upon message reception
 * @param   oc Callback function to be called when a new client gets connected
 * @return  True if successful, False otherwise
 * @note    This method must be called before "listen"
 */
bool MessageReceiver::init(const char *filename, CallbackFunc cb, OnClientConnected oc) {
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
         ::close(server_socket_fd);
         return false;
      }
   DEBUG_MSG("%s: done.\n", __FUNCTION__);

   // setup callbacks
   callback = cb;
   on_client_connected = oc;

   // success
   return true;
}

/**
 * @name    listen
 * @brief   Start listening for incoming connection, then receive messages and call the callback function in a loop
 * @note    "init" must be called before starting listen
 * @return  True if listen ended successfuly, False if any error during listening occured
 */
bool MessageReceiver::listen() {
   if (!server_socket_fd) {
      DEBUG_MSG("%s: not initialized\n", __FUNCTION__);
      return false;
   }

   // mark socket as listening socket
   ::listen(server_socket_fd, 1); // 1 is max size of waiting connections queue, more connection will be rejected

   sockaddr_un remote;
   unsigned remote_length = sizeof(remote);

   // listen for incoming connections
   bool keep_working = true;
   while (keep_working) {
       DEBUG_MSG("%s: listening for incoming connection...\n", __FUNCTION__);

       // 1. accept new client socket
       int client_socket_fd = accept(server_socket_fd, (sockaddr*)&remote, &remote_length); // remote is filled with remote config
       if (client_socket_fd == -1) {
          DEBUG_MSG("%s: accept failed\n", __FUNCTION__);
          return false;
       }

       // 2. notify about new client connected
       if (on_client_connected)
           on_client_connected();

       // 3. handle the client
       keep_working = handle_next_client(client_socket_fd);

       // 4. communication done. Close the client socket
      ::close(client_socket_fd);
   }

   DEBUG_MSG("%s: listening done\n", __FUNCTION__);
   return true;
}

/**
 * @name    handle_next_client
 * @brief   Read messages from the connected sender and call the configured callback function
 *          until the sender disconnects/sends STOP_LISTENING to the receiver
 * @param   client_socket_fd File descriptor of the connected client
 * @return  true if the receiver should accept next client, false if it should exit(received STOP_LISTENING)
 */
bool MessageReceiver::handle_next_client(int client_socket_fd) {
    DEBUG_MSG("%s: client connected. waiting for msg...\n", __FUNCTION__);
    while (true) {
        uint32_t id;
        uint32_t size;

        if (!receive_message(client_socket_fd, id, message_buf, size)) {
            DEBUG_MSG("%s: receive_message failed\n", __FUNCTION__);
            return true; // probably connection broken. accept next sender
        }

        if (id == STOP_LISTENING_MSG_ID) {
            DEBUG_MSG("%s: got STOP LISTENING\n", __FUNCTION__);
            return false; // stop the listener, finish work
        }

        DEBUG_MSG("%s: calling callback: id:%d, size:%d\n", __FUNCTION__, id, size);
        if (size > 0)
            callback(id, message_buf, size);
        else
            callback(id, nullptr, 0);
    }
}

/**
 * @name    receive_message
 * @note    Implementation detail
 */
bool MessageReceiver::receive_message(int client_socket_fd, uint32_t &id, char* buf, uint32_t &size ) {
   if (!receive_buffer(client_socket_fd, reinterpret_cast<char*>(&id), sizeof(id)))
      return false;

   if (!receive_buffer(client_socket_fd, reinterpret_cast<char*>(&size), sizeof(size)))
      return false;

   assert(size <= MESSAGE_BUFF_SIZE && "Received message size exceeds reception buffer size!");

   if (!receive_buffer(client_socket_fd, buf, size))
      return false;

   return true;
}

/**
 * @name    receive_buffer
 * @note    Implementation detail
 */
bool MessageReceiver::receive_buffer(int client_socket_fd, char* buf, uint32_t size) {
   auto num_bytes_left = size;
   int num_bytes_received;
   while ((num_bytes_left > 0) && ((num_bytes_received = recv(client_socket_fd, buf, num_bytes_left, 0)) > 0)) {
      num_bytes_left -= num_bytes_received;
      buf += num_bytes_received;
      DEBUG_MSG("%s: received %d bytes\n", __FUNCTION__, num_bytes_received);
   }

   return (num_bytes_left == 0);
}
