include_directories(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)

# CLEAR START
add_executable(clear_fault demo_clear_fault.cpp)
target_link_libraries(clear_fault pthread sdk_cpp_fsa)
# CLEAR END

# ===================================================================================
# COMM START
add_executable(comm_config_get demo_comm_config_get.cpp)
target_link_libraries(comm_config_get pthread sdk_cpp_fsa)
add_executable(comm_config_set demo_comm_config_set.cpp)
target_link_libraries(comm_config_set pthread sdk_cpp_fsa)
# COMM END #

# ===================================================================================
# #### CONTROL START
add_executable(control_current_mode demo_control_current_mode.cpp)
target_link_libraries(control_current_mode pthread sdk_cpp_fsa)

add_executable(control_param_get demo_control_param_get.cpp)
target_link_libraries(control_param_get pthread sdk_cpp_fsa)

add_executable(control_param_set demo_control_param_set.cpp)
target_link_libraries(control_param_set pthread sdk_cpp_fsa)

add_executable(control_param_imm_get demo_control_param_imm_get.cpp)
target_link_libraries(control_param_imm_get pthread sdk_cpp_fsa)

add_executable(control_param_imm_set demo_control_param_imm_set.cpp)
target_link_libraries(control_param_imm_set pthread sdk_cpp_fsa)

add_executable(control_position_ff_mode demo_control_position_ff_mode.cpp)
target_link_libraries(control_position_ff_mode pthread sdk_cpp_fsa)

add_executable(control_position_mode demo_control_position_mode.cpp)
target_link_libraries(control_position_mode pthread sdk_cpp_fsa)

add_executable(control_velocity_mode demo_control_velocity_mode.cpp)
target_link_libraries(control_velocity_mode pthread sdk_cpp_fsa)
# #### CONTROL END #

# ===================================================================================
# #### CTRL  START
add_executable(ctrl_config_get demo_ctrl_config_get.cpp)
target_link_libraries(ctrl_config_get pthread sdk_cpp_fsa)

add_executable(ctrl_config_save demo_ctrl_config_save.cpp)
target_link_libraries(ctrl_config_save pthread sdk_cpp_fsa)

add_executable(ctrl_config_set demo_ctrl_config_set.cpp)
target_link_libraries(ctrl_config_set pthread sdk_cpp_fsa)

# #### CTRL  END #
# ===================================================================================
# #### ENABLE  START
add_executable(disable_set demo_disable_set.cpp)
target_link_libraries(disable_set pthread sdk_cpp_fsa)

add_executable(enable_set demo_enable_set.cpp)
target_link_libraries(enable_set pthread sdk_cpp_fsa)

# #### ENABLE  END #
# ===================================================================================
# #### FLAG OF OPERATION  START
add_executable(flag_of_operation_get demo_flag_of_operation_get.cpp)
target_link_libraries(flag_of_operation_get pthread sdk_cpp_fsa)

add_executable(flag_of_operation_set demo_flag_of_operation_set.cpp)
target_link_libraries(flag_of_operation_set pthread sdk_cpp_fsa)
# #### FLAG OF OPERATION  END

# #
# ===================================================================================
# #### GET  START
add_executable(get_abs_encoder_value demo_get_abs_encoder_value.cpp)
target_link_libraries(get_abs_encoder_value pthread sdk_cpp_fsa)

add_executable(get_measured demo_get_measured.cpp)
target_link_libraries(get_measured pthread sdk_cpp_fsa)

add_executable(get_pvc demo_get_pvc.cpp)
target_link_libraries(get_pvc pthread sdk_cpp_fsa)

add_executable(get_pvcc demo_get_pvcc.cpp)
target_link_libraries(get_pvcc pthread sdk_cpp_fsa)

add_executable(get_pvcccc demo_get_pvcccc.cpp)
target_link_libraries(get_pvcccc pthread sdk_cpp_fsa)

add_executable(get_state demo_get_state.cpp)
target_link_libraries(get_state pthread sdk_cpp_fsa)

# #### GET  END #
# ===================================================================================
# #### HOME  START
add_executable(home_offset_get demo_home_offset_get.cpp)
target_link_libraries(home_offset_get pthread sdk_cpp_fsa)

add_executable(home_offset_set demo_home_offset_set.cpp)
target_link_libraries(home_offset_set pthread sdk_cpp_fsa)

add_executable(home_position_set demo_home_position_set.cpp)
target_link_libraries(home_position_set pthread sdk_cpp_fsa)
# #### HOME   END #
# ===================================================================================
# ####  LOOKUP START
add_executable(lookup demo_lookup.cpp)
target_link_libraries(lookup pthread sdk_cpp_fsa)

add_executable(lookup_ctrlbox demo_lookup_ctrlbox.cpp)
target_link_libraries(lookup_ctrlbox pthread sdk_cpp_fsa)

add_executable(lookup_actuator demo_lookup_actuator.cpp)
target_link_libraries(lookup_actuator pthread sdk_cpp_fsa)

add_executable(lookup_abs_encoder demo_lookup_abs_encoder.cpp)
target_link_libraries(lookup_abs_encoder pthread sdk_cpp_fsa)

# ####  LOOKUP END #
# ===================================================================================
# ####  OTA  START
add_executable(ota demo_ota.cpp)
target_link_libraries(ota pthread sdk_cpp_fsa)

add_executable(ota_test demo_ota_test.cpp)
target_link_libraries(ota_test pthread sdk_cpp_fsa)

add_executable(ota_devel demo_ota_devel.cpp)
target_link_libraries(ota_devel pthread sdk_cpp_fsa)

add_executable(ota_cloud demo_ota_cloud.cpp)
target_link_libraries(ota_cloud pthread sdk_cpp_fsa)

add_executable(ota_driver demo_ota_driver.cpp)
target_link_libraries(ota_driver pthread sdk_cpp_fsa)

add_executable(ota_driver_test demo_ota_driver_test.cpp)
target_link_libraries(ota_driver_test pthread sdk_cpp_fsa)

add_executable(ota_driver_cloud demo_ota_driver_cloud.cpp)
target_link_libraries(ota_driver_cloud pthread sdk_cpp_fsa)

add_executable(ota_driver_devel demo_ota_driver_devel.cpp)
target_link_libraries(ota_driver_devel pthread sdk_cpp_fsa)

# ####  OTA  END #
# ===================================================================================
# ####  PID  PARAM START
add_executable(pid_param_set demo_pid_param_set.cpp)
target_link_libraries(pid_param_set pthread sdk_cpp_fsa)

add_executable(pid_param_get demo_pid_param_get.cpp)
target_link_libraries(pid_param_get pthread sdk_cpp_fsa)

add_executable(pid_param_imm_get demo_pid_param_imm_get.cpp)
target_link_libraries(pid_param_imm_get pthread sdk_cpp_fsa)

add_executable(pid_param_imm_set demo_pid_param_imm_set.cpp)
target_link_libraries(pid_param_imm_set pthread sdk_cpp_fsa)
# ####  PID  PARAM END

# #
# ===================================================================================
# ####  REBOOT  STARxT
add_executable(reboot_actuator demo_reboot_actuator.cpp)
target_link_libraries(reboot_actuator pthread sdk_cpp_fsa)

add_executable(reboot demo_reboot.cpp)
target_link_libraries(reboot pthread sdk_cpp_fsa)
# ####  REBOOT  END #
# ===================================================================================

# ####   SET   START
add_executable(set_calibrate_encoder demo_set_calibrate_encoder.cpp)
target_link_libraries(set_calibrate_encoder pthread sdk_cpp_fsa)
# ####   SET   END #

# ===================================================================================
# ####  USER LOG IN
add_executable(set_encrypt demo_set_encrypt.cpp)
target_link_libraries(set_encrypt pthread sdk_cpp_fsa)
# ####  USER LOG IN END

# #
# ===================================================================================
# ####  FSA LIST START add_executable(fsa_list fsa_list.cpp )
add_executable(fsa_list_v2 fsa_list_v2.cpp)
target_link_libraries(fsa_list_v2 pthread sdk_cpp_fsa)
# ####  FSA LIST END

# target_link_libraries(demo_lookup pthread sdk_cpp_fsa)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
