import numpy

import fi_fsa_v2
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fi_fsa.get_home_offset(server_ip_list[i])

        print('\n')


if __name__ == '__main__':
    main()
