/**
 *   @file: MessageReceiver_test.cpp
 *
 *   @date: Aug 1, 2016
 * @author: Mateusz Midor
 */



#include "gtest/gtest.h"
#include "MessageSender.h"
#include "MessageReceiver.h"

using namespace std;
using namespace unixsocketipc;


TEST(MessageReceiver_test, testBadSocketFilename) {
    MessageReceiver r;
    auto cb = [](uint32_t, const char*, uint32_t) {};
    bool initialized = r.init("/sys/socket_filepath", cb);
    EXPECT_FALSE(initialized);
}

TEST(MessageReceiver_test, testUninitializedListen) {
    MessageReceiver r;
    bool successful_listen = r.listen(); // but need to init() first!
    EXPECT_FALSE(successful_listen);
}
