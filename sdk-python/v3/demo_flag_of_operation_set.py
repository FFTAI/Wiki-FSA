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
                'flag_do_calibrate_adc': fsa.FSAFlagState.SET.value,
                'flag_do_calibrate_motor': fsa.FSAFlagState.SET.value,
                'flag_do_calibrate_encoder': fsa.FSAFlagState.SET.value,
                'flag_do_calibrate_direction': fsa.FSAFlagState.SET.value,
                'flag_do_calibrate_offset': fsa.FSAFlagState.SET.value,

                'flag_do_use_store_motor_param': fsa.FSAFlagState.SET.value,
                'flag_do_use_store_encoder_param': fsa.FSAFlagState.SET.value,
                'flag_do_use_store_pid_param': fsa.FSAFlagState.SET.value,
                'flag_do_use_store_control_param': fsa.FSAFlagState.SET.value,
            }
            fsa.set_flag_of_operation(server_ip_list[i], dict)

        print('\n')
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fsa.get_config(server_ip_list[i])

        print('\n')


if __name__ == '__main__':
    main()
