#pragma once

#include <string>
namespace FSA_CONNECT {
namespace FSAConfig {
struct FSAPIDParams {
    double gear_ratio;
    double pos_gain;
    double vel_gain;
    double vel_integrator_gain;
    double pos_max_limit;
    double pos_min_limit;
    double vel_limit;
    double vel_limit_tolerance;
    double torque_limit;
};

struct FSAControlConfig {
    int actuator_type;
    int actuator_reduction_ratio;
    int motor_index;
    double motor_vbus;
    int motor_direction;
    int motor_pole_pairs;
    double motor_max_speed;
    int encoder_direction;
    double encoder_resolution;
    double encoder_phase_offset;
};

// struct FSARootConfig {
//     std::string serial_number;
//     std::string connect_mode;
//     std::string name;
//     std::string model;
//     // double gear_ratio;
//     int vbus_voltage;
//     int motor_temp_m1;
//     int inverter_temp_m1;
//     std::vector<std::string> hw_version;
//     std::vector<std::string> fw_version;
// };

} // namespace FSAConfig
} // namespace FSA_CONNECT