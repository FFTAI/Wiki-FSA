import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_config(server_ip_list[i])

        print('\n')
        time.sleep(1)

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = {
                # Notice:
                # if use TYPE_DEFAULT, the following parameters will be used,
                # otherwise, the parameters will be used according to the type.

                # 'actuator_type': fi_fsa.FSAActuatorType.TYPE_130_21_A_7,

                'actuator_type': fi_fsa.FSAActuatorType.TYPE_DEFAULT,
                'actuator_direction': fi_fsa.FSAActuatorDirection.DIRECTION_NORMAL,
                'actuator_reduction_ratio': fi_fsa.FSAActuatorReductionRatio.REDUCTION_RATIO_120,

                'motor_type': 0,
                'motor_hardware_type': 0,
                'motor_vbus': fi_fsa.FSAMotorVBUS.VBUS_36V,
                'motor_direction': fi_fsa.FSAMotorDirection.ACB,
                'motor_pole_pairs': fi_fsa.FSAMotorPolePairs.POLE_PAIRS_21,
                'motor_max_speed': fi_fsa.FSAMotorMaxSpeed.MAX_SPEED_3000,  # rpm
                'motor_max_acceleration': 60000,  # rpm/s
                'motor_max_current': 10,  # A

                'encoder_direction': fi_fsa.FSAEncoderDirection.DIRECTION_CCW,
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
