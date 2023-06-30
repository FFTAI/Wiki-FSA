import aios
import time
import threading
import numpy as np

Server_IP_list = []


def main():

    Server_IP_list = aios.broadcast_func()
    
    if Server_IP_list:

        for i in range(len(Server_IP_list)):
            aios.getRoot(Server_IP_list[i])

        for i in range(len(Server_IP_list)):
            aios.reboot(Server_IP_list[i])
            # aios.rebootMotorDrive(Server_IP_list[i])
        print('\n')

    #     Server_IP_list = aios.broadcast_func()
    




if __name__ == '__main__':
    main()
