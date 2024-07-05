from fi_fsa import fi_fsa_v1
import time
from fi_fsa_v1 import fi_fsa_v1_predefine

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v1.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = {
                "actuator_type": fi_fsa_predefine.FSAActuatorType.TYPE_DEFAULT,
                "actuator_direction": fi_fsa_predefine.FSAActuatorDirection.DIRECTION_NORMAL,
                "actuator_reduction_ratio": fi_fsa_predefine.FSAActuatorReductionRatio.REDUCTION_RATIO_36,
                "motor_type": fi_fsa_predefine.FSAMotorType.FSA36_10V0,
                "motor_hardware_type": fi_fsa_predefine.FSAHardwareType.TYPE_H46V104,
                "motor_vbus": fi_fsa_predefine.FSAMotorVBUS.VBUS_48V,
                "motor_direction": fi_fsa_predefine.FSAMotorDirection.ABC,
                "motor_max_speed": fi_fsa_predefine.FSAMotorMaxSpeed.MAX_SPEED_3000,
                "motor_max_acceleration": fi_fsa_predefine.FSAMotorMaxAcceleration.MAX_ACCELERATION_60000,
                "motor_max_current": 100,
                "actuator_comm_hardware_type": fi_fsa_predefine.FSAActuatorCommHardwareType.TYPE_NULL,
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
