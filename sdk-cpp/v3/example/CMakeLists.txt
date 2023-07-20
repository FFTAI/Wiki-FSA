cmake_minimum_required(VERSION 3.16.1)

set(CMAKE_CXX_STANDARD 17)
project(main)

include_directories(${PROJECT_SOURCE_DIR}/../fsa-cpp-sdk/include)

link_directories(${PROJECT_SOURCE_DIR}/../fsa-cpp-sdk/lib)

include_directories(${PROJECT_SOURCE_DIR}/../thirdparty)

add_executable(main

    ${PROJECT_SOURCE_DIR}/demo_allinone.cpp
)

target_link_libraries(main fsasdk)