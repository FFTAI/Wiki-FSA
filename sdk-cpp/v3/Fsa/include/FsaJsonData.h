#pragma once

#include "nlohmann/json.hpp"

using namespace nlohmann;

namespace FSA_CONNECT {
namespace JsonData {
extern ordered_json get_root_config_json;
extern ordered_json get_state_json;
extern ordered_json enable_json;
extern ordered_json disable_json;
extern ordered_json set_control_mode_json;
extern ordered_json get_control_mode_json;
extern ordered_json set_pid_params_json;
extern ordered_json get_pid_params_json;
extern ordered_json get_cvp_json;
extern ordered_json get_trapezoidal_trajectory_param_json;
extern ordered_json set_trapezoidal_trajectory_param_json;
extern ordered_json get_motion_controller_config_json;
extern ordered_json set_motion_controller_config_json;
extern ordered_json set_linear_count_json;
extern ordered_json ext_pos_json;
extern ordered_json ext_vel_json;
extern ordered_json ext_cur_json;
extern ordered_json vel_ramp_enable_json;
extern ordered_json set_vel_ramp_target_json;
} // namespace JsonData
} // namespace FSA