# V3 Cpp API 介绍

## 初始化

```cpp
void FSA_CONNECT::FSA::init( const string& ip );
```

初始化指定 IP 地址的 FSA 通信接口

参数：

- `ip`：FSA 的 IP 地址

返回：无

## 获取配置

```cpp
int FSA_CONNECT::FSA::GetRootConfig();
```

参数：无

返回：

- SUCCESS：设置成功
- GET_ROOT_FAILED：获取 ROOT 失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 获取状态

```cpp
int FSA_CONNECT::FSA::GetState();
```

参数：无

返回：

- SUCCESS：设置成功
- GET_ROOT_FAILED：获取 ROOT 失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 使能

```cpp
int FSA_CONNECT::FSA::Enable();
```

参数：无

返回：

- SUCCESS：设置成功
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 失能

```cpp
int FSA_CONNECT::FSA::Disable();
```

参数：无

返回：

- SUCCESS：设置成功
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 设置 FSA 配置参数

```cpp
int FSA_CONNECT::FSA::SetControlConfig( const FSAConfig::FSAControlConfig& config );
```

参数：

- `config`：配置参数结构体

返回：

- SUCCESS：设置成功
- DISABLE_FAILED：设置失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 获取 FSA 配置参数

```cpp
int FSA_CONNECT::FSA::GetControlConfig();
```

参数：无

返回：

- SUCCESS：获取成功
- DISABLE_FAILED：获取失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 设置 PID 参数

```cpp
int FSA_CONNECT::FSA::SetPIDParams( FSAConfig::FSAPIDParams& pidparams );
```

参数：

- `pidparams`：PID 参数结构体

返回：

- SUCCESS：设置成功
- DISABLE_FAILED：设置失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 获取 PID 参数

```cpp
int FSA_CONNECT::FSA::GetPIDParams( FSAConfig::FSAPIDParams& pidparams );
```

参数：

- `pidparams`：PID 参数结构体

返回：

- SUCCESS：获取成功
- DISABLE_FAILED：获取失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 使能位置控制

```cpp
int FSA_CONNECT::FSA::EnablePosControl();
```

参数：无

返回：

- SUCCESS：设置成功
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 使能速度控制

```cpp
int FSA_CONNECT::FSA::EnableVelControl();
```

参数：无

返回：

- SUCCESS：设置成功
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 使能电流控制

```cpp
int FSA_CONNECT::FSA::EnableCurControl();
```

参数：无

返回：

- SUCCESS：设置成功
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行

## 设置目标位置

```cpp
int FSA_CONNECT::FSA::SetPosition( const double& pos, const double& vel_ff, const double& cur_ff );
```

参数：

- `pos`：位置指令
- `vel_ff`：速度前馈
- `cur_ff`：电流前馈

返回：

- SUCCESS：设置成功
- NOT_EXECUTE：未执行

## 设置目标速度

```cpp
int FSA_CONNECT::FSA::SetVelocity( const double& vel, const double& cur_ff );
```

参数：

- `vel`：速度指令
- `cur_ff`：电流前馈

返回：

- SUCCESS：设置成功
- NOT_EXECUTE：未执行

## 设置目标电流

```cpp
int FSA_CONNECT::FSA::SetCurrent( const double& cur );
```

参数：

- `cur`：电流指令

返回：

- SUCCESS：设置成功
- NOT_EXECUTE：未执行

## 获取实时 PVC（位置、速度、电流）

```cpp
int FSA_CONNECT::FSA::GetPVC( double& pos, double& vel, double& cur );
```

参数：

- `pos`：位置值
- `vel`：速度值
- `cur`：电流值

返回：

- SUCCESS：获取成功
- DISABLE_FAILED：获取失败
- TIMEOUT：通信超时
- NOT_EXECUTE：未执行
