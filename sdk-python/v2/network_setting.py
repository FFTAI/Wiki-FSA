import aios
import time
import threading
import numpy as np
import json

Server_IP_list = ['192.168.100.12']


def main():
    Server_IP_list = aios.broadcast_func()

    if Server_IP_list:

        for i in range(len(Server_IP_list)):
            aios.getNetworkSetting(Server_IP_list[i])
        print('\n')
        time.sleep(1)

        for i in range(len(Server_IP_list)):
            name = 'AXIS-' + str(i)
            dict = {
                'DHCP_enable': False,
                'SSID': 'your_ssid',
                'password': 'your_password',
                'name': name,
                'staticIP': [192, 168, 2, i + 50],
                'gateway': [192, 168, 2, 1],
                'subnet': [255, 255, 255, 0],
                'dns_1': [114, 114, 114, 114],
                'dns_2': [8, 8, 8, 8]
            }
            aios.setNetworkSetting(dict, Server_IP_list[i])
        print('\n')
        time.sleep(1)

        for i in range(len(Server_IP_list)):
            aios.getNetworkSetting(Server_IP_list[i])
        print('\n')


if __name__ == '__main__':
    main()
