
# cmake需要的最低版本
cmake_minimum_required (VERSION 3.14)
cmake_policy(SET CMP0048 NEW)
project (fourier VERSION 0.7.1 LANGUAGES CXX)

message("${PROJECT_NAME}---${CMAKE_CURRENT_SOURCE_DIR}")

include(FetchContent)
FetchContent_Declare(
    libuv
    GIT_REPOSITORY https://github.com/libuv/libuv
    GIT_TAG 3f331e97da5b1cc2e24765dcb6d26c9dd313418f
    CMAKE_ARGS -DLIBUV_BUILD_SHARED=OFF -DLIBUV_BUILD_TESTS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=TRUE -DCMAKE_BUILD_TYPE=Release
)

FetchContent_MakeAvailable(libuv)
set(INC_PATH ${libuv_SOURCE_DIR}/include)
include_directories(${INC_PATH})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../third-party)

IF (WIN32)
	MESSAGE(STATUS "Now is windows")

    # 设置dll与lib输出路径
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/win_x64)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/win_x64)

    add_definitions(-DSPDLOG_WCHAR_FILENAMES) # win下激活spdlog使用wchar
    add_definitions(-DUNICODE -D_UNICODE) # win下使用unicode编码
ELSEIF (APPLE)
	MESSAGE(STATUS "Now is Apple systens.")

    # 设置dylib输出路径
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/mac_x86_64)
    MESSAGE(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")

    # 执行命令获取平台架构
    EXECUTE_PROCESS( COMMAND uname -m COMMAND tr -d '\n' OUTPUT_VARIABLE ARCHITECTURE )
    message( STATUS "Architecture: ${ARCHITECTURE}" )
    if( ${ARCHITECTURE} STREQUAL  "armv7l" )
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux_armv7l)
        MESSAGE(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    elseif(${ARCHITECTURE} STREQUAL "aarch64")
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux_aarch64)
        MESSAGE(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    elseif( ${ARCHITECTURE} STREQUAL  "x86_64" )
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/lib/linux_x86_64)
        MESSAGE(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
    else()
        message( "ERROR: Platform architecture mismatch" )
    endif()
ENDIF ()


# 生成动态库
add_library(${PROJECT_NAME} SHARED
    aios/lookup.cpp
    aios/group.cpp
    aios/actuator.h
    aios/actuator.cpp
    aios/groupFeedback.cpp
    aios/groupCommand.cpp
    aios/version.cpp
    aios/aios.def 
    aios/log.h 
    aios/log.cpp
)


IF (WIN32)
    SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
ENDIF()
# target_compile_options(${PROJECT_NAME} PRIVATE -fPIC)
set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_property(TARGET fourier PROPERTY INTERFACE_LINK_LIBRARIES
    $<$<BOOL:${UNIX}>:m pthread>
    $<$<BOOL:${WIN32}>:kernel32>
)

if(WIN32)
    set(LIB_PATH ${libuv_BINARY_DIR}/Release)
    link_directories(${LIB_PATH})
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_PATH}/libuv.lib)
elseif(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${libuv_BINARY_DIR}/libuv.a)
else()
    target_link_libraries(${PROJECT_NAME} PRIVATE ${libuv_BINARY_DIR}/libuv.a -pthread -ldl)
endif()

set_target_properties(fourier PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED YES
    POSITION_INDEPENDENT_CODE ON
    VERSION ${fourier_VERSION}
)
