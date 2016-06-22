cmake_minimum_required(VERSION 3.2)

set(PROJECT_MAJOR_VERSION 0)
set(PROJECT_MINOR_VERSION 1)
set(PROJECT_PATCH_LEVEL 0)
set(PROJECT_VERSION ${PROJECT_MAJOR_VERSION}.${PROJECT_MINOR_VERSION}.${PROJECT_PATCH_LEVEL})

set(ARCH "32bit")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11 -m32")

add_subdirectory(../unixsocket unixsocket-${ARCH})

ADD_EXECUTABLE(reader-${ARCH}
                "../source/reader.cpp"
)
target_link_libraries(reader-${ARCH} 
    unixsocket-${ARCH}
)

install(TARGETS reader-${ARCH}    
        DESTINATION ${PROJECT_BINARY_DIR}
)