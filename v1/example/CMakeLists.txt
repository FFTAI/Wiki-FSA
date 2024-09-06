cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_STANDARD 17)
project(sdk_cpp_fsa_v3_example)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty)
include_directories(${PROJECT_SOURCE_DIR}/ThirdParty/Eigen)

link_directories(${PROJECT_SOURCE_DIR}/lib)

# 获取example/src目录下的所有.cpp文件
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

# 为每个源文件创建一个可执行文件
foreach(SRC_FILE ${SRC_FILES})
  # 获取源文件的名称（不包括路径和扩展名）
  get_filename_component(FILE_NAME ${SRC_FILE} NAME_WE)
  # 添加可执行文件
  add_executable(${FILE_NAME} ${SRC_FILE})
  # 链接库
  target_link_libraries(${FILE_NAME} fsasdk)
endforeach()

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
