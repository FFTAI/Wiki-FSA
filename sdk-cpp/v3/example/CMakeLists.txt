cmake_minimum_required(VERSION 3.16.1)

set(CMAKE_CXX_STANDARD 17)
project(main)

include_directories("/home/fugo/Documents/actuator-fsa-wiki/sdk-cpp/v3/Fsa/include")
link_directories("/home/fugo/Documents/actuator-fsa-wiki/sdk-cpp/v3/Fsa/install/lib")

include_directories("/home/fugo/Documents/actuator-fsa-wiki/sdk-cpp/v3/thirdparty")

# aux_source_directory(/home/fugo/Documents/actuator-fsa-wiki/sdk-cpp/v3/Fsa/src dir_fsa)
add_executable(main

    /home/fugo/Documents/actuator-fsa-wiki/sdk-cpp/v3/example/demo_allinone.cpp
)

target_link_libraries(main fsasdk)