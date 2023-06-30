import aios
import time
import threading
import numpy as np
import json

Server_IP_list = ['192.168.5.59']

def main():

    Server_IP_list = aios.broadcast_func()
    
    if Server_IP_list:

        for i in range(10000):
            start = time.time()
            for i in range(len(Server_IP_list)):
                cvp = aios.getCVP(Server_IP_list[i], 1)
                print("Position = %f, Velocity = %f, Current = %.4f" %(cvp[0], cvp[1], cvp[2]))
            latency = time.time() - start
            print(latency*1000)

            time.sleep(0.005)




if __name__ == '__main__':
    main()
