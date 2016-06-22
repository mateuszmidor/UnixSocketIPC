add_library(unixsocket-${ARCH}
            source/MessageSender.cpp
            source/MessageReceiver.cpp
)

target_include_directories(unixsocket-${ARCH}
                            PUBLIC 
                                source
)