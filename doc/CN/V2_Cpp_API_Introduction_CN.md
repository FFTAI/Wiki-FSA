# V4 Cpp API 介绍

## 参数获取

### 获取 FSA 当前 PVC（位置、速度、电流）

```cpp
// Declaration
int Actuator::get_pvc( char* ip, double& pos, double& vel, double& cur );

// Example
double pos = 0, vel = 0, cur = 0;
get_pvc( "192.168.137.101", pos, vel, cur ); // 将 FSA 当前的位置、速度和电流数据存入 pos, vel 和 cur
```

### 获取三环 PID 参数

```cpp
// Declartion
int Actuator::pid_param_get( char* ip, rapidjson::Document* config_buffer );

// Example
rapidjson::Document pid_json;
pid_param_get( "192.168.137.101", &pid_json );
```

### 获取 PD 参数

```cpp

```

### 获取 FSA 参数

```cpp
// Declaration
int Actuator::ctrl_config_get( char* ip, rapidjson::Document* config_buffer );

// Example
rapidjson::Document ctrl_config_json;
ctrl_config_get( "192.168.137.101", &ctrl_config_json );
```

### 获取 FSA 限制参数

```cpp
// Declaration
int Actuator::control_param_get( char* ip, float& vel, float& acc, float& cur );

// Example
float motor_max_speed, motor_max_acc, max_cur;
control_param_get( "192.168.137.101", motor_max_speed, motor_max_acc, max_cur );
```

### 获取通信配置

```cpp
// Declaration
int Actuator::comm_config_get( char* ip, rapidjson::Document* config_buffer );

// Example
rapidjson::Document msg_json;
comm_config_get( "192.168.137.101", &msg_json );
```

### 获取操作标志位

```cpp
// Declaration
int Actuator::flag_of_operation_get( char* ip, bool& act_val, bool& motor_val, bool& encoder_val, bool& pid_val );

// Example
bool act_val, motor_val, encoder_val, pid_val;
flag_of_operation_get( "192.168.137.101", act_val, motor_val, encoder_val, pid_val );
```

## 参数设置

### 设置三环 PID 参数

```cpp
// Declaration

// 永久设置 PID 参数（执行器断电后不丢失）
int Actuator::pid_param_set( char* ip, double pos_kp, double vel_kp, double vel_ki, double cur_kp, double cur_ki );

// 临时设置 PID 参数（执行器断电后重置为上一次永久设置的 PID 参数）
int Actuator::pid_param_imm_set( char* ip, double pos_imm_kp, double vel_imm_kp, double vel_imm_ki, double cur_imm_kp, double cur_imm_ki );

// Example
pid_param_set( "192.168.137.101", 0.001, 0.001, 0.001, 0, 0 );
pid_param_imm_set( "192.168.137.101", 0.001, 0.001, 0.001, 0, 0 );
```

> 注：电流环 PI 参数暂时无法自定义。

### 设置 PD 参数

```cpp

```

### 设置 FSA 参数

```cpp
// Declaration
int Actuator::ctrl_config_set( char* ip, uint64_t act_type, int dir, int redTat, int motor_type, int hardware_type, int vbus, int motor_dir, int pole_pairs, int max_speed, int max_acc, int encoder_dir );

// FSA 型号
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

// FSA 方向
class FSAActuatorDirection {
public:
    int DIRECTION_NORMAL  = 1;
    int DIRECTION_REVERSE = -1;
};

// 减速比
class FSAActuatorReductionRatio {
public:
    int REDUCTION_RATIO_7   = 7;
    int REDUCTION_RATIO_30  = 30;
    int REDUCTION_RATIO_50  = 50;
    int REDUCTION_RATIO_70  = 70;
    int REDUCTION_RATIO_100 = 100;
    int REDUCTION_RATIO_120 = 120;
};

// 电机型号
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

// 硬件型号
class FSAHardwareType {
public:
    int TYPE_HARDWARE_NULL = 0;
    int TYPE_ODrive_V36    = 1;
    int TYPE_H95V104       = 2;
    int TYPE_H66V104       = 3;
    int TYPE_H46V104       = 4;
    int TYPE_H30V303       = 5;
};

// 母线电压
class FSAMotorVBUS {
public:
    int VBUS_36V = 36;
    int VBUS_48V = 48;
};

// 电机方向
class FSAMotorDirection {
public:
    int ABC = 1;
    int ACB = -1;
};

// 电机极对数
class FSAMotorPolePairs {
public:
    int POLE_PAIRS_7  = 7;
    int POLE_PAIRS_10 = 10;
    int POLE_PAIRS_21 = 21;
};

// FSA 最大转速
class FSAMotorMaxSpeed {
public:
    int MAX_SPEED_1000 = 1000;  // rpm
    int MAX_SPEED_2000 = 2000;  // rpm
    int MAX_SPEED_3000 = 3000;  // rpm
};

// FSA 最大加速度
class FSAMotorMaxAcceleration {
public:
    int MAX_ACCELERATION_6000  = 6000;   // rpm/s
    int MAX_ACCELERATION_10000 = 10000;  // rpm/s
    int MAX_ACCELERATION_30000 = 30000;  // rpm/s
    int MAX_ACCELERATION_60000 = 60000;  // rpm/s
};

// 编码器方向
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

### 设置 FSA 限制参数

```cpp
// Declaration

// 永久设置 FSA 限制参数（执行器断电后不丢失）
int Actuator::control_param_set( char* ip, float vel, float acc, float cur );

// 临时设置 FSA 限制参数（执行器断电后重置为上一次永久设置的 FSA 限制参数）
int Actuator::control_param_imm_set( char* ip, float vel, float acc, float cur );

// Example
control_param_set( "192.168.137.101", 3000, 60000, 8 ); //限制 FSA 最大转速为 3000RPM，最大加速度 60000RPM/s，最大 Q 轴电流为 8A
```

### 设置通信配置

```cpp
// Declaration
int Actuator::comm_config_set( char* ip, std::string name, bool dhcp_enable, std::string ssid, std::string password, uint8_t static_ip );

// Example
comm_config_set( "192.168.137.101", "FSA", false, "fftai-12", "fftai2015", 102 );
```

### 设置操作标志位

```cpp
// Declaration
int Actuator::flag_of_operation_set( char* ip, int act_val, int motor_val, int encoder_val, int pid_val );

// 标志位
class FSAFlagState {
public:
    int CLEAR = 0;
    int SET   = 1;
};

// Example
FSAFlagState flagState;
flag_of_operation_set("192.168.137.101", flagState.CLEAR, flagState.SET, flagState.CLEAR, flagState.SET ); // 将电机和 PID 的保存参数标志位开启
```

## 运动控制

### 检索可连接 FSA

```cpp
// Declaration
int Actuator::lookup( char* ip_ser, int& ser_num );

// Example
std::string ser_list[ 254 ] = { "" };
int         ip_num          = 0;
lookup( ( char* )ser_list, ip_num ); // lookup 函数会将扫描到的 IP 地址和 IP 数量分别添加到 ser_list 和ip_num 中
```

### 使能/失能

```cpp
// Declaration

// Enable
int Actuator::enable_set( char *ip );

// Disable
int Actuator::disable_set( char *ip );

// Example
enable_set( "192.168.137.101" ); // 执行器使能
disable_set( "192.168.137.101" ); // 执行器失能
```

### 设置工作模式

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

### 发送控制指令

```cpp
// Declaration

// Position Control
int Actuator::set_position_control( char *ip, double pos, double vel, double acc );

// Velocity Control
int Actuator::set_velocity_control( char *ip, double vel, double cur_ff );

// Current Control
int Actuator::set_current_control( char *ip, double current_val );

// PD Control
// Tips: PD 控制器输入参数为位置，所以此处依然调用 set_position_control 函数以设定目标位置
int Actuator::set_position_control( char *ip, double pos, double vel, double acc );

// Example
set_position_control( "192.168.137.101", 90, 0, 0 ); // 执行器输出轴逆时针旋转 90 度
set_velocity_control( "192.168.137.101", 90, 0 ); // 执行器输出轴以 90 度每秒的速度逆时针旋转
set_current_control( "192.168.137.101", 1 ); // 执行器输出轴在堵转时 Q 轴电流为 1 安培
```

### 重启 FSA

```cpp
// Declaration
int Actuator::reboot( char* ip );

// Example
reboot( "192.168.137.101" );
```

### 编码器校准

```cpp
// Declaration
int Actuator::set_calibrate_encoder( char* ip );

// Example
set_calibrate_encoder( "192.168.137.101" );
```
