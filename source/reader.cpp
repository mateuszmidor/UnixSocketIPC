/*
 * reader.cpp
 *
 *  Created on: Jun 9, 2016
 *      Author: mateusz
 */

#include "MessageReceiver.h"
#include <cstdio>

using namespace unixsocketipc;

void callback(uint32_t msgid, const char* data, uint32_t datasize) {
   printf("Received: msgid %d, size %d, content: %s\n", msgid, datasize, data);
}

void readMessages() {
   MessageReceiver rx;
   if (!rx.init("/tmp/MessageSenderReceiverSocket", callback)) {
      printf("rx.init failed.\n");
      return;
   }

   printf("reader ready. waiting for connection...\n");
   rx.listen();
}

int main() {
   readMessages();
}
