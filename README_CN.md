# Actuator-FSA-Wiki

## 目录

- [文件目录说明](#文件目录说明)
- [上手指南](#上手指南)
  - [安装步骤](#安装步骤)
  - [简要示例](#简要示例)
- [开发指南](#开发指南)
- [贡献者](#贡献者)

## 文件目录说明

## 上手指南

### 安装步骤

### 简要示例

## 开发指南

### 执行器控制原理

#### 伺服三环控制

![FSA控制框图](./doc/images/FSA%E6%8E%A7%E5%88%B6%E6%A1%86%E5%9B%BE.svg)

由于计算机控制是一种采样控制，它只能根据采样时刻的偏差计算控制量，而不能像模拟控制那样连续输出控制量，进行连续控制。由于这一特点，$PID$ 控制器中的积分项和微分项不能直接使用，必须进行离散化处理。离散化处理的方法为：以 $T$ 作为采样周期，$k$ 作为采样序号，则离散采样时间 $kT$ 对应着连续时间 $t$​，用矩形法数值积分近似代替积分，用一阶后向差分近似代替微分，可作如下近似变换：
$$
t \approx kT \left(k = 0, 1, 2, \dots \right)

\tag{1}
$$

$$
\int e(t) dt \approx T \sum_{j = 0}^{k} e(jT) = T \sum_{j = 0}^{k} e_{j}

\tag{2}
$$

$$
\frac{d}{dt} e(t) \approx \frac{e(kT) - e[(k - 1) T]}{T} = \frac{e_{k} - e_{k - 1}}{T}

\tag{3}
$$

上式中，为了表达方便，将类似于 $e(jT)$ 简化成 $e_{j}$​ 等。

##### 位置控制

$$
e_{j} = q_{d_{j}} - q_{j}
$$

$$
\dot{e}_{j} = K_{p}^{pos} e_{j} G_{\theta} - \dot{q}_{j} G_{\omega}
$$

$$
I_{q_{set_{j}}} = K_{p}^{vel} \dot{e}_{j} + T K_{i}^{vel} \sum_{j = 0}^{k} \dot{e}_{j}
$$

##### 速度控制

$$
\dot{e}_{j} = \dot{q}_{d_{j}} - \dot{q}_{j} G_{\omega}
$$

$$
I_{q_{set_{j}}} = K_{p}^{vel} \dot{e}_{j} + T K_{i}^{vel} \sum_{j = 0}^{k} \dot{e}_{j}
$$

##### 电流控制

理想情况下，用户给定 $I_{q}$ 值后 FSA 将会瞬间输出最大为 $\tau = K_{t} I_{q}$ 的力矩，此处的 $K_{t}$​ 为电流力矩系数，执行器出厂后 $K_{t}$ 值即确定。

> Tips：
>
> 只有在执行器**堵转**的情况下才能输出 $\tau = K_{t} I_{q}$ 的力矩。

#### PD 控制

$$
e_{j} = q_{d_{j}} - q_{j}
$$

$$
\tau_{j} = K_{p}^{pd} e_{j} + K_{d}^{pd} \left(0 - \dot{q}_{j} \right)
$$

#### 符号表

|     符号      |       单位       |   释义   |
| :-----------: | :--------------: | :------: |
|      $q$      |    $\rm{deg}$    | 实际位置 |
|    $q_{d}$    |    $\rm{deg}$    | 目标位置 |
|   $\dot{q}$   |   $\rm{deg/s}$   | 实际速度 |
| $\dot{q_{d}}$ |   $\rm{deg/s}$   | 目标速度 |
|    $I_{q}$    |     $\rm{A}$     | Q 轴电流 |
|    $\tau$     | $\rm{N \cdot m}$ |   力矩   |
|      $e$      |    $\rm{deg}$    | 位置误差 |
|   $\dot{e}$   |   $\rm{deg/s}$   | 速度误差 |
|      $T$      |     $\rm{s}$     | 控制周期 |



### API 介绍

#### 通信



#### 配置



#### 控制

##### 使能/失能

```cpp
// Declaration

// Enable
int Actuator::enable_set(char *ip);

// Disable
int Actuator::disable_set(char *ip);

// Example
enable_set("192.168.137.101"); // 执行器使能
disable_set("192.168.137.101"); // 执行器失能
```

##### 设置工作模式

```cpp
// Declaration
int Actuator::set_mode_of_operation(char *ip, uint8_t mode);

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
set_mode_of_operation("192.168.137.101", modeOfOper.VELOCITY_CONTROL);
```

##### 发送控制指令

```cpp
// Declaration

// Position Control
int Actuator::set_position_control(char *ip, double pos, double vel, double acc);

// Velocity Control
int Actuator::set_velocity_control(char *ip, double vel, double cur_ff);

// Current Control
int Actuator::set_current_control(char *ip, double current_val);

// PD Control
// Tips: PD 控制属于位置控制的一种，所以此处依然调用 set_position_control 函数以设定目标位置
int Actuator::set_position_control(char *ip, double pos, double vel, double acc);

// Example
set_position_control("192.168.137.101", 90, 0, 0); // 执行器输出轴逆时针旋转 90 度
set_velocity_control("192.168.137.101", 90, 0); // 执行器输出轴以 90 度每秒的速度逆时针旋转
set_current_control("192.168.137.101", 1); // 执行器输出轴在堵转时 Q 轴电流为 1 安培
```



