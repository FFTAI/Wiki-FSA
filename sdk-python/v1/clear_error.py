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
            aios.getError(Server_IP_list[i], 1)
            aios.getError(Server_IP_list[i], 0)
            # aios.clearError(Server_IP_list[i], 1)
            # aios.clearError(Server_IP_list[i], 0)
        print('\n')


if __name__ == '__main__':
    main()