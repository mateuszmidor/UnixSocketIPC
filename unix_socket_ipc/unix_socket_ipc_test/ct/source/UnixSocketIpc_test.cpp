/**
 *   @file: UnixSocketIpc_test.cpp
 *
 *   @date: Aug 1, 2016
 * @author: Mateusz Midor
 */

#include <thread>
#include <mutex>
#include <condition_variable>
#include "gtest/gtest.h"
#include "MessageSender.h"
#include "MessageReceiver.h"
#include <iostream>
using namespace std;
using namespace unixsocketipc;

// connection socket filename
const char* SOCKET_FILENAME = "/tmp/unixsocketipc_test";

// data sending
const int MSG_ID_DATA = 5;


// thread synchronization mechanism
mutex m;
condition_variable cv_receiver_ready;
bool receiver_ready = false;

mutex mm;
condition_variable cv_data_received;
bool data_received = false;

// This function listens for incoming message and then stores the received data
void receiver_func(int64_t volatile *received_data) {

    auto cb = [received_data](uint32_t message_id, const char* data, uint32_t len) {
        if (message_id == MSG_ID_DATA) {
            // update the data
            *received_data = *reinterpret_cast<int64_t const *>(data);

            // notify that the data has been received and can be read
            data_received = true;
            cv_data_received.notify_one();
        }
    };

    MessageReceiver receiver;
    receiver.init(SOCKET_FILENAME, cb);
    unique_lock<mutex> lck(m); // "receiver_ready" is shared state; protect it
    receiver_ready = true;
    cv_receiver_ready.notify_one();
    lck.unlock(); // let the main thread read "receiver_ready"
    receiver.listen(); // this call blocks
}

// This function sends data
void sender_func (int64_t data, bool stop_listener) {
    data_received = false;
    MessageSender sender;
    sender.init(SOCKET_FILENAME);
    sender.send(MSG_ID_DATA, (const char*)&data, sizeof(data));
    if (stop_listener)
        sender.send_stop_listener();
}

// This function awaits receiver ready state
void wait_receiver_ready() {
    unique_lock<mutex> lock(m);
    cv_receiver_ready.wait(lock, []() { return receiver_ready; });
}

// This function awaits until data is received
void wait_data_received() {
    unique_lock<mutex> lock(mm);
    cv_data_received.wait(lock, []() { return data_received; });
}

TEST(UnixSocketIpc_test, testSendReceiveData) {
    const int64_t DATA_TO_SEND = 1234567890123;
    int64_t received_data = 0;

    // start receiver
    receiver_ready = false;
    thread receiver_thread {receiver_func, &received_data};

    // wait till receiver is ready
    wait_receiver_ready();

    // send some data
    thread sender_thread {sender_func, DATA_TO_SEND, true};

    sender_thread.join();
    receiver_thread.join();

    // check if data properly received
    wait_data_received();
    EXPECT_EQ(received_data, DATA_TO_SEND);
}

TEST(UnixSocketIpc_test, testConnectNewClient) {
    const int64_t DATA_TO_SEND1 = 1234567890123;
    const int64_t DATA_TO_SEND2 = 555;
    const int64_t DATA_TO_SEND3 = 1020304050;
    volatile int64_t received_data = 0;

    // start receiver
    receiver_ready = false;
    thread receiver_thread {receiver_func, &received_data};

    // wait till receiver is ready
    wait_receiver_ready();

    // send data 1
    thread sender_thread1 {sender_func, DATA_TO_SEND1, false};
    sender_thread1.join();
    // check result
    wait_data_received();
    EXPECT_EQ(received_data, DATA_TO_SEND1);

    // send data 2
    thread sender_thread2 {sender_func, DATA_TO_SEND2, false};
    sender_thread2.join();
    // check result
    wait_data_received();
    EXPECT_EQ(received_data, DATA_TO_SEND2);

    // send data 3 and close the receiver
    thread sender_thread3 {sender_func, DATA_TO_SEND3, true};
    sender_thread3.join();
    // check result
    wait_data_received();
    EXPECT_EQ(received_data, DATA_TO_SEND3);

    receiver_thread.join();
}
