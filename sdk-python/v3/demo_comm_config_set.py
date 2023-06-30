import fsa
import time

server_ip_list = []


def main():
    server_ip_list = fsa.broadcast_func()

    if server_ip_list:

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fsa.get_comm_config(server_ip_list[i])

        print('\n')
        time.sleep(1)

        # set the communication configuration of all FAS
        # Notice : set static_IP should config DHCP_enable to be False! Otherwise, it will not work!
        for i in range(len(server_ip_list)):
            dict = {
                'name': 'FAS-' + str(i),
                'DHCP_enable': True,
                'SSID': 'FSA',
                'password': 'fftai2015',
                'static_IP': [192, 168, 31, i + 50],
                'gateway': [192, 168, 31, 1],
                'subnet_mask': [255, 255, 255, 0],
                'dns_1': [114, 114, 114, 114],
                'dns_2': [8, 8, 8, 8]
            }
            fsa.set_comm_config(server_ip_list[i], dict)

        print('\n')
        time.sleep(1)

        # get the communication configuration of all FAS
        for i in range(len(server_ip_list)):
            fsa.get_comm_config(server_ip_list[i])

        print('\n')


if __name__ == '__main__':
    main()
