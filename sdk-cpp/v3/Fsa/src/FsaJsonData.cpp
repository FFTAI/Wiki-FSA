#include "FsaJsonData.h"
#include "FsaStatus.h"
namespace FSA_CONNECT {
namespace JsonData {
ordered_json get_root_config_json = {{"method", "GET"}, {"reqTarget", "/"}, {"property", ""}};

ordered_json get_state_json = {{"method", "GET"}, {"reqTarget", "/state"}, {"property", ""}};

ordered_json enable_json = {{"method", "SET"}, {"reqTarget", "/control_word"}, {"property", ""}, {"control_word", 1}};

ordered_json disable_json = {{"method", "SET"}, {"reqTarget", "/control_word"}, {"property", ""}, {"control_word", 1}};

ordered_json set_control_mode_json = {
    {"method", "SET"},
    {"reqTarget", "/config"},
    {"property", ""},
    {"actuator_type", Status::FSAActuatorType::TYPE_60_ACB_120},
    {"actuator_reduction_ratio", 1},
    {"motor_index", 1},
    {"motor_vbus", 1},
    {"motor_direction", 1},
    {"motor_pole_pairs", 1},
    {"motor_max_speed", 1},
    {"encoder_direction", 1},
    {"encoder_resolution", 1},
    {"encoder_phase_offset", 0}};

ordered_json get_control_mode_json = {{"method", "GET"}, {"reqTarget", "/config"}, {"property", ""}};

ordered_json set_pid_params_json = {
    {"method", "SET"},
    {"reqTarget", "/pid_param"},
    {"property", ""},
    {"control_position_kp", 200.0},
    {"control_velocity_kp", 0.04},
    {"control_velocity_ki", 0.002},
    {"control_current_kp", 0.02},
    {"control_current_ki", 0.0015},
    {"control_position_output_max", 3000},
    {"control_position_output_min", -3000},
    {"control_velocity_output_max", 6.0},
    {"control_velocity_output_min", -6.0},
    {"control_current_output_max", 6.0},
    {"control_current_output_min", -6.0}};

ordered_json get_pid_params_json = {{"method", "GET"}, {"reqTarget", "/pid_param"}, {"property", ""}};

ordered_json get_cvp_json = {{"method", "GET"}, {"reqTarget", "/m1/CVP"}};

ordered_json get_trapezoidal_trajectory_param_json = {{"method", "GET"}, {"reqTarget", "/m1/trap_traj"}};

ordered_json set_trapezoidal_trajectory_param_json = {{"method", "SET"},
                                                      {"reqTarget", "/m1/trap_traj"},
                                                      {"accel_limit", 20000},
                                                      {"decel_limit", 20000},
                                                      {"vel_limit", 60000}};

ordered_json get_motion_controller_config_json = {{"method", "GET"}, {"reqTarget", "/m1/controller/config"}};

ordered_json set_motion_controller_config_json = {
    {"method", "SET"}, {"reqTarget", "/m1/controller/config"}, {"pos_gain", 20.0}, {"vel_gain", 0.0005}, {"vel_integrator_gain", 0.0002}, {"vel_limit", 400000}, {"vel_limit_tolerance", 1.2}};

ordered_json move_to_json = {{"method", "SET"}, {"reqTarget", "/m1/trapezoidalMove"}, {"property", 0}, {"reply_enable", false}};

ordered_json set_linear_count_json = {{"method", "SET"}, {"reqTarget", "/m1/encoder"}, {"set_linear_count", 0}};

ordered_json ext_pos_json = {
    {"method", "SET"}, {"reqTarget", "/m1/setPosition"}, {"position", 0}, {"velocity_ff", 0}, {"current_ff", 0}};

ordered_json ext_vel_json = {{"method", "SET"}, {"reqTarget", "/m1/setVelocity"}, {"velocity", 0}, {"current_ff", 0}};

ordered_json ext_cur_json = {
    {"method", "SET"},
    {"reqTarget", "/m1/setCurrent"},
    {"current", 0},
};

ordered_json vel_ramp_enable_json = {
    {"method", "SET"},
    {"reqTarget", "/m1/controller"},
    {"vel_ramp_enable", true},
};

ordered_json set_vel_ramp_target_json = {
    {"method", "SET"},
    {"reqTarget", "/m1/controller"},
    {"vel_ramp_target", 0},
};
} // namespace JsonData
} // namespace FSA_CONNECT