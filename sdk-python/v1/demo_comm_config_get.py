import fi_fsa_v1 as fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter("Actuator")

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_comm_config(server_ip_list[i])

        print('\n')
        time.sleep(1)


if __name__ == '__main__':
    main()