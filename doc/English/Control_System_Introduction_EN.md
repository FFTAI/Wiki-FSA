# Control System Introduction

## Actuator Control Principle

### Servo Three-loop Control

Computer control is a form of sampled control; it can only compute control actions based on the sampled deviation at specific moments, unlike analog control which can output control actions continuously. Due to this characteristic, the integral and differential terms in the $PID$ controller cannot be used directly and must be discretized. The discretization method is as follows: with $T$ as the sampling period and $k$ as the sampling index, the discrete sampling time $kT$ corresponds to continuous time $t$. Using the rectangle method for numerical integration and backward difference for differentiation, the transformations are approximated as:

$$
t \approx kT \left(k = 0, 1, 2, \dots \right)
$$

$$
\int e(t) dt \approx T \sum_{j = 0}^{k} e(jT) = T \sum_{j = 0}^{k} e_{j}
$$

$$
\frac{d}{dt} e(t) \approx \frac{e(kT) - e[(k - 1) T]}{T} = \frac{e_{k} - e_{k - 1}}{T}
$$

For simplicity, terms like $e(jT)$ are expressed as $e_{j}$ in the equations above.

#### Position Control

![FSA Control Diagram - Position Control](../images/FSA%E6%8E%A7%E5%88%B6%E6%A1%86%E5%9B%BE-%E4%BD%8D%E7%BD%AE%E6%8E%A7%E5%88%B6.svg)

$$
e_{j} = q_{d_{j}} - q_{j}
$$

$$
\dot{e}_{j} = K_{p}^{pos} e_{j} G_{\theta} - \dot{q}_{j} G_{\omega}
$$

$$
I_{q_{set_{j}}} = K_{p}^{vel} \dot{e}_{j} + T_{\omega} K_{i}^{vel} \sum_{j = 0}^{k} \dot{e}_{j}
$$

#### Velocity Control

![FSA Control Diagram - Velocity Control](../images/FSA%E6%8E%A7%E5%88%B6%E6%A1%86%E5%9B%BE-%E9%80%9F%E5%BA%A6%E6%8E%A7%E5%88%B6.svg)

$$
\dot{e}_{j} = \dot{q}_{d_{j}} - \dot{q}_{j} G_{\omega}
$$

$$
I_{q_{set_{j}}} = K_{p}^{vel} \dot{e}_{j} + T_{\omega} K_{i}^{vel} \sum_{j = 0}^{k} \dot{e}_{j}
$$

#### Current Control

![FSA Control Diagram - Current Control](../images/FSA%E6%8E%A7%E5%88%B6%E6%A1%86%E5%9B%BE-%E7%94%B5%E6%B5%81%E6%8E%A7%E5%88%B6.svg)

Ideally, after the user sets the $I_{q}$ value, the FSA will instantaneously output a maximum torque of $\tau = K_{t} I_{q}$, where $K_{t}$ is the current-torque coefficient, determined at the time of actuator manufacture.

> Tips:
>
> The torque $\tau = K_{t} I_{q}$ can only be output when the actuator is **stalled**.

### PD Control

![FSA Control Diagram - PD Control](../images/FSA%E6%8E%A7%E5%88%B6%E6%A1%86%E5%9B%BE-PD%E6%8E%A7%E5%88%B6.svg)

$$
e_{j} = q_{d_{j}} - q_{j}
$$

$$
\dot{e_{j}} = 0 - \dot{q_{j}}
$$

$$
\tau_{j} = K_{p}^{pd} e_{j} + K_{d}^{pd} \dot{e}_{j}
$$

## Symbol Table

|     Symbol      |              Value               |       Unit       |                      Description                       |
| :-------------: | :------------------------------: | :--------------: | :---------------------------------------------------: |
|    $q_{d}$      |       Variable (user-defined)    |    $\rm{deg}$    |                  Target Position                      |
|      $q$        |             Variable             |    $\rm{deg}$    |                  Actual Position                      |
|      $e$        |             Variable             |    $\rm{deg}$    |                  Position Error                       |
| $\dot{q_{d}}$   |       Variable (user-defined)    |   $\rm{deg/s}$   |                  Target Velocity                      |
|   $\dot{q}$     |             Variable             |   $\rm{deg/s}$   |                  Actual Velocity                      |
|   $\dot{e}$     |             Variable             |   $\rm{deg/s}$   |                  Velocity Error                       |
| $I_{q_{set}}$   |             Variable             |     $\rm{A}$     |             Target $Q$-axis Current                   |
|    $I_{q}$      |             Variable             |     $\rm{A}$     |             Actual $Q$-axis Current                   |
|    $\tau$       |             Variable             | $\rm{N \cdot m}$ |                  Target Torque                        |
| $T_{\omega}$    |              $0.2$               |    $\rm{ms}$     |            Velocity Loop Control Period               |
|    $K_{t}$      |   Constant (actuator-dependent)  |        -         |              Current-torque Coefficient               |
|      $i$        |   Constant (actuator-dependent)  |        -         |                    Reduction Ratio                    |
|   $N_{pp}$      |   Constant (actuator-dependent)  |        -         |                    Pole Pairs                         |
| $G_{\theta}$    |              $i$                 |        -         |                  Position Conversion Coefficient     |
| $G_{\omega}$    | $\frac{i \cdot N_{pp}}{360}$     |        -         |                  Velocity Conversion Coefficient     |
|   $G_{pd}$      |           $0.01745$              |        -         | $\rm{deg/s} \rightarrow \rm{rad/s}$ Conversion Coefficient |
