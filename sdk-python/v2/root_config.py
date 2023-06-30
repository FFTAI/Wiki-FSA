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
        print('\n')

        for i in range(len(Server_IP_list)):
            aios.getRootConfig(Server_IP_list[i])
        print('\n')

        dict = {
            'dc_bus_overvoltage_trip_level' : 55,
            'dc_bus_undervoltage_trip_level' : 10,
        }
        for i in range(len(Server_IP_list)):
            aios.setRootConfig(dict, Server_IP_list[i])
        print('\n')

        for i in range(len(Server_IP_list)):
            aios.saveConfig(Server_IP_list[i])
            # aios.rebootMotorDrive(Server_IP_list[i])

        print('\n')
        time.sleep(2)

        for i in range(len(Server_IP_list)):
            aios.getRootConfig(Server_IP_list[i])
        print('\n')




if __name__ == '__main__':
    main()
