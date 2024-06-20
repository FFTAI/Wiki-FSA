# Actuator-FSA-Wiki

## 文件目录说明

```shell
.
├── README_CN.md									# 中文 README
├── doc												# 文档
│   ├── Control_System_Introduction_CN.md			# 控制系统介绍
│   ├── Cpp_API_Introduction_CN.md					# Cpp API 介绍
│   └── images										# 图片
│
├── v3												# v3 版本 SDK
│   ├── CMakeLists.txt
│   ├── ThirdParty									# 第三方库
│   ├── build										# 编译输出
│   ├── example										# 示例程序
│   ├── include										# 头文件
│   ├── lib											# 动态库/静态库
│   └── src											# 源文件
│
└── v4												# v4 版本 SDK
    ├── CMakeLists.txt
    ├── build										# 第三方库
    ├── config										# Json 配置文件
    ├── example										# 编译输出
    ├── include										# 头文件
    ├── lib											# 动态库/静态库
    └── src											# 源文件
```

## 上手指南

### 开发环境

`clang >= 14.0.0`

`gcc >= 11.4.0`

`GLIBC >= 2.35`

在 Ubuntu 系统（Windows 系统可使用 [WSL](https://learn.microsoft.com/zh-cn/windows/wsl/install)）中安装必要的工具和库：

```shell
sudo apt install build-essential cmake
```

### 克隆仓库

```shell
git clone --recursive https://gitee.com/FourierIntelligence/wiki-fsa.git
```

### 编译

```shell
cd wiki-fsa/v4 # 或者 cd wiki-fsa/fsa-cpp-sdk/v3
mkdir build & cd build
cmake ..
make
```

编译后 `example` 示例代码生成的可执行文件被放置在 `bin` 目录下，`libsdk_cpp_fsa.so` 动态库和 `libsdk_cpp_fsa_static.a` 静态库被放置在 `lib` 文件夹中。

### 运行示例程序

进入 `bin` 目录，执行想要运行的示例程序，例如：

```cpp
cd bin
./lookup
```

`lookup` 程序将会扫描所有 `192.168.137.xxx` 网段下的所有 FSA 并打印出来。

## 开发指南

### FSA 控制原理

 [Control_System_Introduction_CN.md](doc/Control_System_Introduction_CN.md) 

### API 文档

 [V3_Cpp_API_Introduction_CN.md](doc/V3_Cpp_API_Introduction_CN.md) 

 [V4_Cpp_API_Introduction_CN.md](doc/V4_Cpp_API_Introduction_CN.md) 
