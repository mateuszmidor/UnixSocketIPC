/**
 *   @file: MessageSender_test.cpp
 *
 *   @date: Aug 1, 2016
 * @author: Mateusz Midor
 */

#include "gtest/gtest.h"
#include "MessageSender.h"
#include "MessageReceiver.h"

using namespace std;
using namespace unixsocketipc;


TEST(MessageSender_test, testNooneListening) {
    MessageSender s;
    bool initialized = s.init("/tmp/socket_filepath");
    EXPECT_FALSE(initialized);
}

TEST(MessageSender_test, testUninitializedSend) {
    MessageSender s;
    bool successful_send = s.send(10, nullptr, 0); // but should first init() !
    EXPECT_FALSE(successful_send);
}

//TEST(MessageSender_test, testBadSocketFilename) {
//    auto cb = [](uint32_t, const char*, uint32_t) {};
//    MessageReceiver r;
//    r.init("/tmp/bad_socket_filepath", cb);
//
//    MessageSender s;
//    bool initialized = s.init("/tmp/bad_socket_filepath");
//    EXPECT_FALSE(initialized);
//}
