include_directories(${PROJECT_SOURCE_DIR}/include)

#### CLEAR START
add_executable(demo_clear_fault demo_clear_fault.cpp fsa.cpp os.cpp logger.cpp)
#### CLEAR END
# ===================================================================================
#### COMM START
add_executable(demo_comm_config_get demo_comm_config_get.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_comm_config_set demo_comm_config_set.cpp fsa.cpp os.cpp logger.cpp)
#### COMM END
# ===================================================================================
#### CONTROL START

add_executable(demo_control_param_get demo_control_param_get.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_control_param_imm_get demo_control_param_imm_get.cpp fsa.cpp os.cpp logger.cpp)


#### CONTROL END
# ===================================================================================
#### CTRL  START

#### CTRL  END
# ===================================================================================
#### ENABLE  START

#### ENABLE  END
# ===================================================================================
#### FLAG OF OPERATION  START

#### FLAG OF OPERATION  END

# ===================================================================================
#### GET  START
add_executable(demo_get_measured demo_get_measured.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_get_pvc demo_get_pvc.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_get_pvcc demo_get_pvcc.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_get_pvcccc demo_get_pvcccc.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_get_state demo_get_state.cpp fsa.cpp os.cpp logger.cpp)
#### GET  END
# ===================================================================================
#### HOME  START

#### HOME   END
# ===================================================================================
####  LOOKUP START
add_executable(demo_lookup demo_lookup.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_lookup_ctrlbox demo_lookup_ctrlbox.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_lookup_actuator demo_lookup_actuator.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_lookup_abs_encoder demo_lookup_abs_encoder.cpp fsa.cpp os.cpp logger.cpp)
####  LOOKUP END

# ===================================================================================
####  OTA  START
add_executable(demo_ota demo_ota.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_ota_test demo_ota_test.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_ota_devel demo_ota_devel.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_ota_cloud demo_ota_cloud.cpp fsa.cpp os.cpp logger.cpp)

add_executable(demo_ota_driver demo_ota_driver.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_ota_driver_test demo_ota_driver_test.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_ota_driver_cloud demo_ota_driver_cloud.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_ota_driver_devel demo_ota_driver_devel.cpp fsa.cpp os.cpp logger.cpp)
####  OTA  END
# ===================================================================================
####  PID  PARAM START

####  PID  PARAM END

# ===================================================================================
####  REBOOT  START
add_executable(demo_reboot_actuator demo_reboot_actuator.cpp fsa.cpp os.cpp logger.cpp)
add_executable(demo_reboot demo_reboot.cpp fsa.cpp os.cpp logger.cpp)
####  REBOOT  END
# ===================================================================================

####   SET   START

####   SET   END
# ===================================================================================
####  USER LOG IN
add_executable(demo_set_encrypt demo_set_encrypt.cpp fsa.cpp os.cpp logger.cpp)
####  USER LOG IN END



set (EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)