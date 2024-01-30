include_directories(${PROJECT_SOURCE_DIR}/include)

#### CLEAR START
add_executable(clear_fault demo_clear_fault.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(clear_fault pthread)
#### CLEAR END
# ===================================================================================
#### COMM START
add_executable(comm_config_get demo_comm_config_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(comm_config_get pthread)
add_executable(comm_config_set demo_comm_config_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(comm_config_set pthread)
#### COMM END
# # ===================================================================================
# #### CONTROL START
add_executable(control_current_mode demo_control_current_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_current_mode pthread)

add_executable(control_param_get demo_control_param_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_param_get pthread)

add_executable(control_param_set demo_control_param_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_param_set pthread)

add_executable(control_param_imm_get demo_control_param_imm_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_param_imm_get pthread)

add_executable(control_param_imm_set demo_control_param_imm_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_param_imm_set pthread)

add_executable(control_position_ff_mode demo_control_position_ff_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_position_ff_mode pthread)

add_executable(control_position_mode demo_control_position_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_position_mode pthread)

add_executable(control_velocity_mode demo_control_velocity_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(control_velocity_mode pthread)
# #### CONTROL END
# # ===================================================================================
# #### CTRL  START
add_executable(ctrl_config_get demo_ctrl_config_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ctrl_config_get pthread)

add_executable(ctrl_config_save demo_ctrl_config_save.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ctrl_config_save pthread)

add_executable(ctrl_config_set demo_ctrl_config_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ctrl_config_set pthread)

# #### CTRL  END
# # ===================================================================================
# #### ENABLE  START
add_executable(disable_set demo_disable_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(disable_set pthread)

add_executable(enable_set demo_enable_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(enable_set pthread)

# #### ENABLE  END
# # ===================================================================================
# #### FLAG OF OPERATION  START
add_executable(flag_of_operation_get demo_flag_of_operation_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(flag_of_operation_get pthread)

add_executable(flag_of_operation_set demo_flag_of_operation_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(flag_of_operation_set pthread)
# #### FLAG OF OPERATION  END

# # ===================================================================================
# #### GET  START
add_executable(get_abs_encoder_value demo_get_abs_encoder_value.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(get_abs_encoder_value pthread)

add_executable(get_measured demo_get_measured.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(get_measured pthread)

add_executable(get_pvc demo_get_pvc.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(get_pvc pthread)

add_executable(get_pvcc demo_get_pvcc.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(get_pvcc pthread)

add_executable(get_pvcccc demo_get_pvcccc.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(get_pvcccc pthread)

add_executable(get_state demo_get_state.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(get_state pthread)

# #### GET  END
# # ===================================================================================
# #### HOME  START
add_executable(home_offset_get demo_home_offset_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(home_offset_get pthread)

add_executable(home_offset_set demo_home_offset_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(home_offset_set pthread)

add_executable(home_position_set demo_home_position_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(home_position_set pthread)
# #### HOME   END
# # ===================================================================================
# ####  LOOKUP START
add_executable(lookup demo_lookup.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(lookup pthread)

add_executable(lookup_ctrlbox demo_lookup_ctrlbox.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(lookup_ctrlbox pthread)

add_executable(lookup_actuator demo_lookup_actuator.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(lookup_actuator pthread)

add_executable(lookup_abs_encoder demo_lookup_abs_encoder.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(lookup_abs_encoder pthread)

# ####  LOOKUP END
# # ===================================================================================
# ####  OTA  START
add_executable(ota demo_ota.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota pthread)

add_executable(ota_test demo_ota_test.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_test pthread)

add_executable(ota_devel demo_ota_devel.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_devel pthread)

add_executable(ota_cloud demo_ota_cloud.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_cloud pthread)

add_executable(ota_driver demo_ota_driver.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_driver pthread)

add_executable(ota_driver_test demo_ota_driver_test.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_driver_test pthread)

add_executable(ota_driver_cloud demo_ota_driver_cloud.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_driver_cloud pthread)

add_executable(ota_driver_devel demo_ota_driver_devel.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(ota_driver_devel pthread)

# ####  OTA  END
# # ===================================================================================
# ####  PID  PARAM START
add_executable(pid_param_set demo_pid_param_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(pid_param_set pthread)

add_executable(pid_param_get demo_pid_param_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(pid_param_get pthread)

add_executable(pid_param_imm_get demo_pid_param_imm_get.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(pid_param_imm_get pthread)

add_executable(pid_param_imm_set demo_pid_param_imm_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(pid_param_imm_set pthread)
# ####  PID  PARAM END

# # ===================================================================================
# ####  REBOOT  STARxT
add_executable(reboot_actuator demo_reboot_actuator.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(reboot_actuator pthread)

add_executable(reboot demo_reboot.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(reboot pthread)
# ####  REBOOT  END
# # ===================================================================================

# ####   SET   START
add_executable(set_calibrate_encoder demo_set_calibrate_encoder.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(set_calibrate_encoder pthread)
# ####   SET   END
# # ===================================================================================
# ####  USER LOG IN
add_executable(set_encrypt demo_set_encrypt.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(set_encrypt pthread)
# ####  USER LOG IN END

# # ===================================================================================
# ####  FSA LIST START
# add_executable(fsa_list fsa_list.cpp fsa.cpp os.cpp logger.cpp)
add_executable(fsa_list_v2 fsa_list_v2.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fsa_list_v2 pthread)
# ####  FSA LIST END

# #####################添加fast接口#########################

add_executable(fast_disable_set demo_fast_disable_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_disable_set pthread)

add_executable(fast_enable_set demo_fast_enable_set.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_enable_set pthread)

add_executable(fast_get_pvc demo_fast_get_pvc.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_get_pvc pthread)

add_executable(fast_get_error demo_fast_get_error.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_get_error pthread)

add_executable(fast_set_clear_fault demo_fast_set_clear_fault.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_set_clear_fault pthread)

add_executable(fast_control_position_mode demo_fast_control_position_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_control_position_mode pthread)

add_executable(fast_control_velocity_mode demo_fast_control_velocity_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_control_velocity_mode pthread)

add_executable(fast_control_current_mode demo_fast_control_current_mode.cpp fsa.cpp os.cpp logger.cpp)
target_link_libraries(fast_control_current_mode pthread)

set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)