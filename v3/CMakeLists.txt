cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
project(fsasdk)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty)
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty/Eigen)

aux_source_directory(${PROJECT_SOURCE_DIR}/src SOURCES)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

add_library(${PROJECT_NAME}_static STATIC ${SOURCES})
set_target_properties(${PROJECT_NAME}_static PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib
)

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
