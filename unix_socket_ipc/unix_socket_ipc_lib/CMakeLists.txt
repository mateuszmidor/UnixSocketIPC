add_library(UnixSocketIpcLib
            source/MessageSender.cpp
            source/MessageReceiver.cpp
)

target_include_directories(UnixSocketIpcLib
                            PUBLIC 
                                "source"
)