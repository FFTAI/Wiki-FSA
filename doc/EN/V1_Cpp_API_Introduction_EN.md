# V3 C++ API Introduction

## Initialization

```cpp
void FSA_CONNECT::FSA::init(const string& ip);
```

Initialize the FSA communication interface for the specified IP address.

Parameters:

- `ip`: IP address of the FSA

Returns: None

## Get Configuration

```cpp
int FSA_CONNECT::FSA::GetRootConfig();
```

Parameters: None

Returns:

- `SUCCESS`: Configuration Getd successfully
- `GET_ROOT_FAILED`: Failed to get ROOT
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Get State

```cpp
int FSA_CONNECT::FSA::GetState();
```

Parameters: None

Returns:

- `SUCCESS`: State Getd successfully
- `GET_ROOT_FAILED`: Failed to get ROOT
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Enable

```cpp
int FSA_CONNECT::FSA::Enable();
```

Parameters: None

Returns:

- `SUCCESS`: Enabled successfully
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Disable

```cpp
int FSA_CONNECT::FSA::Disable();
```

Parameters: None

Returns:

- `SUCCESS`: Disabled successfully
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Set FSA Configuration Parameters

```cpp
int FSA_CONNECT::FSA::SetControlConfig(const FSAConfig::FSAControlConfig& config);
```

Parameters:

- `config`: Configuration parameter structure

Returns:

- `SUCCESS`: Configuration set successfully
- `DISABLE_FAILED`: Setting failed
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Get FSA Configuration Parameters

```cpp
int FSA_CONNECT::FSA::GetControlConfig();
```

Parameters: None

Returns:

- `SUCCESS`: Configuration Getd successfully
- `DISABLE_FAILED`: Retrieval failed
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Set PID Parameters

```cpp
int FSA_CONNECT::FSA::SetPIDParams(FSAConfig::FSAPIDParams& pidparams);
```

Parameters:

- `pidparams`: PID parameter structure

Returns:

- `SUCCESS`: Parameters set successfully
- `DISABLE_FAILED`: Setting failed
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Get PID Parameters

```cpp
int FSA_CONNECT::FSA::GetPIDParams(FSAConfig::FSAPIDParams& pidparams);
```

Parameters:

- `pidparams`: PID parameter structure

Returns:

- `SUCCESS`: Parameters Getd successfully
- `DISABLE_FAILED`: Retrieval failed
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Enable Position Control

```cpp
int FSA_CONNECT::FSA::EnablePosControl();
```

Parameters: None

Returns:

- `SUCCESS`: Position control enabled successfully
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Enable Velocity Control

```cpp
int FSA_CONNECT::FSA::EnableVelControl();
```

Parameters: None

Returns:

- `SUCCESS`: Velocity control enabled successfully
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Enable Current Control

```cpp
int FSA_CONNECT::FSA::EnableCurControl();
```

Parameters: None

Returns:

- `SUCCESS`: Current control enabled successfully
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed

## Set Target Position

```cpp
int FSA_CONNECT::FSA::SetPosition(const double& pos, const double& vel_ff, const double& cur_ff);
```

Parameters:

- `pos`: Position command
- `vel_ff`: Velocity feedforward
- `cur_ff`: Current feedforward

Returns:

- `SUCCESS`: Position set successfully
- `NOT_EXECUTE`: Not executed

## Set Target Velocity

```cpp
int FSA_CONNECT::FSA::SetVelocity(const double& vel, const double& cur_ff);
```

Parameters:

- `vel`: Velocity command
- `cur_ff`: Current feedforward

Returns:

- `SUCCESS`: Velocity set successfully
- `NOT_EXECUTE`: Not executed

## Set Target Current

```cpp
int FSA_CONNECT::FSA::SetCurrent(const double& cur);
```

Parameters:

- `cur`: Current command

Returns:

- `SUCCESS`: Current set successfully
- `NOT_EXECUTE`: Not executed

## Get Real-Time PVC (Position, Velocity, Current)

```cpp
int FSA_CONNECT::FSA::GetPVC(double& pos, double& vel, double& cur);
```

Parameters:

- `pos`: Position value
- `vel`: Velocity value
- `cur`: Current value

Returns:

- `SUCCESS`: PVC values Getd successfully
- `DISABLE_FAILED`: Retrieval failed
- `TIMEOUT`: Communication timeout
- `NOT_EXECUTE`: Not executed
