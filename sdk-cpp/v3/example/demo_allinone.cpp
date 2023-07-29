#include "Fsa.h"

int main() {
    string ip = "192.168.137.163";
    FSA_CONNECT::LOG::LOG mylog;

    mylog.setLevel(spdlog::level::level_enum::info);
    FSA_CONNECT::FSAConfig::FSAControlConfig motor1;
    FSA_CONNECT::FSAConfig::FSAPIDParams pid1;
    motor1.actuator_type = FSA_CONNECT::Status::TYPE_60_ACB_120;
    // motor1.actuator_reduction_ratio = 7;
    // motor1.motor_index = 1;
    // motor1.motor_vbus = 36;
    // motor1.motor_direction = FSA_CONNECT::Status::ACB;
    // motor1.motor_pole_pairs = 21;
    // motor1.motor_max_speed = 3000;

    // motor1.encoder_direction = 1;
    // motor1.encoder_resolution = 4000;
    // motor1.encoder_phase_offset = 0;

    FSA_CONNECT::FSA fsa(ip);

    fi_fsa.Enable();
    // fi_fsa.Disable();
    // fi_fsa.GetControlConfig();
    // fi_fsa.GetRootConfig();
    // fi_fsa.SetPIDParams(pid1);
    // fi_fsa.GetPIDParams();
    fi_fsa.EnableVelControl();
    double pos;
    double vel;
    double cur;
    while (1) {
        fi_fsa.SetVelocity(2, 0);
        usleep(2000);
        fi_fsa.GetPVC(pos, vel, cur);
        std::cout << "pos: " << pos << "vel: " << vel << "cur: " << cur << std::endl;
    }

    return 0;
}