import numpy

import fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func_with_filter(filter_type="Actuator")

    if server_ip_list:

        for i in range(len(server_ip_list)):
            fsa.set_home_position(server_ip_list[i])

        print('\n')


if __name__ == '__main__':
    main()
