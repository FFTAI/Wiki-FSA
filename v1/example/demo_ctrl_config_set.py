from fi_fsa import fi_fsa_v1, fi_fsa_predefine_v1
import time

# set a fsa ip e.g. server_ip_list = ["192.168.137.9"]
server_ip_list = ["192.168.137.9"]

def main():
    # server_ip_list = fi_fsa_v1.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        match_version = "2.14.2"
        for i in range(len(server_ip_list)):
            firmware_version = fi_fsa_v1.get_comm_firmware_version(server_ip_list[i])
            if fi_fsa_v1.version_compare(firmware_version, match_version) < 0:
                print("the firmware version of %s is %s, less than the match version %s" % (
                    server_ip_list[i], firmware_version, match_version))
                return

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = {
                "actuator_type": fi_fsa_predefine_v1.FSAActuatorType.TYPE_DEFAULT,
                "actuator_direction": fi_fsa_predefine_v1.FSAActuatorDirection.DIRECTION_NORMAL,
                "actuator_reduction_ratio": fi_fsa_predefine_v1.FSAActuatorReductionRatio.REDUCTION_RATIO_36,
                "motor_type": fi_fsa_predefine_v1.FSAMotorType.FSA36_10V0,
                "motor_hardware_type": fi_fsa_predefine_v1.FSAHardwareType.TYPE_H46V104,
                "motor_vbus": fi_fsa_predefine_v1.FSAMotorVBUS.VBUS_48V,
                "motor_direction": fi_fsa_predefine_v1.FSAMotorDirection.ABC,
                "motor_max_speed": fi_fsa_predefine_v1.FSAMotorMaxSpeed.MAX_SPEED_3000,
                "motor_max_acceleration": fi_fsa_predefine_v1.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                "motor_max_current": 100,
                "actuator_comm_hardware_type": fi_fsa_predefine_v1.FSAActuatorCommHardwareType.TYPE_NULL,
            }
            fi_fsa_v1.set_config(server_ip_list[i], dict)

        print("\n")
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa_v1.get_config(server_ip_list[i])

        print("\n")

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fi_fsa_v1.reboot(server_ip_list[i])


if __name__ == "__main__":
    main()
