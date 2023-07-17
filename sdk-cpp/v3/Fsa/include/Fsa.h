#pragma once
#include "FsaConfig.h"
#include "FsaJsonData.h"
#include "FsaLog.h"
#include "FsaResult.h"
#include "FsaSocket.h"
#include "FsaStatus.h"
// #include "peripheral/abs_encoder/abs_encoder.h"
// #include "udp_socket/include/udp_socket.h"
#include <chrono>
#include <deque>
#include <memory>
#include <sys/socket.h>
#define MAX_UDP_RECV_DATA_NUM_PER_CYCLE 20

using namespace std;

namespace FSA_CONNECT {

class FSA {
  public:
    // constructor
    FSA(const string &ip);
    // FSA &operator=(const FSA &otherAIOS);

    int GetRootConfig();
    int GetState();
    int Enable();
    int Disable();
    // int SetConfig(const FSAConfig::FSAMotionConfig &motion_controller_config);

    bool is_enabled = false;

    std::shared_ptr<Transmit::UDPSocket> ctrl_udp_socket;
    std::shared_ptr<Transmit::UDPSocket> root_udp_socket;
    int SetControlConfig(const FSAConfig::FSAControlConfig &config);
    int SetPIDParams(const FSAConfig::FSAPIDParams &pidparams);
    int GetPIDParams();
    int GetControlConfig();
    int SetPosition(const double &pos, const double &vel_ff, const double &cur_ff);
    int SetVelocity(const double &vel, const double &cur_ff);
    int SetCurrent(const double &cur);
    int GetPVC(double &pos, double &vel, double &cur);
    int EnablePosControl();
    int EnableVelControl();
    int EnableCurControl();
    // std::shared_ptr<Transmit::UDPSocket> pt_udp_socket;

  private:
    int get_root_config = 0;
    int get_state = 0;
    int poweron_state = 0;
    int poweroff_state = 0;
    int set_ctrlcfg_state = 0;
    int get_ctrlcfg_state = 0;
    int set_pid_state = 0;
    int get_pid_state = 0;
    int set_pos_state = 0;
    int set_vel_state = 0;
    int set_cur_state = 0;
    int getpvc_state = 0;
    int control_state = 0;

    std::string ip_;
    std::chrono::steady_clock::time_point begin;
    std::chrono::steady_clock::time_point end;
    std::chrono::milliseconds int_ms;
    bool motor_drive_ready_;
};

} // namespace FSA_CONNECT