# Actuator-FSA-Wiki

## Directory Structure

```shell
.
├── README.md                                                      # English README
├── README_CN.md                                                   # Chinese README
│
├── doc                                                            # Documentation
│   ├── Control_System_Introduction_CN.md                          # FSA Control System Introduction (Chinese)
│   └── images                                                     # Images
│
├── v1                                                             # Version 1 Python SDK
│   ├── config.yaml                                                # YAML Configuration File
│   ├── demo_clear_fault.py                                        # Example Script
│   ├── demo_comm_config_get.py
│   ├── ... 
│   └── pyscurve                                                   # Trajectory Planning Scripts
│
└── v2                                                             # Version 2 Python SDK
    ├── config.yaml                                                # YAML Configuration File
    ├── demo_clear_fault.py                                        # Example Script
    ├── demo_comm_config_get.py
    ├── ... 
    └── pyscurve                                                   # Trajectory Planning Scripts
```

## Getting Started

### Development Environment

`Python >= 3.10.12`

Install Python SDK V1:

```shell
pip install 'fi_fsa>=0.1.0, <0.2.0'
```

Install Python SDK V2:

```shell
pip install 'fi_fsa>=0.2.0'
```

### Clone the Repository

```shell
git clone --recursive https://gitee.com/FourierIntelligence/wiki-fsa.git -b python
```

### Communication Configuration

Connect the computer to the FSA using an Ethernet cable and set the corresponding network card to the static IP `192.168.137.1`.

> To update the FSA firmware, network sharing must be enabled.

### Running Example Scripts

Power on the FSA, navigate to the `v1` or `v2` directory, and execute the desired example script, for example:

```shell
cd v2/example # or cd v1/example
python ./demo_lookup.py
```

The `demo_lookup.py` script will scan all FSAs in the `192.168.137.xxx` subnet and print them.

> To drive the motor, a 48V DC power supply is required. For communication with the motor only, USB power is sufficient.

## Developer Guide

### FSA Control Principle

 [Control_System_Introduction_EN.md](doc/English/Control_System_Introduction_EN.md)

### API Documentation

 [V1_Python_API_Introduction_EN.md](doc/English/V1_Python_API_Introduction_EN.md)

 [V2_Python_API_Introduction_EN.md](doc/English/V2_Python_API_Introduction_EN.md)
