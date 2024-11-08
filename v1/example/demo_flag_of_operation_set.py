from fi_fsa import fi_fsa_v1
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa_v1.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # set the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            dict = {
                "flag_do_use_store_motor_param": fi_fsa_v1.FSAFlagState.CLEAR,
                "flag_do_use_store_pid_param": fi_fsa_v1.FSAFlagState.CLEAR,
                # 'flag_do_use_store_motor_param': fi_fsa_v1.FSAFlagState.SET,
                # 'flag_do_use_store_pid_param': fi_fsa_v1.FSAFlagState.SET,
            }
            fi_fsa_v1.set_flag_of_operation(server_ip_list[i], dict)

        print("\n")
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa_v1.get_flag_of_operation(server_ip_list[i])

        print("\n")

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fi_fsa_v1.reboot(server_ip_list[i])


if __name__ == "__main__":
    main()
