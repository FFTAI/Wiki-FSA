#pragma once

#ifdef WIN32
#include <WS2tcpip.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#define DllExport __declspec(dllexport)
#else
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#endif

#include <time.h>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <regex>
#include <string>
#include <thread>
#include <vector>

#include "aios.h"
#include "json.hpp"
#include "log.h"
#include "uv.h"

#ifdef _WIN32
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "Userenv.lib")
#endif

constexpr auto MOTOR_POSITION_INACCURACY = 200;

// #define StartTimeChrono(funName)                                \
//   std::chrono::milliseconds ms##funName =                       \
//       std::chrono::duration_cast<std::chrono::milliseconds>(    \
//           std::chrono::system_clock::now().time_since_epoch()); \
//   long start##funName = ms##funName.count();

// #define EndTimeChrono(funName)                                         \
//   ms##funName = std::chrono::duration_cast<std::chrono::milliseconds>( \
//       std::chrono::system_clock::now().time_since_epoch());            \
//   long end##funName = ms##funName.count();                             \
//   std::cout << "The function " << #funName << " runs for "             \
//             << end##funName - start##funName << "ms" << std::endl;

#define StartTimeChrono(funName)                                \
  std::chrono::microseconds ms##funName =                       \
      std::chrono::duration_cast<std::chrono::microseconds>(    \
          std::chrono::system_clock::now().time_since_epoch()); \
  long start##funName = ms##funName.count();

#define EndTimeChrono(funName)                                                \
  ms##funName = std::chrono::duration_cast<std::chrono::microseconds>(        \
      std::chrono::system_clock::now().time_since_epoch());                   \
  long end##funName = ms##funName.count();                                    \
  double ms_time = (end##funName - start##funName) / 1000.0;                  \
  std::cout << "The function " << #funName << " runs for " << ms_time << "ms" \
            << std::endl;

using vStrPtr = std::shared_ptr<std::vector<std::string>>;
using strMapPtr = std::shared_ptr<std::map<std::string, std::string>>;

typedef struct {
  std::string name;
  std::string serial_number;
} cfgStc;

struct EntryInfo {
  std::string name;
  std::string family;
  std::string serial_number;
  std::string ip;
  std::string mac_address;
  std::string fw_version;

  bool operator<(const EntryInfo &rhs) const {  // 升序排序时必须写的函数
    return name < rhs.name;
  }
};

namespace Fourier {
typedef struct PosPtInfoActual_ {
  float pos;
  short vel;
  short torque;
} PosPtInfoActual;

struct VelPtInfo {
  float vel;
  float torque;
};

struct TorquePtInfo {
  float torque;
};

struct CvpPtFeedback {
  float pos;
  float vel;
  float torque;
  int error_code;
};

enum class FwVersion {
  FW_1,
  FW_2,
};

#define PORT_SER \
  2334  // Low priority service data port. ie, parameter setting and reading
#define PORT_RT \
  2333  // Real-time control data port, ie. speed,position, current and other
        // real-time data
#define PORT_PT 10000  // Passthrough port
#define TIME_OUT 500

extern uv_loop_t *g_loop;

class Broadcast_ {
 public:
  Broadcast_(std::string ip = "255.255.255.255") : network_(ip){};
  ~Broadcast_();

  int constructionSocket();

  int broadcast_func(strMapPtr ipAndActuatorInfo, int timeout = TIME_OUT);

  std::string network_;

  uv_udp_t send_socket_;
  uv_udp_t recv_socket_;
  struct sockaddr_in send_addr_;
  struct sockaddr_in recv_addr_;
  uv_timer_t timer_;
};

class Broadcast {
 public:
  Broadcast(std::string ip = "255.255.255.255") : network_(ip){};
  ~Broadcast(){};

  /**
   * @brief 构造socket
   * @param timeout_ 设置超时,单位ms
   * @return 成功返回0，其他返回-1
   */
  int constructionSocket(int timeout = timeout_);

  /**
   * @brief 设置addr和port
   * @param ip ip地址
   * @param port 端口
   * @return 成功返回0，其他返回-1
   */
  int setSockaddr(const std::string ip);

  /**
   * @brief 通过广播获取所有在线server ip存储到addrist中
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode broadcast_func(strMapPtr ipAndActuatorInfo);

  // std::shared_ptr<std::map<std::string, std::vector<cfgStc>>> getCfgInfo()
  // const {
  //     return g_cfg;
  // }

  FourierStatusCode sendData(const std::string &send_buf,
                             struct sockaddr_in sockaddr,
                             bool print_flag = true);
  FourierStatusCode sendData(const unsigned char *send_buf, int str_len,
                             struct sockaddr_in sockaddr,
                             bool print_flag = true);
  FourierStatusCode sendBroadcastData(const std::string &send_buf,
                                      bool print_flag = true);
  /**
   * @brief 通过UDP向server发送数据
   * @param send_buf 要发送的数据
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode sendServData(const std::string &send_buf,
                                 bool print_flag = true);

  /**
   * @brief 通过UDP向server发送数据
   * @param send_buf 要发送的数据
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode sendRtData(const std::string &send_buf,
                               bool print_flag = true);

  /**
   * @brief 通过UDP向server发送数据
   * @param send_buf 要发送的数据
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode sendPtData(const std::string &send_buf,
                               bool print_flag = true);

  // fw2.0
  FourierStatusCode sendPtData(const unsigned char *send_buf, const int str_len,
                               const bool print_flag = false);

  /**
   * @brief 通过UDP接收server返回的数据
   * @param recv_buf 如果接收数据成功，将数据写入到该参数
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode recvServData(char *recv_buf, bool print_flag = true);

  FourierStatusCode recvRtData(char *recv_buf, bool print_flag = true);

  FourierStatusCode recvPtData(char *recv_buf, bool print_flag = true,
                               std::string *recv_ip = nullptr);

  // fw2.0
  FourierStatusCode recvPtData(std::shared_ptr<CvpPtFeedback> cvp_pt_feedback);

  /**
   * @brief 根据入参设置socket，然后发送data，最后接受data
   * @param data 要发送的json数据
   * @param serverIp 目标server ip
   * @param PORT_srv 目标端口号
   * @param recv_buf 接收数据写回recv_buf
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode runServSocket(const nlohmann::json &data, char *recv_buf,
                                  bool print_flag = true);

  FourierStatusCode runRtSocket(const nlohmann::json &data, char *recv_buf,
                                bool print_flag = true);

  const std::string getIp() const { return network_; }

 public:
  static float frequency_;  // 广播的默认频率 HZ
  static const int recv_buf_len_ = 512;
  static const int recv_buf_pt_len_ = 17;
  char recv_rt_buf_[recv_buf_len_];
  char recv_pt_buf_[recv_buf_pt_len_];

 protected:
  // Low priority service data port. ie, parameter setting and reading
  static const int PORT_srv = 2334;

  // Real-time control data port, ie. speed,position, current and other
  // real-time data
  static const int PORT_rt = 2333;

  static const int PORT_pt = 10000;  // Passthrough port

  static const int timeout_ = 500;  // 默认超时时间,单位ms

  std::string network_;

  /**
   * socket文件描述符
   */
#ifdef WIN32
  SOCKET sockfd_;
#else
  int sockfd_;
  int sockfd_serv_;
  int sockfd_rt_;
  int sockfd_pt_;
#endif

  /**
   * 套接字结构体、套接字结构体大小
   */
  struct sockaddr_in sockaddr_broadcast_;
  struct sockaddr_in sockaddr_serv_;
  struct sockaddr_in sockaddr_rt_;
  struct sockaddr_in sockaddr_pt_;
  int sockaddr_len_;

  static const int inet_buf_len_ = 16;
};

class Aios : public Broadcast {
 public:
  Aios(std::string ip);
  ~Aios();

  /**
   * @brief AIOS获取根属性
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getRoot(char *recv_buf = nullptr, bool print_flag = true);

  /**
   * @brief 判断电机是否存在
   * @param motor_number 电机编号
   * @return 存在返回 true，其他返回false
   */
  bool isMotorNumberInVec(const int motor_number);

  /**
   * @brief 输出PVC值
   * @param buf 要输出的buf
   * @return None
   */
  void printPVC(const char *buf);

  /**
   * @brief AIOS enable
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  int enable(const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Disable
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  int disable(const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief ctrlBox enable
   * @return 成功返回0，其他返回-1
   */
  int ctrlBoxEnable();

  /**
   * @brief ctrlBox Disable
   * @return 成功返回0，其他返回-1
   */
  int ctrlBoxDisable();

  /**
   * @brief AIOS Get current status
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  int getState(AxisState *axis_state, const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Get Root Config property
   * @return 成功返回0，其他返回-1
   */
  int getRootConfig();

  /**
   * @brief AIOS set Root Config properties
   * @param configParam 配置参数
   * @return 成功返回0，其他返回-1
   */
  int setRootConfig(const nlohmann::json &configParam);

  /**
   * @brief AIOS save configuration
   * @return 成功返回0，其他返回-1
   */
  int saveConfig();

  /**
   * @brief AIOS clear configuration
   * @return 成功返回0，其他返回-1
   */
  int eraseConfig();

  /**
   * @brief AIOS restart
   * @return 成功返回0，其他返回-1
   */
  int reboot();

  /**
   * @brief AIOS restart the motor drive
   * @return 成功返回0，其他返回-1
   */
  int rebootMotorDrive();

  /**
   * @brief AIOS OTA upgrade
   * @return 成功返回0，其他返回-1
   */
  int OTAupdate();

  /**
   * @brief AIOS Get error code
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getError(char *recv_buf = nullptr,
                             const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Remove error
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  int clearError(const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Get actuator position, speed, current
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  int getCVP(std::shared_ptr<std::map<std::string, std::vector<double>>> pvcMap,
             bool print_flag = true, const int motor_number = MOTOR_NUMBER_1);

  int recvCVPData(std::shared_ptr<std::vector<double>> vPVC);

  int recvCVPData(
      std::shared_ptr<std::map<std::string, std::vector<double>>> pvcMap);

  /**
   * @brief AIOS test if the encoder is ready or not (If not ready, perform
   * encoder calibration)
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode encoderIsReady(const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS set control mode
   * @param ctrlMode 设置控制模式
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode controlMode(ControlMode ctrlMode,
                                const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief fw2.0 AIOS set input mode
   * @param inMode 设置控制模式
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode inputMode(InputMode inMode,
                              const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS reset linear count
   * @param linearCount 线性计数值
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setLinearCount(int linearCount,
                                   const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS get actuator PID controller parameters
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getMotionCtrlConfig(
      char *recv_buf = nullptr, const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS set actuator PID controller parameters
   * @param param Position proportional gain Speed proportional gain Speed
   *  integral gain Speed limit Speed limit tolerance json参数
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setMotionCtrlConfig(
      const nlohmann::json &param, const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS get actuator MotorConfig parameters
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getMotorConfig(char *recv_buf = nullptr,
                                   const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS set actuator MotorConfig parameter
   * @param param motor current limit, current limit margin, inverter
   * temperature offline, inverter temperature upper limit, current control
   * bandwidth
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setMotorConfig(const nlohmann::json &param,
                                   const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Get the trajectory parameters of actuator trapezoidal mode
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getTrapTraj(char *recv_buf = nullptr,
                                const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Set trajectory parameters of actuator trapezoidal mode
   * @param param Including trapezoidal acceleration, trapezoidal deceleration,
   * trapezoidal speed limit
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setTrapTraj(const nlohmann::json &param,
                                const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS velocity ramp enable
   * @param enable controller mode
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode velRampEnable(bool enable,
                                  const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Speed ramp mode Set target speed
   * @param enable target velocity
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode velRampTarget(const int target,
                                  const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS Set the target position of the trapezoidal motion track
   * @param enable position
   * @param reply_enable reply enable
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode trapezoidalMove(const int position, bool reply_enable,
                                    const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS position control
   * @param pos position
   * @param velocity_ff velocity
   * @param current_ff current feedforward
   * @param reply_enable 输出控制
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setPosition(const int pos, const int velocity_ff,
                                const int current_ff, bool reply_enable,
                                const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS velocity control
   * @param velocity velocity
   * @param current_ff current feedforward
   * @param reply_enable 输出控制
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setVelocity(const int velocity, const int current_ff,
                                bool reply_enable,
                                const int motor_number = MOTOR_NUMBER_1);

  /**
   * @brief AIOS current control
   * @param current current
   * @param reply_enable 输出控制
   * @param motor_number 电机编号
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setCurrent(const int current, bool reply_enable,
                               const int motor_number = MOTOR_NUMBER_1);

  int dumFunc();

  /**
   * @brief IO_Module set IO_State status
   * @param param PWM0_CH PWM1_CH SERVO0 SERVO1 json
   * @param reply_enable 输出控制
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setIOState(const nlohmann::json &param, bool reply_enable);

  /**
   * @brief IO_Module get IO_Staten status
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getIOState();

  /**
   * @brief Aios get network_setting status
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getNetworkSetting(char *recv_buf = nullptr);

  /**
   * @brief Aios set network_setting status
   * @param param 网络设置json参数
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setNetworkSetting(const nlohmann::json &param);

  /******* fw2 .0 *******/
  /**
   * @brief AIOS set input position through pt port
   * @param pos_pt_info 位置相关信息
   * @param cvp_pt_feedback 设置位置后返回的CVP信息
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setInputPositionPt(const PosPtInfoActual &pos_pt_info);
  FourierStatusCode setInputPositionPt(
      const PosPtInfoActual &pos_pt_info,
      std::shared_ptr<CvpPtFeedback> cvp_pt_feedback);

  /**
   * @brief AIOS set input velocity through pt port
   * @param vel_pt_info 速度相关信息
   * @param cvp_pt_feedback 设置位置后返回的CVP信息
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setInputVelocityPt(const VelPtInfo &vel_pt_info);
  FourierStatusCode setInputVelocityPt(
      const VelPtInfo &vel_pt_info,
      std::shared_ptr<CvpPtFeedback> cvp_pt_feedback);

  /**
   * @brief AIOS set input torque through pt port
   * @param torque_pt_info 力矩相关信息
   * @param cvp_pt_feedback 设置位置后返回的CVP信息
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode setInputTorquePt(const TorquePtInfo &torque_pt_info);
  FourierStatusCode setInputTorquePt(
      const TorquePtInfo &torque_pt_info,
      std::shared_ptr<CvpPtFeedback> cvp_pt_feedback);

  /**
   * @brief AIOS get CVP through pt port
   * @param cvp_pt_feedback 返回pvc反馈
   * @return 成功返回0，其他返回-1
   */
  FourierStatusCode getCvpPt();
  FourierStatusCode getCvpPt(std::shared_ptr<CvpPtFeedback> cvp_pt_feedback);
  /******* fw2.0 *******/

 public:
  /**
   * @brief 表示固件版本
   */
  FwVersion fw_version_ = FwVersion::FW_1;

 private:
  static const int sendBufSize = 1024;

  /**
   * 电机编号组
   */
  const std::vector<int> motorNumbers = {MOTOR_NUMBER_0, MOTOR_NUMBER_1};
};

// 秒级延时
void Delay(const double delayTime);

void DelaySelect(const double delayTime);

// 秒级延时
int Delay(const double delayTime, const std::shared_ptr<Aios> aios,
          const std::vector<double> &posList);

const timeval intervalTimeStart();
double intervalTimeEnd(const timeval &tv_last, const float &frequency = 0);

}  // namespace Fourier

// json与自定义struct转换函数
void to_json(nlohmann::json &j, const MotionControllerConfig *cfg);
void from_json(const nlohmann::json &j, MotionControllerConfig *cfg);

void to_json(nlohmann::json &j, const MotorConfig *cfg);
void from_json(const nlohmann::json &j, MotorConfig *cfg);

void to_json(nlohmann::json &j, const TrapTraj *cfg);
void from_json(const nlohmann::json &j, TrapTraj *cfg);

void to_json(nlohmann::json &j, const NetworkSetting *cfg);
void from_json(const nlohmann::json &j, NetworkSetting *cfg);

struct FourierLookup_ {
  std::vector<std::unique_ptr<Fourier::Broadcast_>>
      vBroadcast;  // 广播地址唯一指针数组
  FourierLookupEntryListPtr lookupEntryListPtr;
  strMapPtr ipAndActuatorInfo;  // 存放actuator与CtrlBox信息
  FourierGroupPtr group;
  std::mutex readWriteMu;     // 读写锁,用于锁定ipAndActuatorInfo
  std::mutex boradcastVecMu;  // 用于读写vBroadcast
  std::atomic<bool> exitLookupThreadFlag;  // lookup线程是否结束标志
};

struct FourierLookupEntryList_ {
  std::vector<EntryInfo> entryInfo;
};

struct FourierGroupFeedback_ {
  std::vector<FourierFeedbackPtr> feedback;

  // 第一次send feedback request会请求config信息，请求后将其置为false；
  // 当config修改后，将其置为true，下次请求时会请求最新的config信息
  std::atomic<bool> sendFeedbackRequestConfigFlag;
  std::atomic<bool> getNextFeedbackConfigFlag;
};

struct FourierGroupCommand_ {
  std::vector<FourierCommandPtr> command;
  std::mutex cmd_enum_mutex;  // 多线程访问commandEnum锁
};

struct FourierGroup_ {
  std::vector<std::unique_ptr<Fourier::Aios>> aiosGroup;
  std::mutex aiosGroupMutex;  // 多线程访问aiosGroup锁
  std::timed_mutex aios_feedback_mu;
  FourierLookupPtr lookup;

  FourierGroupFeedback_ g_preFb;  // 存放上一次的位置

  // 为了不把反馈直接写到feedback中，重新new一个struct
  FourierGroupFeedback_ g_fb;
  std::vector<FourierFeedbackErrorPtr> feedbackError;  // 错误的反馈
  std::vector<int> packedLossNum;  // 连续丢包数量，与aiosGroup下标对应
  std::vector<bool> actuatorOnline;
  int maxPackedLossNum;

  std::vector<EntryInfo> entryList;
  AxisState aiosStatus;        // 执行器状态
  FourierCommandEnum preMode;  // 上一次的控制模式

  std::atomic<float> feedbackFrequencyHz;  // 返回频率
  std::thread *fb_thread = nullptr;
  std::mutex fb_mtx;
  std::condition_variable fb_cv;
  bool fb_ready = true;

  // Command Thread
  std::thread *cmd_thread = nullptr;
  std::mutex cmd_mtx;
  std::condition_variable cmd_cv;
  bool cmd_ready = true;

  // exit condition_variable
  std::atomic<bool> exit_signal;

  int32_t lifetime_ms;

  // For xu test 临时功能所需数据结构：输出到达一定位置所用的时间
  float curVelocity;
};
