import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_pid_param(server_ip_list[i])

        print('\n')
        time.sleep(1)

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = { #36
                'control_position_kp': 0.5,
                'control_velocity_kp': 0.002,
                'control_velocity_ki': 0.00001,
                'control_current_kp': 0.0,  # not work for now
                'control_current_ki': 0.0,  # not work for now
            }
            dict = { #60
                'control_position_kp': 0.08,
                'control_velocity_kp': 0.04,
                'control_velocity_ki': 0.0001,
                'control_current_kp': 0.0,  # not work for now
                'control_current_ki': 0.0,  # not work for now
            }
            fi_fsa.set_pid_param(server_ip_list[i], dict)

        # print('\n')
        # time.sleep(1)

        # # get the communication configuration of all FAS
        # for i in range(len(server_ip_list)):
        #     fi_fsa.get_config(server_ip_list[i])

        # print('\n')

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.reboot(server_ip_list[i])


if __name__ == '__main__':
    main()
