import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = {
                'actuator_type': fi_fsa.FSAActuatorType.TYPE_DEFAULT,
                'actuator_direction': fi_fsa.FSAActuatorDirection.DIRECTION_NORMAL,
                'actuator_reduction_ratio': fi_fsa.FSAActuatorReductionRatio.REDUCTION_RATIO_36,
                'motor_type': fi_fsa.FSAMotorType.FSA36_10V0,
                'motor_hardware_type': fi_fsa.FSAHardwareType.TYPE_H46V104,
                'motor_direction': fi_fsa.FSAMotorDirection.ABC,
                'motor_max_current': 100
            }
            fi_fsa.set_config(server_ip_list[i], dict)

        print('\n')
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_config(server_ip_list[i])

        print('\n')

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.reboot(server_ip_list[i])


if __name__ == '__main__':
    main()
