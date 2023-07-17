import fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func()

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        print('\n')
        time.sleep(1)

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = {
                # Notice:
                # if use TYPE_DEFAULT, the following parameters will be used,
                # otherwise, the parameters will be used according to the type.
                'actuator_type': fsa.FSAActuatorType.TYPE_DEFAULT.value,
                # 'actuator_type': fsa.FSAActuatorType.TYPE_60_ACB_1.value,
                # 'actuator_type': fsa.FSAActuatorType.TYPE_60_ACB_120.value,

                'actuator_reduction_ratio': 30,
                
                'motor_index': 1,
                'motor_vbus': 36,
                'motor_direction': fsa.FSAMotorDirection.ABC.value,
                'motor_pole_pairs': 21,
                'motor_max_speed': 3000,  # [rpm]

                'encoder_direction': -1,
                'encoder_resolution': 4000,  # 4000 pulse/rev
                'encoder_phase_offset': 0,
            }
            fsa.set_config(server_ip_list[i], dict)

        print('\n')
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        print('\n')


if __name__ == '__main__':
    main()
