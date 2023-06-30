import aios
import time
import threading
import numpy as np

Server_IP_list = []



def main():

    Server_IP_list = aios.broadcast_func()
    
    if Server_IP_list:

        for i in range(len(Server_IP_list)):
            aios.eraseConfig(Server_IP_list[i])
        
        time.sleep(1)
        
        for i in range(len(Server_IP_list)):    
            aios.getEncoderInfo(Server_IP_list[i])

        print('\n')




if __name__ == '__main__':
    main()
