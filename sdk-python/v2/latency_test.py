import aios
import time
import threading
import numpy as np

Server_IP_list = ['192.168.2.15']


def main():

    Server_IP_list = aios.broadcast_func()
    
    if Server_IP_list:

        for i in range(len(Server_IP_list)):
            aios.getRoot(Server_IP_list[i])

        for i in range(10000):
            start = time.time()
            for j in range(len(Server_IP_list)):
                aios.dum_func(Server_IP_list[j])
            for j in range(len(Server_IP_list)):
                aios.receive_func()
            print((time.time() - start)*1000)
            time.sleep(0.005)


if __name__ == '__main__':
    main()
