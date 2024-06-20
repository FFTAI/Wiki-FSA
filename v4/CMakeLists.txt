cmake_minimum_required(VERSION 3.16)

project(fsasdk_v4)

include_directories(include)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCES)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME})

add_library(${PROJECT_NAME} SHARED ${SOURCES})
add_library(${PROJECT_NAME}_static STATIC ${SOURCES})

if(WIN32)
    target_link_libraries(${PROJECT_NAME} ws2_32)
endif()

# 编译示例程序
add_subdirectory(example build)

# 格式化代码
add_custom_target(
    clangformat
    COMMAND /usr/bin/clang-format
    -i
    ${PROJECT_SOURCE_DIR}/**/*.cpp ${PROJECT_SOURCE_DIR}/**/*.h
    COMMENT "Formatting Cpp Files"
)

execute_process(
    COMMAND bash -c "echo ~"
    OUTPUT_VARIABLE HOME_DIR
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
add_custom_target(
    cmakeformat 
    COMMAND ${HOME_DIR}/.local/bin/cmake-format
    -i
    ${PROJECT_SOURCE_DIR}/**/CMakeLists.txt
    COMMENT "Formatting CMakeLists.txt"
)
