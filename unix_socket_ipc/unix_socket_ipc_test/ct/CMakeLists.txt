set(UnixSocketIpc_sources
        "source/main.cpp"
        "source/MessageSender_test.cpp"
        "source/MessageReceiver_test.cpp"
        "source/UnixSocketIpc_test.cpp"
)
add_executable(UnixSocketIpcTest ${UnixSocketIpc_sources})

target_link_libraries(UnixSocketIpcTest
                        UnixSocketIpcLib
                        gtest_main
)

# make test
add_test(UnixSocketIpcTest UnixSocketIpcTest)