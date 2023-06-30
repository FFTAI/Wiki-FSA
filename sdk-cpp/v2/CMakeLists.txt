# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
# cmake需要的最低版本
cmake_minimum_required (VERSION 3.14)

# 项目名称
cmake_policy(SET CMP0048 NEW)
project (fourier_cpp_api VERSION 0.7.1 LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Release")

# 添加编译宏
# add_definitions(-DDEBUG)

# 指定头文件搜索路径
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/fourier/include)


# 跨平台引入库目录头文件
IF (WIN32)
    link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/fourier/lib/win_x64
    )
ELSEIF (APPLE)
    link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/fourier/lib/mac_x86_64
    )
ELSEIF (UNIX)
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    if( ${ARCHITECTURE} STREQUAL  "armv7l" )
        link_directories(
            ${CMAKE_CURRENT_SOURCE_DIR}/fourier/lib/linux_armv7l
        )
    elseif(${ARCHITECTURE} STREQUAL "aarch64")
        link_directories(
            ${CMAKE_CURRNET_SOURCE_DIR}/fourier/lib/linux_aarch64
        )
    elseif( ${ARCHITECTURE} STREQUAL  "x86_64" )
        link_directories(
            ${CMAKE_CURRENT_SOURCE_DIR}/fourier/lib/linux_x86_64
        )
    else()
        message( "ERROR: Platform architecture mismatch" )
    endif()
ENDIF ()

# 指定编译子目录，sdk目录
add_subdirectory (fourier)

# 编译静态库
set(fourier_cpp_src
    src/lookup.cpp
    src/groupCommand.cpp
    src/groupFeedback.cpp
    src/group.cpp
    src/version.cpp
)
    
# 编译库
add_library(_fourierc++-obj OBJECT ${fourier_cpp_src})
add_library(fourierc++ SHARED $<TARGET_OBJECTS:_fourierc++-obj>)
add_library(fourierc++-static STATIC $<TARGET_OBJECTS:_fourierc++-obj>)

# 链接动态库fourier
target_link_libraries(fourierc++ PUBLIC fourier)
target_link_libraries(fourierc++-static PUBLIC fourier)

target_include_directories(_fourierc++-obj
  PUBLIC src include fourier
)
target_include_directories(fourierc++ INTERFACE $<TARGET_PROPERTY:_fourierc++-obj,INTERFACE_INCLUDE_DIRECTORIES>)
target_include_directories(fourierc++-static INTERFACE $<TARGET_PROPERTY:_fourierc++-obj,INTERFACE_INCLUDE_DIRECTORIES>)

# 项目支持C++14
set_target_properties(_fourierc++-obj fourierc++ fourierc++-static PROPERTIES
  CXX_STANDARD 14
  CXX_STANDARD_REQUIRED YES
  POSITION_INDEPENDENT_CODE ON
  VERSION ${fourier_cpp_api_VERSION})

if (WIN32)
  set_target_properties(fourierc++ PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS TRUE)
endif()

set_property(TARGET fourierc++ fourierc++-static PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<BOOL:${UNIX}>:m pthread>
    $<$<BOOL:${WIN32}>:kernel32>
)
