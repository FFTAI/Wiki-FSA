cmake_minimum_required(VERSION 3.16)

project(sdk_cpp_fsa)

include_directories(include)
set(SOURCES
    ${PROJECT_SOURCE_DIR}/source/fsa.cpp
    ${PROJECT_SOURCE_DIR}/source/os.cpp
    ${PROJECT_SOURCE_DIR}/source/logger.cpp
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

add_library(${PROJECT_NAME}_static STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

add_subdirectory(example build)

# 格式化代码
add_custom_target(
    clangformat
    COMMAND /usr/bin/clang-format
    -i
    ${CMAKE_CURRENT_SOURCE_DIR}/**/*.cpp ${CMAKE_CURRENT_SOURCE_DIR}/**/*.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/**/CMakeLists.txt
    COMMENT "Formatting CMakeLists.txt"
)
