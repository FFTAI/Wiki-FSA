# Actuator-FSA-Wiki

## Directory Structure

```shell
.
├── README.md                   # English README
├── README_CN.md                # Chinese README
│
├── doc/                        # Documentation
│   ├── CN/                     # Chinese Documentation
│   ├── EN/                     # English Documentation
│   └── images/                 # Images
│
├── v1/                         # Cpp SDK V1
│   ├── CMakeLists.txt
│   ├── ThirdParty/             # Third-Party Libraries
│   ├── build/                  # Build Output
│   ├── example/                # Example Programs
│   ├── include/                # Header Files
│   ├── lib/                    # Dynamic/Static Libraries
│   └── src/                    # Source Files
│
└── v2/                         # Cpp SDK V2
    ├── CMakeLists.txt
    ├── build/                  # Build Output
    ├── config/                 # JSON Configuration Files
    ├── example/                # Example Programs
    ├── include/                # Header Files
    ├── lib/                    # Dynamic/Static Libraries
    └── src/                    # Source Files
```

## Getting Started

### Development Environment

`clang >= 14.0.0` or `gcc >= 11.4.0`

`GLIBC >= 2.35`

Install the necessary tools and libraries on Ubuntu (for Windows, use [WSL](https://learn.microsoft.com/en-us/windows/wsl/install)):

```shell
sudo apt install build-essential cmake
```

### Cloning the Repository

```shell
git clone https://gitee.com/FourierIntelligence/wiki-fsa.git -b cpp
```

### Building

```shell
cd wiki-fsa/v1 # or cd wiki-fsa/fsa-cpp-sdk/v2
mkdir build & cd build
cmake ..
make
```

After building, the executable files generated from the `example` code will be placed in the `bin` directory. The dynamic library `libsdk_cpp_fsa.so` and the static library `libsdk_cpp_fsa_static.a` will be placed in the `lib` folder.

### Running Example Programs

Navigate to the `bin` directory and execute the example program you want to run. For example:

```cpp
cd bin
./lookup
```

The `lookup` program will scan and list all FSAs in the `192.168.137.xxx` subnet.

> To drive the motor, a 48V DC power supply is required. For communication with the motor only, USB power is sufficient.

## Development Guide

### FSA Control Principles

 [Control_System_Introduction_EN.md](doc/EN/Control_System_Introduction_EN.md) 

### API Documentation

 [V1_Cpp_API_Introduction_EN.md](doc/EN/V1_Cpp_API_Introduction_EN.md) 

 [V2_Cpp_API_Introduction_EN.md](doc/EN/V2_Cpp_API_Introduction_EN.md)
