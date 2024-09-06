# V4 Cpp API Introduction

## Parameter Retrieval

### Get Current PVC (Position, Velocity, Current) of the FSA

```cpp
// Declaration
int Actuator::get_pvc( char* ip, double& pos, double& vel, double& cur );

// Example
double pos = 0, vel = 0, cur = 0;
get_pvc( "192.168.137.101", pos, vel, cur ); // Store the current position, velocity, and current of the FSA into pos, vel, and cur
```

### Get PID Parameters

```cpp
// Declaration
int Actuator::pid_param_get( char* ip, rapidjson::Document* config_buffer );

// Example
rapidjson::Document pid_json;
pid_param_get( "192.168.137.101", &pid_json );
```

### Get PD Parameters

```cpp

```

### Get FSA Parameters

```cpp
// Declaration
int Actuator::ctrl_config_get( char* ip, rapidjson::Document* config_buffer );

// Example
rapidjson::Document ctrl_config_json;
ctrl_config_get( "192.168.137.101", &ctrl_config_json );
```

### Get FSA Limiting Parameters

```cpp
// Declaration
int Actuator::control_param_get( char* ip, float& vel, float& acc, float& cur );

// Example
float motor_max_speed, motor_max_acc, max_cur;
control_param_get( "192.168.137.101", motor_max_speed, motor_max_acc, max_cur );
```

### Get Communication Configuration

```cpp
// Declaration
int Actuator::comm_config_get( char* ip, rapidjson::Document* config_buffer );

// Example
rapidjson::Document msg_json;
comm_config_get( "192.168.137.101", &msg_json );
```

### Get Operation Flags

```cpp
// Declaration
int Actuator::flag_of_operation_get( char* ip, bool& act_val, bool& motor_val, bool& encoder_val, bool& pid_val );

// Example
bool act_val, motor_val, encoder_val, pid_val;
flag_of_operation_get( "192.168.137.101", act_val, motor_val, encoder_val, pid_val );
```

## Parameter Setting

### Set PID Parameters

```cpp
// Declaration

// Permanently set PID parameters (retained after actuator power cycle)
int Actuator::pid_param_set( char* ip, double pos_kp, double vel_kp, double vel_ki, double cur_kp, double cur_ki );

// Temporarily set PID parameters (reset to last permanent settings after power cycle)
int Actuator::pid_param_imm_set( char* ip, double pos_imm_kp, double vel_imm_kp, double vel_imm_ki, double cur_imm_kp, double cur_imm_ki );

// Example
pid_param_set( "192.168.137.101", 0.001, 0.001, 0.001, 0, 0 );
pid_param_imm_set( "192.168.137.101", 0.001, 0.001, 0.001, 0, 0 );
```

> Note: Current loop PI parameters cannot be customized at this time.

### Set PD Parameters

```cpp

```

### Set FSA Parameters

```cpp
// Declaration
int Actuator::ctrl_config_set( char* ip, uint64_t act_type, int dir, int redTat, int motor_type, int hardware_type, int vbus, int motor_dir, int pole_pairs, int max_speed, int max_acc, int encoder_dir );

// FSA Types
class FSAActuatorType {
public:
    uint64_t TYPE_DEFAULT     = 0x00000001;
    uint64_t TYPE_25_10_C_1   = 0x01010301;
    uint64_t TYPE_25_10_C_30  = 0x01010303;
    uint64_t TYPE_25_10_D_1   = 0x01010401;
    uint64_t TYPE_25_10_D_FSA = 0x02010403;
    uint64_t TYPE_60_10_D_1   = 0x03010401;
    uint64_t TYPE_60_10_D_120 = 0x03010405;
    uint64_t TYPE_80_21_C_1   = 0x04020301;
    uint64_t TYPE_80_21_C_30  = 0x04020303;
    uint64_t TYPE_100_21_A_1  = 0x05020101;
    uint64_t TYPE_100_21_A_7  = 0x05020102;
    uint64_t TYPE_130_21_A_1  = 0x06020101;
    uint64_t TYPE_130_21_A_7  = 0x06020102;
};

// FSA Directions
class FSAActuatorDirection {
public:
    int DIRECTION_NORMAL  = 1;
    int DIRECTION_REVERSE = -1;
};

// Reduction Ratios
class FSAActuatorReductionRatio {
public:
    int REDUCTION_RATIO_7   = 7;
    int REDUCTION_RATIO_30  = 30;
    int REDUCTION_RATIO_50  = 50;
    int REDUCTION_RATIO_70  = 70;
    int REDUCTION_RATIO_100 = 100;
    int REDUCTION_RATIO_120 = 120;
};

// Motor Types
class FSAMotorType {
public:
    int TYPE_MOTOR_NULL = 0;
    int TYPE_JIOALONG   = 1;
    int FSA130_20V0     = 2;
    int FSA100_15V0     = 3;
    int FSA80_10V0      = 4;
    int FSA60_08V0      = 5;
    int FSA36_08V0      = 6;
    int FSA25_08V0      = 7;
    int FSA36_10V0      = 8;
};

// Hardware Types
class FSAHardwareType {
public:
    int TYPE_HARDWARE_NULL = 0;
    int TYPE_ODrive_V36    = 1;
    int TYPE_H95V104       = 2;
    int TYPE_H66V104       = 3;
    int TYPE_H46V104       = 4;
    int TYPE_H30V303       = 5;
};

// Bus Voltages
class FSAMotorVBUS {
public:
    int VBUS_36V = 36;
    int VBUS_48V = 48;
};

// Motor Directions
class FSAMotorDirection {
public:
    int ABC = 1;
    int ACB = -1;
};

// Motor Pole Pairs
class FSAMotorPolePairs {
public:
    int POLE_PAIRS_7  = 7;
    int POLE_PAIRS_10 = 10;
    int POLE_PAIRS_21 = 21;
};

// Maximum Speeds
class FSAMotorMaxSpeed {
public:
    int MAX_SPEED_1000 = 1000;  // rpm
    int MAX_SPEED_2000 = 2000;  // rpm
    int MAX_SPEED_3000 = 3000;  // rpm
};

// Maximum Accelerations
class FSAMotorMaxAcceleration {
public:
    int MAX_ACCELERATION_6000  = 6000;   // rpm/s
    int MAX_ACCELERATION_10000 = 10000;  // rpm/s
    int MAX_ACCELERATION_30000 = 30000;  // rpm/s
    int MAX_ACCELERATION_60000 = 60000;  // rpm/s
};

// Encoder Directions
class FSAEncoderDirection {
public:
    int DIRECTION_CW  = -1;
    int DIRECTION_CCW = 1;
};

// Example
FSAActuatorType           actuatorType;
FSAActuatorDirection      actuatorDir;
FSAActuatorReductionRatio actuatorRedTat;
FSAMotorType              motorType;
FSAHardwareType           hardwareType;
FSAMotorVBUS              motorBus;
FSAMotorDirection         motorDir;
FSAMotorPolePairs         motorPolePairs;
FSAMotorMaxSpeed          motorMaxSpeed;
FSAMotorMaxAcceleration   motorMaxAcc;
FSAEncoderDirection       encoderDir;

ctrl_config_set( "192.168.137.101", actuatorType.TYPE_DEFAULT, actuatorDir.DIRECTION_NORMAL, actuatorRedTat.REDUCTION_RATIO_30, motorType.FSA80_10V0, hardwareType.TYPE_H66V104, motorBus.VBUS_36V, motorDir.ACB, motorPolePairs.POLE_PAIRS_10, motorMaxSpeed.MAX_SPEED_3000, motorMaxAcc.MAX_ACCELERATION_60000, encoderDir.DIRECTION_CCW );
```

### Set FSA Limiting Parameters

```cpp
// Declaration

// Permanently set FSA limiting parameters (retained after actuator power cycle)
int Actuator::control_param_set( char* ip, float vel, float acc, float cur );

// Temporarily set FSA limiting parameters (reset to last permanent settings after power cycle)
int Actuator::control_param_imm_set( char* ip, float vel, float acc, float cur );

// Example
control_param_set( "192.168.137.101", 3000, 60000, 8 ); // Limit FSA maximum speed to 3000 RPM, maximum acceleration to 60000 RPM/s, and maximum Q-axis current to 8A
```

### Set Communication Configuration

```cpp
// Declaration
int Actuator::comm_config_set( char* ip, std::string name, bool dhcp_enable, std::string ssid, std::string password, uint8_t static_ip );

// Example
comm_config_set( "192.168.137.101", "FSA", false, "fftai-12", "fftai2015", 102 );
```

### Set Operation Flags

```cpp
// Declaration
int Actuator::flag_of_operation_set( char* ip, int act_val, int motor_val, int encoder_val, int pid_val );

// Flags
class FSAFlagState {
public:
    int CLEAR = 0;
    int SET   = 1;
};

// Example
FSAFlagState flagState;
flag_of_operation_set( "192.168.137.101", flagState.CLEAR, flagState.SET, flagState.CLEAR, flagState.SET ); // Enable the motor and PID parameter save flags
```

## Motion Control

### Lookup Available FSAs

```cpp
// Declaration
int Actuator::lookup( char* ip_ser, int& ser_num );

// Example
std::string ser_list[ 254 ] = { "" };
int         ip_num          = 0;
lookup( ( char* )ser_list, ip_num ); // The lookup function will add the scanned IP addresses and the number of IPs to ser_list and ip_num respectively
```

### Enable/Disable

```cpp
// Declaration

// Enable
int Actuator::enable_set( char *ip );

// Disable
int Actuator::disable_set( char *ip );

// Example
enable_set( "192.168.137.101" ); // Enable the actuator
disable_set( "192.168.137.101" ); // Disable the actuator
```

### Set Operation Mode

```cpp
// Declaration
int Actuator::set_mode_of_operation( char *ip, uint8_t mode );

class FsaModeOfOperation {
public:
    int NONE = 0;
    int CURRENT_CLOSE_LOOP_CONTROL = 4;
    int VELOCITY_CONTROL = 3;
    int POSITION_CONTROL = 1;
    int TRAPEZOIDAL_CONTROL = 5;
};

// Example
FsaModeOfOperation modeOfOper;
set_mode_of_operation( "192.168.137.101", modeOfOper.VELOCITY_CONTROL );
```

### Send Control Commands

```cpp
// Declaration

// Position Control
int Actuator::set_position_control( char *ip, double pos, double vel, double acc );

// Velocity Control
int Actuator::set_velocity_control( char *ip, double vel, double cur_ff );

// Current Control
int Actuator::set_current_control( char *ip, double current_val );

// PD Control
// Tips: PD controller input parameter is position, so the set_position_control function is still called here to set the target position
int Actuator::set_position_control( char *ip, double pos, double vel, double acc );

// Example
set_position_control( "192.168.137.101", 90, 0, 0 ); // Actuator output shaft rotates counterclockwise by 90 degrees
set_velocity_control( "192.168.137.101", 90, 0 ); // Actuator output shaft rotates counterclockwise at 90 degrees per second
set_current_control( "192.168.137.101", 1 ); // Actuator output shaft has a Q-axis current of 1 amp when stalled
```

### Reboot FSA

```cpp
// Declaration
int Actuator::reboot( char* ip );

// Example
reboot( "192.168.137.101" );
```

### Encoder Calibration

```cpp
// Declaration
int Actuator::set_calibrate_encoder( char* ip );

// Example
set_calibrate_encoder( "192.168.137.101" );
```
