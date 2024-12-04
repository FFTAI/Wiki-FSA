# FSA使用参考

[TOC]

## 上位机介绍

[上位机文档链接](https://alidocs.dingtalk.com/i/nodes/a9E05BDRVQbZgLMvC9ZXxqrgJ63zgkYA?doc_type=wiki_doc)

## 执行器控制模式介绍

### 电流控制模式

* 控制框图

![](FSA-电流控制模式-控制框图.drawio.svg)

* 电流模式关键API

| 接口名称                                             | 描述                     |
| ---------------------------------------------------- | ------------------------ |
| int SetCurrent( const double& cur );                 | 设置电流环目标电流       |
|                                                      | 设置最大电流             |
| int GetPVC( double& pos, double& vel, double& cur ); | 获取执行器的位置速度电流 |

### 速度控制模式

* 控制框图

![](FSA-速度控制模式-控制框图.drawio.svg)

* 速度模式关键API

  | 接口名称                                                    | 描述                      |
  | ----------------------------------------------------------- | ------------------------- |
  | int SetVelocity( const double& vel, const double& cur_ff ); | 设置目标速度$\dot{q}_d$   |
  | int SetCurrent( const double& cur );                        | 设置电流前馈$I_{q_{set}}$ |
  |                                                             | 设置最大速度$V_{max}$     |
  |                                                             | 设置最大电流$I_{q_{max}}$ |
  | int GetPVC( double& pos, double& vel, double& cur );        | 获取执行器的位置速度电流  |

### 位置控制模式



![](FSA-位置控制模式-控制框图.drawio.svg)

* 位置模式关键API

  | 接口名称                                                     | 描述                        |
  | ------------------------------------------------------------ | --------------------------- |
  | int SetPosition( const double& pos, const double& vel_ff, const double& cur_ff ); | 设置目标速度$\dot{q}_d$     |
  | int SetVelocity( const double& vel, const double& cur_ff );  | 设置速度前馈$\dot{q}_{set}$ |
  | int SetCurrent( const double& cur );                         | 设置电流前馈$I_{q_{set}}$   |
  |                                                              | 设置最大速度$V_{max}$       |
  |                                                              | 设置最大电流$I_{q_{max}}$   |
  | int GetPVC( double& pos, double& vel, double& cur );         | 获取执行器的位置速度电流    |

### PD控制模式

![](FSA-PD控制模式-控制框图.drawio.svg)

* PD模式关键API

  | 接口名称                                             | 描述                      |
  | ---------------------------------------------------- | ------------------------- |
  |                                                      | 设置位置指令$q_d$         |
  |                                                      | 设置速度指令$\dot{q}_{d}$ |
  |                                                      | 设置力矩指令$\tau_d$      |
  |                                                      | 设置最大力矩$T_{max}$     |
  |                                                      |                           |
  | int GetPVC( double& pos, double& vel, double& cur ); | 获取执行器的位置速度电流  |

### PD & 三环PD 控制参数转换

* 伺服 PD 控制（旧 PD 控制器）

$$
I_{q_{set_{j}}} = i K_{d}^{old} (K_{p}^{old} q_{err} G_{\theta} - \dot{q} G_{\omega})
$$

* 直接PD控制（新 PD 控制器）

$$
I_{q_{set_{j}}} = \frac{(K_{p}^{new} q_{err} - K_{d}^{new} \dot{q}) G_{pd}}{K_{t}}
$$

* 旧参数到新参数

$$
K_{p}^{new} = \frac{i K_{t} K_{p}^{old} K_{d}^{old} G_{\theta}}{G_{pd}}
$$

$$
K_{d}^{new} = \frac{i K_{t} K_{d}^{old} G_{\omega}}{G_{pd}}
$$

* 新参数到老参数

$$
K_{p}^{old} = \frac{K_{p}^{new} G_{\omega}}{K_{d}^{new} G_{\theta}}
$$

$$
K_{d}^{old} = \frac{K_{d}^{new} G_{pd}}{i K_{t} G_{\omega}}
$$

### 符号表

|      符号       |              值              |       单位       |                     释义                     |
| :-------------: | :--------------------------: | :--------------: | :------------------------------------------: |
|     $q_{d}$     |             变量             |    $\rm{deg}$    |                   目标位置                   |
|       $q$       |             变量             |    $\rm{deg}$    |                   实际位置                   |
|    $q_{err}$    |             变量             |    $\rm{deg}$    |                   位置误差                   |
|  $\dot{q_{d}}$  |             变量             |   $\rm{deg/s}$   |                   目标速度                   |
|    $\dot{q}$    |             变量             |   $\rm{deg/s}$   |                   实际速度                   |
| $\dot{q}_{err}$ |             变量             |   $\rm{deg/s}$   |                   速度误差                   |
|  $I_{q_{set}}$  |             变量             |     $\rm{A}$     |               目标 $Q$ 轴电流                |
|     $I_{q}$     |             变量             |     $\rm{A}$     |               实际 $Q$ 轴电流                |
|    $\tau_d$     |             变量             | $\rm{N \cdot m}$ |                   目标力矩                   |
|  $T_{\omega}$   |            $0.2$             |    $\rm{ms}$     |                速度环控制周期                |
|     $K_{t}$     |   常量（由执行器型号决定）   |        -         |               电流力矩转换系数               |
|       $i$       |   常量（由执行器型号决定）   |        -         |                    减速比                    |
|    $N_{pp}$     |   常量（由执行器型号决定）   |        -         |                    极对数                    |
|  $G_{\theta}$   |             $i$              |        -         |                 位置转换系数                 |
|  $G_{\omega}$   | $\frac{i \cdot N_{pp}}{360}$ |        -         |                 速度转换系数                 |
|    $G_{pd}$     |          $0.01745$           |        -         | $\rm{deg/s} \rightarrow \rm{rad/s}$ 转换系数 |

## 执行器状态监控

列出 CPP API

* 母线电压反馈
* MOS温度反馈
* 绕组温度反馈
* 错误码
* 执行器驱动版本号
* 执行器通讯版本号
* 执行器SDK版本号



## 执行器配置

### 控制类配置

列出 CPP API or python

* PID PD配置 立即生效 掉电重置

* PID PD配置 重启生效 掉电保存

* 最大速度、电流、力矩、加速度限幅

* PVCT反馈接口低通滤波截止频率配置

* 控制环路分频系数配置

  可以通过配置分频系数**，实现速度环、位置环、PD环的实际控制频率

* 摩擦补偿

  * 使能
  * 设置补偿增益

* 负载观测器补偿增益

### 其它配置

列出python demo 或者提示使用上位机

* 执行器方向配置
* 执行器校准
* 执行器设置零位
* 执行器启用双编配置
* 网络心跳保护配置

## 特殊模式介绍

### 摩擦补偿

![](FSA-摩擦补偿-控制框图.drawio.svg)



