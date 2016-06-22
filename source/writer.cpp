/*
 * writer.cpp
 *
 *  Created on: Jun 9, 2016
 *      Author: mateusz
 */

#include <string>
#include <unistd.h>
#include "MessageSender.h"

using namespace std;
using namespace unixsocketipc;

void write() {
   const string WORDS[] = {"Litwo", "ojczyzno", "moja", "Ty", "jesteś", "jak", "zdrowie", "!"};


   MessageSender tx;
   if (!tx.init("/tmp/MessageSenderReceiverSocket")) {
      printf("tx.init failed.\n");
      return;
   }

   // send words in a loop
   for (const auto &word : WORDS) {
      printf("Sending: %s\n", word.c_str());
      tx.send(123, word.c_str(), word.length() + 1); // +1 for '\0'
      sleep(1);
   }

   tx.send_stop_listener();
}

int main() {
   write();
}
