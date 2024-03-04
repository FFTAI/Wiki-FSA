import fi_fsa
import time

server_ip_list = []


def main():
    server_ip_list = fi_fsa.broadcast_func_with_filter(filter_type="Actuator")
    version_str = '0.1.10.3'
    if server_ip_list:

        for i in range(len(server_ip_list)):
            fi_fsa.ota_driver_custom(server_ip_list[i], version_str)

        print('\n')
        time.sleep(1)


if __name__ == '__main__':
    main()
