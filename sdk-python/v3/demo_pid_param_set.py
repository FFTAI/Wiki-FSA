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
                'control_position_kp': 200.0,
                'control_velocity_kp': 0.04,
                'control_velocity_ki': 0.0002,
                'control_current_kp': 0.02,
                'control_current_ki': 0.00015,

                'control_position_output_max': 3000,
                'control_position_output_min': -3000,
                'control_velocity_output_max': 6.0,
                'control_velocity_output_min': -6.0,
                'control_current_output_max': 6.0,
                'control_current_output_min': -6.0,
            }
            fsa.set_pid_param(server_ip_list[i], dict)

        print('\n')
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        print('\n')


if __name__ == '__main__':
    main()
