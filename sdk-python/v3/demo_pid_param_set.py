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
                'position_control_kp': 50.0,
                'velocity_control_kp': 0.04,
                'velocity_control_ki': 0.0002,
                'current_control_kp': 0.02,
                'current_control_ki': 0.00015,
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
