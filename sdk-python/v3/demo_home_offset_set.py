import numpy

import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        # get the communication configuration of all FAS
        home_offset = numpy.ones(len(server_ip_list)) * 10

        for i in range(len(server_ip_list)):
            fsa.set_home_offset(server_ip_list[i], home_offset[i])

        print('\n')

        # reboot all FAS
        for i in range(len(server_ip_list)):
            fsa.reboot(server_ip_list[i])


if __name__ == '__main__':
    main()
